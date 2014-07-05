#include <sys/ptrace.h>
#include <sys/user.h>
#include "ftrace.h"

static inline unsigned long	call_rm_0x5_with_rexb(int pid, unsigned long instruction,
						      struct user_regs_struct* registers, t_rex* rex,
						      unsigned char rm, char addb)
{
  _(instruction);
  _(rex);
  switch (rm)
    {
    case 0x50:
      return (get_data(pid, registers->r8 + addb));
    case 0x51:
      return (get_data(pid, registers->r9 + addb));
    case 0x52:
      return (get_data(pid, registers->r10 + addb));
    case 0x53:
      return (get_data(pid, registers->r11 + addb));
    case 0x55:
      return (get_data(pid, registers->r13 + addb));
    case 0x56:
      return (get_data(pid, registers->r14 + addb));
    case 0x57:
      return (get_data(pid, registers->r15 + addb));
    default:
      return (0);
    }
}

static inline unsigned long	call_rm_0x5_without_rexb(int pid, unsigned long instruction,
							 struct user_regs_struct* registers, t_rex* rex,
							 unsigned char rm, char addb)
{
  _(instruction);
  _(rex);
  switch (rm)
    {
    case 0x50:
      return (get_data(pid, registers->rax + addb));
    case 0x51:
      return (get_data(pid, registers->rcx + addb));
    case 0x52:
      return (get_data(pid, registers->rdx + addb));
    case 0x53:
      return (get_data(pid, registers->rbx + addb));
    case 0x55:
      return (get_data(pid, registers->rbp + addb));
    case 0x56:
      return (get_data(pid, registers->rsi + addb));
    case 0x57:
      return (get_data(pid, registers->rdi + addb));
    default:
      return (0);
    }
}


unsigned long	call_rm_0x5(int pid, unsigned long instruction,
			    struct user_regs_struct* registers, t_rex* rex,
			    unsigned char rm, int* offset_rip)
{
  char			addb;

  *offset_rip = 3;
  addb = (instruction & 0xFF0000) >> 16;
  if (rm == 0x54)
    {
      *offset_rip = 4;
      return (get_data(pid, get_sib_addr(pid, (instruction & 0xFF0000) >> 16, registers, rex, 1) + addb));
    }
  if (rex->b)
    return (call_rm_0x5_with_rexb(pid, instruction, registers, rex, rm, addb));
  else
    return (call_rm_0x5_without_rexb(pid, instruction, registers, rex, rm, addb));
}
