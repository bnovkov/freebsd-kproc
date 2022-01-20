#pragma once

#include <sys/types.h>
#include <sys/param.h>

struct kas_md;

void kas_smp_md_init(void* data);

struct kas_md {
  vm_pindex_t kas_data_start_pte_idx;
  vm_pindex_t kas_data_end_pte_idx;
};

struct kas_component_md {
  vm_pindex_t start_pte_idx;
  vm_pindex_t end_pte_idx;
};
