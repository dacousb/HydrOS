#include <task/sched.h>
#include <mem/kheap.h>
#include <mem/virt.h>
#include <string/string.h>
#include <kernel/panic.h>
#include <kernel/timer.h>
#include <kernel/ports.h>
#include <kernel/irq.h>
#include <stddef.h>

#define MAX_TASKS 64

/* in order to keep the count of the different tasks that are running, we will
 * store them in a queue */
task_t *tasks;
static int task_count = 0;

/* this function is defined in __task.asm */
extern void pop_regs(regs_t *regs);

/* as you can read in task.h, we need to change tasks quickly, so it looks like
 * there are several tasks running at the same time
 * 
 * in order to achieve that, we will use the PIT (timer.c), each tick, it will
 * call our schedule function, that will take care of changing tasks */
void schedule(regs_t *regs)
{
    if (task_count == 0) /* if the scheduler has not been initialized */
        return;

    /* each time our schedule function gets called, it will search for the last
     * running task in the queue, the queue has the followning structure (where
     * S is stopped, R is running, and X is no init) 
     *
     *                 | S | S | X | S | R | X | S | S | X | X |
     *                  (3)             (1)     (2)
     * 
     * we will iterate over this, find the running task (1) and set it as stopped,
     * then, we will change to the next stopped task (2) (no init ones will be ignored)
     * 
     * in case we get to the last initialized task in the queue, we will jump again to
     * the first initialized one (3)
     */

    int next = 0;
    if (task_count > 1)
    {
        for (size_t i = 0; i < MAX_TASKS; i++)
        {
            if (tasks[i].status == TASK_NO_INIT && i == MAX_TASKS - 1) /* finished the iteration without finding anything */
                panic("No available tasks to switch to [i]\n");        /* panic because this should not happen            */
            else if (tasks[i].status == TASK_NO_INIT)
                continue;
            else if (tasks[i].status == TASK_RUNNING)
            {
                tasks[i].status = TASK_STOPPED;                        /* set current task as stopped                      */
                kmemcpy((void *)&tasks[i].regs, regs, sizeof(regs_t)); /* save current task registers into the task struct */

                /* from that position, we are going to find the next task that we can
                 * set as running, in order change to that new one */
                for (size_t j = i + 1; j < MAX_TASKS; j++)
                {
                    if (tasks[j].status == TASK_STOPPED)
                    {
                        next = j;
                        goto end;
                    }
                    else if (j == MAX_TASKS - 1)
                    {
                        /* if there are not any stopped tasks after that one, we completed a cycle
                         * and we have to start again */
                        for (size_t k = 0; k < i + 1; k++)
                        {
                            if (tasks[k].status == TASK_STOPPED)
                            {
                                next = k;
                                goto end;
                            }
                            else if (k == i)
                                /* if that didn't work, it means that the scheduler did something wrong, or something
                                 * external broke our queue */
                                panic("No available tasks to switch to [k]\n");
                        }
                    }
                }
            }
        }
    }
end:
    if (tasks[next].status != TASK_RUNNING)
    {
        // kprintf("[SCHED] changing to [number = %u, status = %u, rip = 0x%lx, page_map = 0x%lx]\n",
        //        next, tasks[next].status, tasks[next].regs.rip, tasks[next].page_map);
        tasks[next].status = TASK_RUNNING;

        /* remember that the schedule function was called because an IRQ0 was issued, then, we need to
         * mark the interrupt as correctly handled */
        port_byte_out(PIC1, PIC_EOI);

        /* now that we marked the task as running, we need to actually make the switch, before that,
         * we need to load the task's pagemap into CR3 */
        virt_load_page_map((uint64_t)tasks[next].page_map);
        pop_regs(&tasks[next].regs); /* restore task registers and make the switch */
    }
}

void load_task(task_t *task)
{
    for (size_t i = 0; i < MAX_TASKS; i++)
    {
        if (tasks[i].status == TASK_NO_INIT) /* if we find an empty task */
        {
            kmemcpy(&tasks[i], task, sizeof(task_t)); /* load it into the queue */
            tasks[i].status = TASK_STOPPED;
            task_count++;
            kprintf("[SCHD] task[%u] loaded into the queue\n", task->pid);
            break;
        }
        else if (i == MAX_TASKS - 1)
            panic("Cannot load a new task\n");
    }
}

void init_sched()
{
    tasks = (task_t *)kmalloc(sizeof(task_t) * MAX_TASKS);
    for (size_t i = 0; i < MAX_TASKS; i++) /* set all tasks as unitialized */
        tasks[i].status = TASK_NO_INIT;
    kprintf("[SCHD] tasks queue initialized\n");

    init_timer(50);
    kprintf("[SCHD] PIT timer initialized, scheduler initialized\n");
}