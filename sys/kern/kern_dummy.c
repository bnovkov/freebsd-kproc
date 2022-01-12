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

int sys_dummy(struct thread *td, struct dummy_args *uap);

int sys_dummy(struct thread *td, struct dummy_args *uap){
  td->td_retval[0] = 42;

  int *a = malloc(sizeof(int) * 5, M_TEMP, M_WAITOK);

  free(a, M_TEMP);

  __kas_kirc_call(0);

  extern struct kas_priv_data priv_data;

  priv_data.md_data.pcpu_kas_ptpg[0] = 0;

  return (0);
}
