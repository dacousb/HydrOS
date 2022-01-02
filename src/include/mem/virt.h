#pragma once

#include <stdint.h>

#define HIGHER_HALF 0xffff800000000000
#define KERNEL_OFFS 0xffffffff80000000

extern uint64_t *kernel_page_map;

void virt_load_page_map(uint64_t pml4);
void virt_map_multi(uint64_t *page_map, uint64_t base, uint64_t end, uint64_t offset, uint64_t flags);
void init_virt();