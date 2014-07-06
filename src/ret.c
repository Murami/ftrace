#include <stdio.h> /* DBG */
#include <sys/user.h>
#include "ftrace.h"

void		ret_infos(int pid, unsigned long instruction,
			  struct user_regs_struct* registers, t_data *data)
{
  _(registers);
  _(instruction);
  _(pid);
  list_pop_front(data->call_stack);
  printf("<ret>\n");
}
