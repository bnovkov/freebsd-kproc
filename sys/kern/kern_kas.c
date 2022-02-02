#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include <machine/cpufunc.h>
#include <sys/pcpu.h>
#include <sys/cdefs.h>
#include <sys/smp.h>
#include <sys/pcpu.h>

#include <vm/vm.h>
#include <vm/vm_page.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_kern.h>
#include <machine/vmparam.h>


#define _KAS
#include <sys/kas.h>
#include <machine/kas.h>
#include <machine/cpufunc.h>

extern int kas__no_components;
extern struct kas_component kas__components[];
extern struct kas_component *kas__syscall_components[];

struct kas_priv_data priv_data;


/*
 * KAS kernel routines.
 * Only accessible through __kas_enter.
 * These are executed on a preallocated, private per-cpu stack which is loaded by __kas_enter.
 */
static int __kas_activate_component(struct kas_component *c){
  vm_offset_t cpu_ptpg = PCPU_GET(kas_ptpg);


  for(vm_pindex_t i=c->md.start_pte_idx; i<c->md.end_pte_idx; i++){
    ((pt_entry_t *)cpu_ptpg)[i] |= X86_PG_V;
    invlpg(c->layout.base + (i - c->md.start_pte_idx) * PAGE_SIZE);
  }

  return 0;
}

static int __kas_deactivate_component(struct kas_component *c){
  vm_offset_t cpu_ptpg = PCPU_GET(kas_ptpg);


  for(vm_pindex_t i=c->md.start_pte_idx; i<c->md.end_pte_idx; i++){
    ((pt_entry_t *)cpu_ptpg)[i] &= ~X86_PG_V;
    invlpg(c->layout.base + (i - c->md.start_pte_idx) * PAGE_SIZE);
  }

  return 0;
}


/*
 * Performs a "privilege switch" to the KAS kernel.
 * Loads the private PCPU stack and
 * allows read access on KAS data pages.
 *
 * Returns ptr to curcpu thread context saving block.
 */

vm_offset_t __kas_md_enter(void){

  /* Allow rw access to curcpu kas ptpg */
  //  pmap_protect(kernel_pmap, cpu_ptpg, cpu_ptpg + PAGE_SIZE, VM_PROT_RW);

  /* Map data pages */
  //for(vm_pindex_t i=priv_data.md_data.kas_data_start_pte_idx; i<priv_data.md_data.kas_data_end_pte_idx; i++){
  //  ((pt_entry_t *)cpu_ptpg)[i] &= ~X86_PG_U;
  //}

  return (vm_offset_t)&priv_data.md_data.td_ctx[curcpu];
}

/*
 * Called upon leaving the KAS kernel.
 * Performs the opposite of __kas_md_enter.
 */
void __kas_md_leave(void){
  // TODO: map ctx saving bss as RDONLY when leaving the kernel so that the context may be restored
  //vm_offset_t cpu_ptpg = PCPU_GET(kas_ptpg);

  /* Unmap data pages */
  //for(vm_pindex_t i=priv_data.md_data.kas_data_start_pte_idx; i<priv_data.md_data.kas_data_end_pte_idx; i++){
  //  ((pt_entry_t *)cpu_ptpg)[i] |= X86_PG_U;
  //}
  /* Unmap curcpu kas ptpg */
  // pmap_protect(kernel_pmap, cpu_ptpg, cpu_ptpg + PAGE_SIZE, VM_PROT_READ);
}

void __kas_activate_pt(void);
void __kas_deactivate_pt(void);

static void __activate_syscall(int syscall_num){

  struct kas_component *c = kas__syscall_components[syscall_num];
  if(c == NULL){
    return;
  }
  if(curcpu != 0){
    __kas_activate_pt();
  }
  __kas_activate_component(c);
}

static void __deactivate_syscall(int syscall_num){

  struct kas_component *c = kas__syscall_components[syscall_num];
  if(c == NULL){
    return;
  }

  __kas_deactivate_component(c);
  if(curcpu != 0){
    __kas_deactivate_pt();
  }

}

/*
 * KAS kernel entrypoint.
 */

void kas_trampoline(int opcode, uint64_t arg1){
  switch(opcode){
  case KAS_ACTIVATE_SYSCALL:
    __activate_syscall(arg1);
    break;
  case KAS_DEACTIVATE_SYSCALL:
    __deactivate_syscall(arg1);
    break;
  default:
    break;
  }
}
