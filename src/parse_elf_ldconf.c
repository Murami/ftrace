/*
** parse_elf_ldconf.c for ftrace in /home/vaur/epitech/inprogress/B-PSU-380/AUSP_ftrace/src
**
** Made by vaur
** Login   <vaur@epitech.net>
**
** Started on  Sat Jul  5 20:30:13 2014 vaur
** Last update Sat Jul  5 20:42:47 2014 vaur
*/

/*
** Include
*/

#include	<dirent.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"parse_elf_internal.h"
#include	"xfcts.h"

/*
** Functions
*/

void		read_ldconf_file(char *file, char *folder, t_list *list_folder)
{
  FILE		*file_stream;
  char		*line;
  size_t	n;
  char		*path;

  path = xmalloc((strlen(file) + strlen(folder)) * sizeof(path));
  path = strcpy(path, folder);
  path = strcat(path, file);
  /* printf("file: %s\n", path); */
  file_stream = fopen(path, "r");
  if (file_stream == NULL)
    {
      printf("Cant open file\n");
      return ;
    }
  n = 0;
  line = NULL;
  while (getline(&line, &n, file_stream) != -1)
    {
      if (strncmp(line, "#", 1) != 0 && strcmp(line, "\n") != 0)
	list_push_back(list_folder, strndup(line, strlen(line) -1));
      free(line);
      line = NULL;
      n = 0;
    }
  fclose(file_stream);
}

/*
** Public Functions
*/

t_list		*load_ldconf()
{
  DIR		*dir;
  struct dirent	*entry;
  char		*folder;
  t_list	*list_folder;

  list_folder = list_new();
  folder = strdup("/etc/ld.so.conf.d/");
  dir = opendir(folder);
  while ((entry = readdir(dir)) != NULL)
    {
      if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
	read_ldconf_file(entry->d_name, folder, list_folder);
    }
  closedir(dir);
  return (list_folder);
}
