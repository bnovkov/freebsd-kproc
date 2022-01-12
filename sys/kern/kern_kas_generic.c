
#define _KAS
#include <sys/kas.h>

#include <sys/types.h>
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/cdefs.h>
#include <vm/vm.h>
#include <vm/pmap.h>
#include <vm/vm_map.h>
#include <vm/vm_kern.h>

int __weak_symbol kas_protect(vm_offset_t start, vm_offset_t end, vm_prot_t prot, int flags)
{
  return vm_map_protect(kernel_map, start, end, prot, prot, flags);
}

int __weak_symbol __kas_md_activate_component(int component_desc){
  return 0;
}
int __weak_symbol __kas_md_deactivate_component(int component_desc){
  return 0;
}

void __weak_symbol __kas_md_enter(void){
  return;
}


