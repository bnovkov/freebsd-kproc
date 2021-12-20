#pragma once

#include <sys/types.h>
#include <vm/vm.h>

#if defined(_KERNEL)

void __kas_generated_init(void *data);
int __kas_kirc_call(void); /* Used for indirect, function pointer calls */
int __kas_kirc_call_nolookup(void); /* Used for known symbol calls */



#if defined(_KAS)


#define  KAS_PROT_NONE  0
#define  KAS_PROT_READ  0x1
#define  KAS_PROT_WRITE 0x2
#define  KAS_PROT_EXEC  0x4
#define  KAS_PROT_RW    (KAS_PROT_READ | KAS_PROT_WRITE)



/*
 * BST node
 */
struct kas_region_node {
  struct kas_region_node *left;
  struct kas_region_node *right;

  u_int prot;
};

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
  enum ComponentType type;
  const char name[32];

  struct kas_component_layout layout;
};



int kas_protect(vm_offset_t start, vm_offset_t end, vm_prot_t prot, int flags);

/*
 * Protection mechanism interface definition
 */
int __kas_activate_component(int component_desc);
int __kas_deactivate_component(int component_desc);
/* Helper functions used for "privilege switching", i.e. entering/leaving the kas kernel. */
void __kas_enter(void);
void __kas_leave(void);

/*
 * Machine-dependent protection mechanism interface
 */
int  __kas_md_activate_component(int component_desc);
int  __kas_md_deactivate_component(int component_desc);
void __kas_md_enter(void);
void __kas_md_leave(void);


#endif /* _KAS */
#endif /* _KERNEL */
