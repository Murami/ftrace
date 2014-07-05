#include <sys/ptrace.h>
#include <sys/user.h>
#include "ftrace.h"

long				get_data(int pid, long addr)
{
  long				instr;

  if ((instr = ptrace(PTRACE_PEEKDATA, pid, (void*)addr), 0) == -1)
    return (0);
  return (instr);
}

int				get_registers(int pid, struct user_regs_struct* registers)
{
  if (ptrace(PTRACE_GETREGS, pid, 0, registers) == -1)
    return (0);
  return (1);
}

long				get_instruction(int pid, struct user_regs_struct* registers)
{
  long				instr;

  if ((instr = ptrace(PTRACE_PEEKDATA, pid, (void*)registers->rip), 0) == -1)
    return (0);
  return (instr);
}

int				get_instruction_type(long instr)
{
  if ((instr & 0x000000ff) == 0xc2 ||
      (instr & 0x000000ff) == 0xc3 ||
      (instr & 0x000000ff) == 0xca ||
      (instr & 0x000000ff) == 0xcb)
    return (RET);

  if ((instr & 0x000000ff) == 0xe8 ||
      (instr & 0x000000ff) == 0xff)
    return (CALL);

  if ((instr & 0x0000ffff) == 0x050f ||
      (instr & 0x0000ffff) == 0x340f ||
      (instr & 0x0000ffff) == 0x80cd)
    return (SYSCALL);
  return (0);
}
