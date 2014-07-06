/*
** parse_elf_read.c for ftrace in /home/vaur/epitech/inprogress/B-PSU-380/AUSP_ftrace/src
**
** Made by vaur
** Login   <vaur@epitech.net>
**
** Started on  Sat Jul  5 20:25:07 2014 vaur
** Last update Sun Jul  6 16:57:57 2014 vaur
*/

/*
** Include
*/

#include	<dirent.h>
#include	<stdio.h>
#include	<string.h>
#include	"parse_elf_internal.h"

/*
** Functions
*/

/*
** Static Functions
*/

/*
** Public Functions
*/

void			search_libpath(const char *name, t_list *list_folder)
{
  t_list_iterator	it_list;
  DIR			*dir;
  struct dirent		*entry;

  it_list = list_begin(list_folder);
  while (it_list != list_end(list_folder))
    {
      dir = opendir((char*)it_list->data);
      while (dir != NULL && (entry = readdir(dir)) != NULL)
	{
	  if (strcmp(entry->d_name, name) == 0)
	    {
	      printf("%s/%s\n", it_list->data, name);
	      closedir(dir);
	      return ;
	    }
	}
      closedir(dir);
      it_list = list_iterator_next(it_list);
    }
}

void			read_elf(Elf *elf)
{
  t_list		*list_folder;
  t_list_iterator	it_list;

  list_folder = load_ldconf();
}
