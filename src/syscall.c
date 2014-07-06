/*
** syscall.c for ftrace in /home/vaur/epitech/inprogress/B-PSU-380/AUSP_ftrace
**
** Made by vaur
** Login   <vaur@epitech.net>
**
** Started on  Sun Jul  6 23:33:35 2014 vaur
** Last update Sun Jul  6 23:33:37 2014 vaur
*/

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
}
