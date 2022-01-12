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


struct kas_priv_data priv_data;


/*
 * KAS kernel routines.
 * Only accessible through __kas_enter.
 * These are executed on a preallocated, private per-cpu stack which is loaded by __kas_enter.
 */
int __kas_md_activate_component(int component_desc){
  KASSERT(component_desc < kas__no_components, ("kas_activate_component: invalid component descriptor"));
  struct kas_component *c = &kas__components[component_desc];
  vm_offset_t cpu_ptpg = priv_data.md_data.pcpu_kas_ptpg[curcpu];


  for(vm_pindex_t i=c->md.start_pte_idx; i<c->md.end_pte_idx; i++){
    ((pt_entry_t *)cpu_ptpg)[i] &= ~X86_PG_U;
  }

  return 0;
}

int __kas_md_deactivate_component(int component_desc){
  KASSERT(component_desc < kas__no_components, ("kas_activate_component: invalid component descriptor"));
  struct kas_component *c = &kas__components[component_desc];
  vm_offset_t cpu_ptpg = priv_data.md_data.pcpu_kas_ptpg[curcpu];


  for(vm_pindex_t i=c->md.start_pte_idx; i<c->md.end_pte_idx; i++){
    ((pt_entry_t *)cpu_ptpg)[i] |= X86_PG_U;
  }

  return 0;
}



/*
 * KAS kernel entrypoints.
 */
void __kas_enter(void){
  register_t reg =  intr_disable();

  __kas_md_enter();
  // invoke the underlying mechanism to allow RWX on kas pages
  // load kas private stack

  intr_restore(reg);
  return;
}
