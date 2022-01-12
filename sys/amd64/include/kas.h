#pragma once

#include <sys/types.h>
#include <sys/param.h>

struct kas_md;

void kas_smp_md_init(void* data);

struct kas_md {
  /* We limit ourselves to one PTPG for all systems for the POC */
  vm_offset_t pcpu_kas_ptpg[MAXCPU];
};

struct kas_component_md {
  vm_pindex_t start_pte_idx;
  vm_pindex_t end_pte_idx;
};
