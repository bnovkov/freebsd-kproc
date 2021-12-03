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
