#include <sys/types.h>
#include <sys/proc.h>
#include <sys/sysproto.h>

int sys_dummy(struct thread *td, struct dummy_args *uap);

int sys_dummy(struct thread *td, struct dummy_args *uap){
  td->td_retval[0] = 42;

  return (0);
}
