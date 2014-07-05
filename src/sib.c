#include <sys/user.h>
#include "ftrace.h"

static inline unsigned long	get_sib_index_with_rexx(char index,
							struct user_regs_struct* registers)
{
  switch (index)
    {
    case 0:
      return (registers->r8);
    case 1:
      return (registers->r9);
    case 2:
      return (registers->r10);
    case 3:
      return (registers->r11);
    case 4:
      return (registers->r12);
    case 5:
      return (registers->r13);
    case 6:
      return (registers->r14);
    case 7:
      return (registers->r15);
    default:
      return (0);
    }
}

static inline unsigned long	get_sib_index_without_rexx(char index,
							   struct user_regs_struct* registers)
{
  switch (index)
    {
    case 0:
      return (registers->rax);
    case 1:
      return (registers->rcx);
    case 2:
      return (registers->rdx);
    case 3:
      return (registers->rbx);
    case 4:
      return (registers->rsp);
    case 5:
      return (registers->rbp);
    case 6:
      return (registers->rsi);
    case 7:
      return (registers->rdi);
    default:
      return (0);
    }
}

static inline unsigned long	get_sib_index(char index,
					      struct user_regs_struct* registers,
					      t_rex* rex)
{
  if (rex->x)
    return (get_sib_index_with_rexx(index, registers));
  else
    return (get_sib_index_without_rexx(index, registers));
}

static inline unsigned long	get_sib_scale(char scale)
{
  switch (scale)
    {
    case 0:
      return (1);
    case 1:
      return (2);
    case 2:
      return (4);
    case 3:
      return (8);
    default:
      return (1);
    }
}

static inline unsigned long	get_sib_base_with_rexb(char base,
						       struct user_regs_struct* registers)
{
  switch (base)
    {
    case 0:
      return (registers->r8);
    case 1:
      return (registers->r9);
    case 2:
      return (registers->r10);
    case 3:
      return (registers->r11);
    case 4:
      return (registers->r12);
    case 5:
      return (registers->r13);
    case 6:
      return (registers->r14);
    case 7:
      return (registers->r15);
    default:
      return (0);
    }
}


static inline unsigned long	get_sib_base_without_rexb(char base, struct user_regs_struct* registers)
{
  switch (base)
    {
    case 0:
      return (registers->rax);
    case 1:
      return (registers->rcx);
    case 2:
      return (registers->rdx);
    case 3:
      return (registers->rbx);
    case 4:
      return (registers->rsp);
    case 5:
      return (registers->rbp);
    case 6:
      return (registers->rsi);
    case 7:
      return (registers->rdi);
    default:
      return (0);
    }
}

static inline unsigned long	get_sib_base(int pid, char base, struct user_regs_struct* registers,
					     t_rex* rex, char mod)
{
  if (!mod && base == 5)
    return (get_data(pid, registers->rip + 3) & 0xFFFFFFFF);
  if (rex->b)
    return (get_sib_base_with_rexb(base, registers));
  else
    return (get_sib_base_without_rexb(base, registers));
}

unsigned long	get_sib_addr(int pid, unsigned char sib_byte,
			     struct user_regs_struct* registers,
			     t_rex* rex, char mod)
{
  t_sib		sib;
  unsigned long	result = 0;

  sib.scale = sib_byte & 0xC0;
  sib.index = sib_byte & 0x38;
  sib.base = sib_byte & 0x07;

  result = get_sib_index(sib.index, registers, rex);
  result *= get_sib_scale(sib.scale);
  result += get_sib_base(pid, sib.base, registers, rex, mod);
  return (result);
}
