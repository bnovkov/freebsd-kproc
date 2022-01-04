#include <sys/types.h>
#include <sys/proc.h>
#include <sys/sysproto.h>
#include <sys/malloc.h>

int sys_dummy(struct thread *td, struct dummy_args *uap);

int sys_dummy(struct thread *td, struct dummy_args *uap){
  td->td_retval[0] = 42;

  int *a = malloc(sizeof(int) * 5, M_TEMP, M_WAITOK);

  free(a, M_TEMP);

  return (0);
}
