#pragma once

#include <task/task.h>

void load_task(task_t *task);
void schedule(regs_t *regs);
void init_sched();