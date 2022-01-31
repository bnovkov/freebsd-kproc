#include "assym.inc"
#include <machine/asmacros.h>
#include <machine/specialreg.h>

/*
 * kas_enter(opcode, arg1)
 *
 * Saves context of current thread, switches to the private stack,
 * invokes the nested kernel and returns.
 *
 * %rdi = nested kernel opcode
 * %rsi = nested kernel call arg
 */

ENTRY(__kas_syscall)
  pushq %rbp
  movq %rsp, %rbp

  pushq %rax

  pushq %rdi
  pushq %rsi

  cli
  call __kas_md_enter

  popq %rsi
  popq %rdi

  // save ctx to kas private data
	movq	%rdx,(%rax)
	movq	%rcx,8(%rax)
	movq	%rsp,16(%rax)
	movq	%rbp,24(%rax)
	movq	%rbx,32(%rax)
  movq  %r8,40(%rax)
	movq	%r9,48(%rax)
	movq	%r10,56(%rax)
	movq	%r11,64(%rax)
	movq	%r12,72(%rax)
	movq	%r13,80(%rax)
	movq	%r14,88(%rax)
	movq	%r15,96(%rax)

  // swap stack
  movq PCPU(KAS_STACK), %rsp
  movq %rsp, %rbp

  // save ptr to ctx save area
  pushq %rax

  call kas_trampoline

  // fetch ptr to curcpu area 
  popq %rax

  // restore current ctx
 	movq (%rax),%rdx
	movq 8(%rax),%rcx
	movq 16(%rax),%rsp
	movq 24(%rax),%rbp
	movq 32(%rax),%rbx
	movq 40(%rax),%r8
	movq 48(%rax),%r9
	movq 56(%rax),%r10
	movq 64(%rax),%r11
	movq 72(%rax),%r12
	movq 80(%rax),%r13
	movq 88(%rax),%r14
	movq 96(%rax),%r15

  call __kas_md_leave
  sti

  popq %rax

  movq %rbp, %rsp
  popq %rbp
  ret
END(__kas_syscall)

