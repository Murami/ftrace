#include <stdio.h> /* DBG */
#include <sys/user.h>
#include "ftrace.h"

void		syscall_infos(int pid, unsigned long instruction,
			      struct user_regs_struct* registers, t_data *data)
{
  unsigned long			syscall_no;

  _(instruction);
  _(pid);
  syscall_no = registers->rax;
  write_file(syscall_no, data);
  /* printf("<syscall [%ld]>\n", syscall_no); */
}
