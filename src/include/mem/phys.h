#pragma once

#include <stivale2.h>

#define PAGE_SIZE 4096
#define CHECK_PAGE(page) (phys_bitmap[page / 8] & (1 << (page % 8)))

extern uint8_t *phys_bitmap;

void init_phys(struct stivale2_struct_tag_memmap *mem_tag);