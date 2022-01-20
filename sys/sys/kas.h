#pragma once

#include <sys/types.h>
#include <vm/vm.h>

#if defined(_KERNEL)

void __kas_generated_init(void *data);
int __kas_kirc_call(int); /* Used for indirect, function pointer calls */
int __kas_kirc_call_nolookup(void); /* Used for known symbol calls */



#if defined(_KAS)

#include <machine/kas.h>

#define  KAS_PROT_NONE  0
#define  KAS_PROT_READ  0x1
#define  KAS_PROT_WRITE 0x2
#define  KAS_PROT_EXEC  0x4
#define  KAS_PROT_RW    (KAS_PROT_READ | KAS_PROT_WRITE)

/*
 * Main kernel address space descriptor.
 */
enum ComponentType {
  SUBSYS,
  MODULE,
  SYSCALL
};

struct kas_component_layout {
  vm_offset_t base;
  vm_offset_t text_end;
  vm_offset_t data_end;
  vm_offset_t rodata_end;
  vm_offset_t bss_end;
};

struct kas_component {
  int idx;
  const char name[32];

  enum ComponentType type;

  union  {
    int syscall_num;
  } type_data;

  struct kas_component_layout layout;
  struct kas_component_md md;
};


struct kas_priv_data {
  vm_offset_t caller_stack[MAXCPU];

  struct kas_md md_data;
};

int kas_protect(vm_offset_t start, vm_offset_t end, vm_prot_t prot, int flags);

/*
 * Machine-dependent protection mechanism interface
 */
void  __kas_activate_syscall(int syscall_num);


#endif /* _KAS */
#endif /* _KERNEL */
