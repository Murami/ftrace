/*
** parse_map.c for ftrace in /home/vaur/epitech/inprogress/B-PSU-380/AUSP_ftrace/src
**
** Made by vaur
** Login   <vaur@epitech.net>
**
** Started on  Sun Jul  6 18:14:04 2014 vaur
** Last update Sun Jul  6 23:28:02 2014 vaur
*/

/*
** Include
*/

#include	<errno.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	"map_entry.h"
#include	"parse_map.h"
#include	"xfcts.h"

/*
** Define
*/

#define PROC_PATH	"/proc/"

/*
** Functions
*/

static t_map_entry	*new_entry(char *str_ptr_start, char *str_ptr_end, char *filename)
{
  t_map_entry		*new_entry;

  new_entry = xmalloc(sizeof(*new_entry));
  new_entry->ptr_start = strtol(str_ptr_start, NULL, 16);
  new_entry->ptr_end = strtol(str_ptr_end, NULL, 16);
  new_entry->filename = strdup(filename);
  return (new_entry);
}

static void	read_line_map(char *line, t_list *map_info)
{
  int		i;
  char		*str_ptr_start;
  char		*str_ptr_end;
  char		*filename;

  i = 0;
  errno = 0;
  str_ptr_start = strtok(line, "-");
  str_ptr_end = strtok(NULL, " ");
  while (i++ < 4)
    strtok(NULL, " ");
  filename = strtok(NULL, " \n");
  if (filename != NULL && filename[0] == '/')
    {
      list_push_back(map_info, new_entry(str_ptr_start, str_ptr_end, filename));
    }
}

static void	read_map(FILE *file_stream, t_list *map_info)
{
  char		*line;
  size_t	n;

  line = NULL;
  n = 0;
  while (getline(&line, &n, file_stream) != -1)
    {
      read_line_map(line, map_info);
      free(line);
      line = NULL;
      n = 0;
    }
}

static char	*get_map_name(int pid)
{
  char		path[256];

  memset(path, 0, 256);
  sprintf(path, "%s%d/maps", PROC_PATH, pid);
  return (strdup(path));
}

t_list		*parse_map(int pid)
{
  FILE		*file_stream;
  char		*map_file;
  t_list	*map_info;

  map_file = get_map_name(pid);
  map_info = list_new();
  if ((file_stream = fopen(map_file, "r")) == NULL)
    perror("fopen");
  else
    {
      read_map(file_stream, map_info);
      fclose(file_stream);
    }
  return (map_info);
}
