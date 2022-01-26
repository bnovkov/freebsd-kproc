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
    ((pt_entry_t *)cpu_ptpg)[i] &= ~X86_PG_U;
  }

  return 0;
}

static int __kas_deactivate_component(struct kas_component *c){
  vm_offset_t cpu_ptpg = PCPU_GET(kas_ptpg);


  for(vm_pindex_t i=c->md.start_pte_idx; i<c->md.end_pte_idx; i++){
    ((pt_entry_t *)cpu_ptpg)[i] |= X86_PG_U;
  }

  return 0;
}

static vm_offset_t __kas_load_stack(void){
  vm_offset_t caller_stack = 0;
  //  vm_offset_t pcpu_stack = PCPU_GET(kas_stack);
 
  // TODO: swap stack
  return caller_stack;
}

static void __kas_restore_stack(vm_offset_t caller_stack){
  // vm_offset_t pcpu_stack = PCPU_GET(kas_stack);

  // TODO: restore_stack(caller_stack);



  
  return;
}

/*
 * Performs a "privilege switch" to the KAS kernel.
 * Loads the private PCPU stack and
 * allows read access on KAS data pages.
 */
static void __kas_md_enter(void){
  vm_offset_t old_stack = __kas_load_stack();
  vm_offset_t cpu_ptpg = PCPU_GET(kas_ptpg);

  /* Allow rw access to curcpu kas ptpg */
  //  pmap_protect(kernel_pmap, cpu_ptpg, cpu_ptpg + PAGE_SIZE, VM_PROT_RW);
  
  /* Map data pages */
  for(vm_pindex_t i=priv_data.md_data.kas_data_start_pte_idx; i<priv_data.md_data.kas_data_end_pte_idx; i++){
    ((pt_entry_t *)cpu_ptpg)[i] &= ~X86_PG_U;
  }
  priv_data.caller_stack[curcpu] = old_stack;
}

/*
 * Called upon leaving the KAS kernel.
 * Performs the opposite of __kas_md_enter.
 */
static void __kas_md_leave(void){
  vm_offset_t caller_stack = priv_data.caller_stack[curcpu];
  vm_offset_t cpu_ptpg = PCPU_GET(kas_ptpg);

  /* Unmap data pages */
  for(vm_pindex_t i=priv_data.md_data.kas_data_start_pte_idx; i<priv_data.md_data.kas_data_end_pte_idx; i++){
    ((pt_entry_t *)cpu_ptpg)[i] |= X86_PG_U;
  }
  /* Unmap curcpu kas ptpg */
  // pmap_protect(kernel_pmap, cpu_ptpg, cpu_ptpg + PAGE_SIZE, VM_PROT_READ);

  __kas_restore_stack(caller_stack);
}

/*
 * KAS kernel entrypoints.
 */

void __kas_activate_syscall(int syscall_num){
  // TODO: kassert

  register_t reg =  intr_disable();
  __kas_md_enter();

  struct kas_component *c = kas__syscall_components[syscall_num];
  if(c == NULL){
    goto exit;
  }

  __kas_activate_component(c);

 exit:
  __kas_md_leave();
  intr_restore(reg);

}

void __kas_deactivate_syscall(int syscall_num){
  // TODO: kassert

  __kas_md_enter();

  register_t reg =  intr_disable();

  struct kas_component *c = kas__syscall_components[syscall_num];
  if(c == NULL){
    goto exit;
  }

  __kas_deactivate_component(c);

 exit:
  intr_restore(reg);

  __kas_md_leave();

}
