#include <stdio.h> /* DBG */
#include <sys/ptrace.h>
#include <sys/user.h>
#include "ftrace.h"

static inline void		call_relative(int pid, unsigned long instruction,
					      struct user_regs_struct* registers, t_rex* rex)
{
  unsigned long	call_addr;
  int		offset;

  _(pid);
  offset = (int)(instruction >> 8);
  if (rex->w)
    {
      /* WUT ? ca fait la meme chose que instruction >> 8, non ??? */
      /* offset = ptrace(PTRACE_PEEKDATA, pid, registers->rip + 1); */
      call_addr = registers->rip + offset + 9;
    }
  else
    {
      call_addr = registers->rip + offset + 5;
    }
  printf("<relative call [%lx]>\n", call_addr);
}

static inline void		call_absolute(int pid, unsigned long instruction,
					      struct user_regs_struct* registers, t_rex* rex)
{
  unsigned char	rm;
  unsigned long	addr;
  int		offset_rip;

  rm = (instruction & 0xFF00) >> 8;
  offset_rip = 2;

  if (rm >= 0xD0 && rm <= 0xD7) /* register direct */
    addr = call_rm_0xD(pid, instruction, registers, rex, rm);
  else if (rm >= 0x10 && rm <= 0x17) /* register indirect */
    addr = call_rm_0x1(pid, instruction, registers, rex, rm, &offset_rip);
  else if (rm >= 0x50 && rm <= 0x57) /* one-byte signed displacment */
    addr = call_rm_0x5(pid, instruction, registers, rex, rm, &offset_rip);
  else if (rm >= 0x90 && rm <= 0x97) /* four-byte signed displacement */
    addr = call_rm_0x9(pid, instruction, registers, rex, rm, &offset_rip);
  printf("<absolute call [%lx]>  {%lx}\n", addr, instruction);
}

void		call_infos(int pid, unsigned long instruction, struct user_regs_struct* registers)
{
  t_rex		rex = {0, 0, 0, 0};

  if ((instruction & 0xF0) == 0x40)
    {
      rex.w = instruction & 0x4;
      rex.r = instruction & 0x3;
      rex.x = instruction & 0x2;
      rex.b = instruction & 0x1;
      registers->rip++;
      instruction = get_data(pid, registers->rip);
      if (instruction == 0)
	return;
    }
  if ((instruction & 0x000000ff) == 0xe8)
    call_relative(pid, instruction, registers, &rex);
  if ((instruction & 0x000000ff) == 0xff && (instruction & 0x3800) == 0x1000)
    call_absolute(pid, instruction, registers, &rex);
}
