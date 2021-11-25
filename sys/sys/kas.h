#pragma once

#if defined(_KERNEL)

int kas_init(void);
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

struct kas_region_info {
  vm_offset_t region_start;
  vm_offset_t region_end;
};

struct kas_component {
  int idx;
  enum ComponentType type;
  const char[32] name;

  struct kas_region_info info;
};


/*
 * Helper functions used to grant access to kas data.
 */
void __kas_activate_priv_ctx(void);
void __kas_deactivate_priv_ctx(void);

#endif /* _KAS */
#endif /* _KERNEL */
