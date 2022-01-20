#define _KAS

#include <sys/syscall.h>
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


int __kas_kirc_call(int component_desc){
  // __kas_deactivate_component(component_desc);
  return 0;
}


/*
 * Maps all subsystems during kernel startup.
 */

static int kas_init(void* data){
  // TODO: alokacije PCPU stackova
  extern int __kas_vmkern_data_start;
  extern int __kas_vmkern_data_end;
  extern int __kas_vmkern_text_start;
  extern int __kas_vmkern_text_end;
   extern int __kas_vmkern_rodata_start;
  extern int __kas_vmkern_rodata_end;
  //  extern int __kas_vmkern_bss_start;
  //extern int __kas_vmkern_bss_end;

  kas_protect((vm_offset_t)&__kas_vmkern_text_start, (vm_offset_t)&__kas_vmkern_text_end,
              VM_PROT_EXECUTE | VM_PROT_READ , 0);
	kas_protect((vm_offset_t)&__kas_vmkern_data_start, (vm_offset_t)&__kas_vmkern_data_end,
            VM_PROT_READ | VM_PROT_WRITE, 0);
	kas_protect((vm_offset_t)&__kas_vmkern_rodata_start, (vm_offset_t)&__kas_vmkern_rodata_end, VM_PROT_READ, 0);
	//kas_protect((vm_offset_t)&__kas_vmkern_bss_start, round_page((vm_offset_t)&__kas_vmkern_bss_end),
  //          VM_PROT_READ | VM_PROT_WRITE, 0);

  __kas_generated_init(NULL);

  extern int kas__no_components;
  extern struct kas_component kas__components[];
  extern struct kas_component *kas__syscall_components[];


  for(int i=0; i<kas__no_components; i++){
    struct kas_component *c = &kas__components[i];
    struct kas_component_layout *layout = &kas__components[i].layout;
    struct kas_component_md *md = &kas__components[i].md;

    md->start_pte_idx = pmap_pte_index(layout->base);
    md->end_pte_idx = pmap_pte_index(layout->bss_end);

    if(c->type == SYSCALL){
      KASSERT(c->type_data.syscall_num > 0 && c->type_data.syscall_num < SYS_MAXSYSCALL, ("kas_init: invalid syscall number"));
      kas__syscall_components[c->type_data.syscall_num] = c;
    }
  }

  return 0;
}
extern struct kas_priv_data priv_data;

SYSINIT(kas_boot, SI_SUB_KAS_BOOT, SI_ORDER_FIRST, kas_init, NULL);
SYSINIT(kas_smp, SI_SUB_KAS_SMP, SI_ORDER_FIRST, kas_smp_md_init, &priv_data.md_data);
