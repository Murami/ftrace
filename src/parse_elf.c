/*
** parse_elf.c for ftrace in /home/vaur/epitech/inprogress/B-PSU-380/AUSP_ftrace/src
**
** Made by vaur
** Login   <vaur@epitech.net>
**
** Started on  Sat Jul  5 18:43:13 2014 vaur
** Last update Sat Jul  5 20:15:22 2014 vaur
*/

/*
** Include
*/

#if DEBUG_ == 1
# include	<unistd.h>
#endif

#include	<fcntl.h>
#include	<errno.h>
#include	<libelf.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/types.h>
#include	<sys/stat.h>

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
  char		path_link[100];
  char		path[100];

  sprintf(path_link, "%s%d/exe", PROC_PATH, pid);
  if (readlink(path_link, path, strlen(path_link)) == -1)
    {
      fprintf(stderr, "readlink \"%s\": %s\n", path_link, strerror(errno));
      exit(EXIT_FAILURE);
    }
  printf("path_link: %s\n", path_link);
  return (strdup(path));
}

static Elf*	open_elf(const char* filename)
{
  int		fd;
  Elf*		elf;

  elf_version(EV_CURRENT);
  if((fd = open(filename, O_RDONLY)) == -1)
    {
      fprintf(stderr, "open %s: %s", filename, strerror(errno));
      exit(EXIT_FAILURE);
    }
  if (!(elf = elf_begin(fd, ELF_C_READ, NULL)))
    {
      fprintf(stderr, "elf_begin %s: %s (%d)", filename, elf_errmsg(elf_errno()), elf_errno());
      exit(EXIT_FAILURE);
    }
  close(fd);
  return (elf);
}

/*
** Public Functions
*/

void	parse_elf(int pid)
{
  char	*prog_path;
  Elf	*elf;

  display_msg("parsing elf");
  prog_path = get_prog_path(pid);
  printf("\"%s\"\n", prog_path);
  elf = open_elf(prog_path);

  elf_end(elf);
  free(prog_path);

#if DEBUG_ == 1
  pause();
#endif
}
