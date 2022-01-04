#define _KAS

#include <sys/kas.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/cdefs.h>
#include <vm/vm.h>
#include <vm/pmap.h>
#include <vm/vm_map.h>
#include <vm/vm_page.h>
#include <vm/vm_kern.h>

#include <sys/smp.h>
#include <machine/kas.h>

//#include "__kas_info.c"
// Check if curthread can exec target subsystem
static int __kas_check_td_exec_right(void){
  return 1;
}


int __kas_kirc_call(int component_desc){
  __kas_enter();
  __kas_activate_component(component_desc);
  __kas_leave();

  // func(args)

  __kas_enter();
  __kas_deactivate_component(component_desc);
  __kas_leave();
  return 0;
}

/*
int __kas_kirc_call_nolookup(void){
  __kas_activate_priv_ctx();

  // quickly check if the thread is allowed to exec target subsystem
  // TODO: stavit ovo kao kassert
  if(__kas_check_td_exec_right()){
    // panic or return error
    // TODO: make this behaviour a configurable policy
    return -1;
  }

  // check if target function is part of target subsystem
  // TODO: stavit ovo kao kassert
  if(__kas_check_func_in_subsys()){
    // panic or return error
    // TODO: make this behaviour a configurable policy
    return -1;
  }

  // activate target ctx
  // switch to old stack
  // Call the target function
  // TODO: __kas_deactivate_priv_ctx();?
  // func_call()
  // TODO: __kas_activate_priv_ctx();?
  // switch to priv_stack
  // deactivate target ctx
  __kas_deactivate_priv_ctx();

  return 0;
}

*/


/*
 * Maps all subsystems to avoid page faults during kernel startup.
 */
static int kas_init(void){
  // TODO: kreiranje zasebnog submapa
  // TODO: alokacije PCPU stackova

  extern int __kas_vmkern_text_start;
  extern int __kas_vmkern_text_end;
  extern int __kas_vmkern_data_start;
  extern int __kas_vmkern_data_end;
  extern int __kas_vmkern_rodata_start;
  extern int __kas_vmkern_rodata_end;
  extern int __kas_vmkern_bss_start;
  extern int __kas_vmkern_bss_end;

  kas_protect((vm_offset_t)&__kas_vmkern_text_start, (vm_offset_t)&__kas_vmkern_text_end,
              VM_PROT_EXECUTE | VM_PROT_READ, 0);
	kas_protect((vm_offset_t)&__kas_vmkern_data_start, (vm_offset_t)&__kas_vmkern_data_end,
              VM_PROT_READ | VM_PROT_WRITE, 0);
	kas_protect((vm_offset_t)&__kas_vmkern_rodata_start, (vm_offset_t)&__kas_vmkern_rodata_end, VM_PROT_READ, 0);
	kas_protect((vm_offset_t)&__kas_vmkern_bss_start, (vm_offset_t)&__kas_vmkern_bss_end,
              VM_PROT_READ | VM_PROT_WRITE, 0);

  __kas_generated_init(NULL);

  return 0;
}


// TODO: maknuti ovo u md dio
static void kas_smp_init(void){
  kas_smp_md_init();
  return;
}


SYSINIT(kas_boot, SI_SUB_KAS_BOOT, SI_ORDER_FIRST, kas_init, NULL);
SYSINIT(kas_smp, SI_SUB_KAS_SMP, SI_ORDER_FIRST, kas_smp_init, NULL);
