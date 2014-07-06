/*
** parse_map.c for ftrace in /home/vaur/epitech/inprogress/B-PSU-380/AUSP_ftrace/src
**
** Made by vaur
** Login   <vaur@epitech.net>
**
** Started on  Sun Jul  6 18:14:04 2014 vaur
** Last update Sun Jul  6 18:58:50 2014 vaur
*/

/*
** Include
*/

#include	<errno.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	"xfcts.h"


/* #include	"map_entry.h" */

/*
** Functions
*/

static void	new_entry(char *str_ptr_start, char str_ptr_end, char *filename)
{

}

static void	read_line_map(char *line)
{
  int		i;
  char		*str_ptr_start;
  char		*str_ptr_end;
  char		*str_offset;
  char		*filename;

  i = 0;
  errno = 0;
  /* strtok(line, "- "); */
  /* if (strtok(NULL, "- ") == NULL) */
  /*   printf("STRTOK FAIL\n"); */
  str_ptr_start = strtok(line, "-");
  str_ptr_end = strtok(NULL, " ");
  while (i++ < 4)
    strtok(NULL, " ");
  filename = strtok(NULL, " ");
  if (filename != NULL && filename[0] == '/')
    {
      new_entry(str_ptr_start, str_ptr_end, filename);
    }
}

static void	read_map(FILE *file_stream)
{
  char		*line;
  size_t	n;

  line = NULL;
  n = 0;
  while (getline(&line, &n, file_stream) != -1)
    {
      read_line_map(line);
      free(line);
      line = NULL;
      n = 0;
    }
}

void	parse_map(const char *map_file)
{
  FILE	*file_stream;

  if ((file_stream = fopen(map_file, "r")) == NULL)
    perror("fopen");
  read_map(file_stream);
  fclose(file_stream);
}
