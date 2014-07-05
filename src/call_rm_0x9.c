#include <sys/ptrace.h>
#include <sys/user.h>
#include "ftrace.h"

static inline unsigned long	call_rm_0x9_with_rexb(int pid, unsigned long instruction,
						      struct user_regs_struct* registers, t_rex* rex,
						      unsigned char rm, unsigned long addb)
{
  _(instruction);
  _(rex);
  switch (rm)
    {
    case 0x90:
      return (get_data(pid, registers->r8 + addb));
    case 0x91:
      return (get_data(pid, registers->r9 + addb));
    case 0x92:
      return (get_data(pid, registers->r10 + addb));
    case 0x93:
      return (get_data(pid, registers->r11 + addb));
    case 0x95:
      return (get_data(pid, registers->r13 + addb));
    case 0x96:
      return (get_data(pid, registers->r14 + addb));
    case 0x97:
      return (get_data(pid, registers->r15 + addb));
    default:
      return (0);
    }
}

static inline unsigned long	call_rm_0x9_without_rexb(int pid, unsigned long instruction,
							 struct user_regs_struct* registers, t_rex* rex,
							 unsigned char rm, unsigned long addb)
{
  _(instruction);
  _(rex);
  switch (rm)
    {
    case 0x90:
      return (get_data(pid, registers->rax + addb));
    case 0x91:
      return (get_data(pid, registers->rcx + addb));
    case 0x92:
      return (get_data(pid, registers->rdx + addb));
    case 0x93:
      return (get_data(pid, registers->rbx + addb));
    case 0x95:
      return (get_data(pid, registers->rbp + addb));
    case 0x96:
      return (get_data(pid, registers->rsi + addb));
    case 0x97:
      return (get_data(pid, registers->rdi + addb));
    default:
      return (0);
    }
}

unsigned long	call_rm_0x9(int pid, unsigned long instruction,
			    struct user_regs_struct* registers, t_rex* rex,
			    unsigned char rm, int* offset_rip)
{
  unsigned long		addb;

  addb = get_data(pid, registers->rip + 2) & 0xFFFFFFFF;
  *offset_rip = 6;
  if (rm == 0x94)
    {
      *offset_rip = 7;
      return (get_data(pid, get_sib_addr(pid, (instruction & 0xFF0000) >> 16, registers, rex, 2) + addb));
    }
  else if (rex->b)
    return (call_rm_0x9_with_rexb(pid, instruction, registers, rex, rm, addb));
  else
    return (call_rm_0x9_without_rexb(pid, instruction, registers, rex, rm, addb));
}
