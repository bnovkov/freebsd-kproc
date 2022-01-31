#include <sys/types.h>
#include <sys/proc.h>
#include <sys/sysproto.h>
#include <sys/malloc.h>
#include <vm/vm.h>
#include <vm/vm_page.h>
#include <vm/vm_map.h>
#include <vm/pmap.h>

#define _KAS
#include <sys/kas.h>

int sys_dummy2(struct thread *td, struct dummy2_args *uap);

int sys_dummy2(struct thread *td, struct dummy2_args *uap){
  td->td_retval[0] = 43;

  return (0);
}
