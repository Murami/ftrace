#include <sys/ptrace.h>
#include <sys/user.h>
#include "ftrace.h"

static inline unsigned long	call_rm_0xD_with_rexb(int pid, unsigned long instruction,
						      struct user_regs_struct* registers, t_rex* rex,
						      unsigned char rm)
{
  _(instruction);
  _(rex);
  _(pid);
  switch (rm)
    {
    case 0xD0:
      return (registers->r8);
    case 0xD1:
      return (registers->r9);
    case 0xD2:
      return (registers->r10);
    case 0xD3:
      return (registers->r11);
    case 0xD4:
      return (registers->r12);
    case 0xD5:
      return (registers->r13);
    case 0xD6:
      return (registers->r14);
    case 0xD7:
      return (registers->r15);
    default:
      return (0);
    }
}

static inline unsigned long	call_rm_0xD_without_rexb(int pid, unsigned long instruction,
							 struct user_regs_struct* registers, t_rex* rex,
							 unsigned char rm)
{
  _(pid);
  _(instruction);
  _(rex);
  switch (rm)
  {
  case  0xD0:
    return (registers->rax);
  case  0xD1:
    return (registers->rcx);
  case  0xD2:
    return (registers->rdx);
  case  0xD3:
    return (registers->rbx);
  case  0xD4:
    return (registers->rsp);
  case  0xD5:
    return (registers->rbp);
  case  0xD6:
    return (registers->rsi);
  case  0xD7:
    return (registers->rdi);
  default:
    return (0);
  }
}

unsigned long	call_rm_0xD(int pid, unsigned long instruction,
			    struct user_regs_struct* registers, t_rex* rex,
			    unsigned char rm)
{
  if (rex->b)
    return (call_rm_0xD_with_rexb(pid, instruction, registers, rex, rm));
  else
    return (call_rm_0xD_without_rexb(pid, instruction, registers, rex, rm));
}
