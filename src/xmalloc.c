/*
** xmalloc.c for ftrace in /home/vaur/epitech/inprogress/B-PSU-380/AUSP_ftrace/src
**
** Made by vaur
** Login   <vaur@epitech.net>
**
** Started on  Sat Jul  5 19:10:16 2014 vaur
** Last update Sat Jul  5 19:13:08 2014 vaur
*/

/*
** Include
*/

#include	<stdlib.h>
#include	<stdio.h>
#include	"xfcts.h"

/*
** Functions
*/

void	*xmalloc(size_t size)
{
  void	*ptr;

  if ((ptr = malloc(size)) == NULL)
    {
      fprintf(stderr, "Could not Alloc\n");
      exit(EXIT_FAILURE);
    }
  return (ptr);
}
