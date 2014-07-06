/*
** parse_elf.c for ftrace in /home/vaur/epitech/inprogress/B-PSU-380/AUSP_ftrace/src
**
** Made by vaur
** Login   <vaur@epitech.net>
**
** Started on  Sat Jul  5 18:43:13 2014 vaur
** Last update Sat Jul  5 19:32:36 2014 vaur
*/

/*
** Include
*/

#if DEBUG_ == 1
# include	<unistd.h>
#endif

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"parse_elf.h"
#include	"parse_elf_internal.h"
#include	"xfcts.h"


/*
** Static Functions
*/

inline static void	display_msg(const char *msg)
{
  printf("%s\n%s\n%s\n", MSG_DECORATION, msg, MSG_DECORATION);
}

static char	*get_prog_path(int pid)
{
  char		path[100];

  sprintf(path, "%s%d/exec", PROC_PATH, pid);
  return (strdup(path));
}

/* static void	read_file(const char *prog_path) */
/* { */
/*   int		fd; */

/*   if((fd = open(prog_path, O_RDONLY)) == -1) */
/*     { */
/*       printf("couldnt open %s\n", prog_path); */
/*       exit(EXIT_FAILURE); */
/*     } */
/*   close(fd); */
/* } */

/*
** Public Functions
*/

void	parse_elf(int pid)
{
  char	*prog_path;

  (void)pid;

  display_msg("parsing elf");
  prog_path = get_prog_path(pid);
  printf("\"%s\"\n", prog_path);
  /* parse_file(prog_path); */

  free(prog_path);

#if DEBUG_ == 1
  pause();
#endif
}
