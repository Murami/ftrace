#include <stdio.h> /* DBG */
#include <sys/user.h>
#include "ftrace.h"

void		ret_infos(int pid, unsigned long instruction,
			  struct user_regs_struct* registers)
{
  _(registers);
  _(instruction);
  _(pid);
  printf("<ret>\n");
}
