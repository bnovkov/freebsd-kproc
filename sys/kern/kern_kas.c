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

extern int kas__no_components;
extern struct kas_component kas__components[];

/*
 * KAS kernel routines.
 * Only accessible through __kas_enter.
 * These are executed on a preallocated, private per-thread stack which is loaded by __kas_enter.
 */
int __kas_activate_component(int component_desc){
  KASSERT(component_desc < kas__no_components, ("kas_activate_component: invalid component descriptor"));
  return 0;
}

int __kas_deactivate_component(int component_desc){
  KASSERT(component_desc < kas__no_components, ("kas_activate_component: invalid component descriptor"));
  return 0;
}



/*
 * KAS kernel entrypoints.
 */
void __kas_enter(void){
  //  intr_disable();

  __kas_md_enter();
  // invoke the underlying mechanism to allow RWX on kas pages
  // load kas private stack
  return;
}

void __kas_leave(void){
  // restore previous user stack
  // invoke the underlying mechanism to revoke RWX on kas pages

  __kas_md_leave();

  //  intr_enable();
  return;
}


