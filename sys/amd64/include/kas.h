#pragma once

#include <sys/types.h>
#include <sys/param.h>
#include <sys/smp.h>

struct kas_md;

void kas_smp_md_init(void* data);
void kas_smp_early_md_init(void* data);

struct kas_td_ctx {
    register_t	rdx;
    register_t	rcx;	/* (*) */
    register_t	rsp;	/* (*) */
  register_t	rbp;	/* (*) */
  register_t	rbx;	/* (*) */

  register_t	r8;	/* (*) */
  register_t	r9;	/* (*) */
  register_t	r10;	/* (*) */
  register_t	r11;	/* (*) */
    register_t	r12;	/* (*) */
    register_t	r13;	/* (*) */
    register_t	r14;	/* (*) */
    register_t	r15;	/* (*) */


};

struct kas_md {
  vm_pindex_t kas_data_start_pte_idx;
  vm_pindex_t kas_data_end_pte_idx;

  struct kas_td_ctx td_ctx[MAXCPU];
};

struct kas_component_md {
  vm_pindex_t start_pte_idx;
  vm_pindex_t end_pte_idx;
};
