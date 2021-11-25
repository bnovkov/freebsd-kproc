#include <sys/types.h>


#define _KAS
#include <sys/kas.h>



int kas_init(void){
  // TODO: kreiranje zasebnog submapa
  // TODO: runtime alokacije stackova za svaki thread


  return 0;
}


void __kas_activate_priv_ctx(void){
  // invoke the underlying mechanism to allow RWX on kas pages
  // load kas private stack
  return;
}

void __kas_deactivate_priv_ctx(void){
  // restore previous user stack kas private stack
  // invoke the underlying mechanism to revoke RWX on kas pages
  return;
}
