#pragma once

#include <stdint.h>
#include <cpu/regs.h>

#define TASK_STOPPED 1
#define TASK_RUNNING 2
#define TASK_NO_INIT 3

#define FLAG_USER 0b01
#define FLAG_KERN 0b10

#define STACK_SIZE 0x10000

/* a task is a process, for now, we are not going to differentiate
 * between task and thread
 * 
 * each task ought to have its own pid (process id), its own registers
 * (each time we change tasks, we need to load their new registers) and
 * its own pagemap in order to have its own virtual space
 * 
 * so as to get several tasks running (ostensibly), what we are going to do is give
 * them a CPU time, that is, one task will run for some milliseconds, and
 * then we will change to the next one */
typedef struct task
{
    int pid;
    int status;

    regs_t regs;
    uint64_t *page_map;
    uint8_t flags;
} task_t;

void create_task(uint64_t ip, uint8_t flags);