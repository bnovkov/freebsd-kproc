#pragma once

#include <sys/types.h>
#include <vm/vm.h>

#if defined(_KERNEL)

/*
 * Nested kernel opcodes.
 */
#define KAS_ACTIVATE_SYSCALL 0
#define KAS_DEACTIVATE_SYSCALL 1

void __kas_generated_init(void *data);
/*
 * KAS kernel entrypoints.
 */
void __kas_syscall(register_t opcode, register_t arg1);

#define __kas_activate_syscall(syscall_num) __kas_syscall(KAS_ACTIVATE_SYSCALL, (syscall_num))
#define __kas_deactivate_syscall(syscall_num) __kas_syscall(KAS_DEACTIVATE_SYSCALL, (syscall_num))



#if defined(_KAS)

#include <machine/kas.h>

#define  KAS_PROT_NONE  0
#define  KAS_PROT_READ  0x1
#define  KAS_PROT_WRITE 0x2
#define  KAS_PROT_EXEC  0x4
#define  KAS_PROT_RW    (KAS_PROT_READ | KAS_PROT_WRITE)

void kas_trampoline(int opcode, uint64_t arg1);
vm_offset_t __kas_md_enter(void);
void __kas_md_leave(void);


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

#endif /* _KAS */
#endif /* _KERNEL */
