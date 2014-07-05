#include <sys/ptrace.h>
#include <sys/user.h>
#include "ftrace.h"

static inline unsigned long	call_rm_0x1_with_rexb(int pid, unsigned long instruction,
						      struct user_regs_struct* registers, t_rex* rex,
						      unsigned char rm)
{
  _(instruction);
  _(rex);
  switch (rm)
    {
    case 0x10:
      return (get_data(pid, registers->rax));
    case 0x11:
      return (get_data(pid, registers->rcx));
    case 0x12:
      return (get_data(pid, registers->rdx));
    case 0x13:
      return (get_data(pid, registers->rbx));
    case 0x16:
      return (get_data(pid, registers->rsi));
    case 0x17:
      return (get_data(pid, registers->rdi));
    default:
      return (0);
    }
}

static inline	unsigned long	call_rm_0x1_without_rexb(int pid, unsigned long instruction,
							 struct user_regs_struct* registers, t_rex* rex,
							 unsigned char rm)
{
  _(instruction);
  _(rex);
  switch (rm)
    {
    case 0x10:
      return (get_data(pid, registers->r8));
    case 0x11:
      return (get_data(pid, registers->r9));
    case 0x12:
      return (get_data(pid, registers->r10));
    case 0x13:
      return (get_data(pid, registers->r11));
    case 0x16:
      return (get_data(pid, registers->r14));
    case 0x17:
      return (get_data(pid, registers->r15));
    default:
      return (0);
    }
}

unsigned long	call_rm_0x1(int pid, unsigned long instruction,
			    struct user_regs_struct* registers, t_rex* rex,
			    unsigned char rm, int* offset_rip)
{
  unsigned long		addb;

  if (rm == 0x14)
    {
      *offset_rip = 3;
      if ((((instruction & 0xFF0000) >> 16) & 0x07) == 5)
	*offset_rip = 7;
      return (get_data(pid, get_sib_addr(pid, (instruction & 0xFF0000) >> 16, registers, rex, 0)));
    }
  else if (rm == 0x15)
    {
      addb = get_data(pid, registers->rip + 2) & 0xFFFFFFFF;
      *offset_rip = 6;
      return (get_data(pid, registers->rip + 6 + addb));
    }
  else if (rex->b)
    return (call_rm_0x1_with_rexb(pid, instruction, registers, rex, rm));
  else
    return (call_rm_0x1_without_rexb(pid, instruction, registers, rex, rm));
}
