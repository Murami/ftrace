/*
** write_syscall.c for ftrace in /home/vaur/epitech/inprogress/B-PSU-380/AUSP_ftrace
**
** Made by vaur
** Login   <vaur@epitech.net>
**
** Started on  Sun Jul  6 21:47:34 2014 vaur
** Last update Sun Jul  6 21:50:26 2014 vaur
*/

/*
** Include
*/

#include	<string.h>
#include	<unistd.h>
#include	"syscallent.h"
#include	"write_syscall.h"

/*
** Functions
*/

void	write_syscall(int fd, unsigned long addr)
{
  write(fd, syscallent[(int)addr].callname, strlen(syscallent[(int)addr].callname));
}
