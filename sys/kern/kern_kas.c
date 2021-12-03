#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>

#include <cpufunc.h>
#include <sys/pcpu.h>
#include <sys/cdefs.h>


#define _KAS
#include <sys/kas.h>

extern int kas__no_components;
extern struct kas_component kas__components[];


/*
 * KAS kernel routines.
 * Only accessible through __kas_enter.
 * These are executed on a preallocated, private per-thread stack which is loaded by __kas_enter.
 */
void __kas_activate_component(int component_desc){
  KASSERT(component_desc < kas__no_components, ("kas_activate_component: invalid component descriptor"));
  return;
}

void __kas_deactivate_component(int component_desc){
  KASSERT(component_desc < kas__no_components, ("kas_activate_component: invalid component descriptor"));
  return;
}



/*
 * KAS kernel entrypoints.
 */
void __kas_enter(void){
  intr_disable();

  __kas_md_enter();
  // invoke the underlying mechanism to allow RWX on kas pages
  // load kas private stack
  return;
}

void __kas_leave(void){
  // restore previous user stack
  // invoke the underlying mechanism to revoke RWX on kas pages

  __kas_md_leave();

  intr_enable();
  return;
}




int kas_init(void *data){
  // TODO: kreiranje zasebnog submapa
  // TODO: runtime alokacije stackova za svaki thread

  extern int __kas_vmkern_text_start;
  extern int __kas_vmkern_text_end;
  extern int __kas_vmkern_data_start;
  extern int __kas_vmkern_data_end;
  extern int __kas_vmkern_rodata_start;
  extern int __kas_vmkern_rodata_end;
  extern int __kas_vmkern_bss_start;
  extern int __kas_vmkern_bss_end;

  kas_protect(__kas_vmkern_text_start, __kas_vmkern_text_end,
              VM_PROT_EXECUTE | VM_PROT_READ, 0);
	kas_protect(__kas_vmkern_data_start, __kas_vmkern_data_end,
              VM_PROT_READ | VM_PROT_WRITE, 0);
	kas_protect(__kas_vmkern_rodata_start, __kas_vmkern_rodata_end, VM_PROT_READ, 0);
	kas_protect(__kas_vmkern_bss_start, __kas_vmkern_bss_end,
              VM_PROT_READ | VM_PROT_WRITE, 0);

  __kas_generated_init(NULL);

  return 0;
}


SYSINIT(kas, SI_SUB_KAS, SI_ORDER_FIRST, kas_init, NULL);
