#define _KAS

#include <sys/kas.h>

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


// TODO: maknuti ovo u md dio
static void kas_smp_init(void){
  vm_page_t  pcpu_kcr3_arr[32];

  vm_offset_t cur_kcr3_vaddr = DMAP_MIN_ADDRESS + kernel_pmap->pm_cr3;

  /* Allocate per-cpu top-level pagetables */
  /* Leave first CPU intact */
  for(int i=1; i<mp_ncpus-1; i++){
    vm_page_t pcpu_kcr3 = vm_page_alloc_noobj(VM_ALLOC_WIRED);
    KASSERT(pcpu_kcr3, ("kas_init: unable to allocate ptpg"));
    vm_offset_t pcpu_kcr3_dmap_vaddr = DMAP_MIN_ADDRESS + (vm_offset_t)pcpu_kcr3->phys_addr;

    /* Insert to dmap */
    vm_map_lock(kernel_map);
    vm_map_insert(kernel_map, NULL, 0, pcpu_kcr3_dmap_vaddr, pcpu_kcr3_dmap_vaddr + PAGE_SIZE, VM_PROT_RW, VM_PROT_RW, 0);
    vm_map_unlock(kernel_map);

    /* Copy top-level page table */
    bcopy((void *)cur_kcr3_vaddr, (void *)pcpu_kcr3_dmap_vaddr, PAGE_SIZE);

    pcpu_kcr3_arr[i] = pcpu_kcr3;
  }


  /* Leave first CPU intact */
  for(int i=1; i<mp_ncpus-1; i++){
    // TODO: cpuid_to_pcpu[i]
    struct pcpu *cur_pcpu = cpuid_to_pcpu[i];
    KASSERT(cur_pcpu, ("invalid pcpu"));
    cur_pcpu->pc_kcr3 = pcpu_kcr3_arr[i]->phys_addr;
    cur_pcpu->pc_ucr3 = -1;
  }

  return;
}


SYSINIT(kas_boot, SI_SUB_KAS_BOOT, SI_ORDER_FIRST, kas_init, NULL);
SYSINIT(kas_smp, SI_SUB_KAS_SMP, SI_ORDER_FIRST, kas_smp_init, NULL);
