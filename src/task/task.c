#include <task/task.h>
#include <task/sched.h>
#include <kernel/panic.h>
#include <mem/kheap.h>
#include <mem/phys.h>
#include <mem/virt.h>

int pid_count = 0;

void create_task(uint64_t ip, uint8_t flags)
{
    task_t *task = (task_t *)kmalloc(sizeof(task_t));
    task->flags = flags;

    task->pid = pid_count++;
    task->regs.rip = ip;                     /* the ip is the instruction pointer, and will be loaded into the rip register */
    task->regs.rflags = (1 << 9) | (1 << 1); /* rflags reg contains the current CPU state, interrupt enable flag | reserved */

    if (task->flags & FLAG_USER) /* this task is for a user program */
    {
        task->regs.cs = 0x23; /* gdt_entry user_code  */
        task->regs.ss = 0x1B; /* gdt_entry user_data  */

        task->page_map = (uint64_t *)kmalloc(PAGE_SIZE);

        /* the pagemap has 4096/8 = 512 entries (each one 64 bits), so, if the task is for
         * a user program, we will copy the higher half kernel entries into the task pagemap */
        for (size_t i = 256; i < 512; i++)
            task->page_map[i] = kernel_page_map[i];

        panic("User tasks are not implemented\n");
    }
    else /* this task is for the kernel */
    {
        task->regs.cs = 0x08; /* [8]  -> gdt_entry kernel_code */
        task->regs.ss = 0x10; /* [16] -> gdt_entry kernel_data */

        task->page_map = kernel_page_map;

        /* the rsp register points to the top of the stack (since the stack grows downwards, we add its size) */
        task->regs.rsp = (uint64_t)kmalloc(STACK_SIZE) + HIGHER_HALF + STACK_SIZE;
    }

    load_task(task); /* automatically enqueue the new task */
}