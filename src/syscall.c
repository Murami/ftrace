#include <stdio.h> /* DBG */
#include <sys/user.h>
#include "ftrace.h"

void		syscall_infos(int pid, unsigned long instruction, struct user_regs_struct* registers)
{
  unsigned long			syscall_no;

  _(instruction);
  _(pid);
  syscall_no = registers->rax;
  printf("<syscall [%ld]>\n", syscall_no);
}
