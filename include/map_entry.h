/*
** map_entry.h for ftrace in /home/vaur/epitech/inprogress/B-PSU-380/AUSP_ftrace
**
** Made by vaur
** Login   <vaur@epitech.net>
**
** Started on  Sun Jul  6 18:34:11 2014 vaur
** Last update Sun Jul  6 18:35:51 2014 vaur
*/

#ifndef		MAP_ENTRY_H
# define	MAP_ENTRY_H

/*
** Typedef
*/

typedef struct s_map_entry	t_map_entry;

/*
** Struct
*/

struct		s_map_entry
{
  unsigned long	ptr_start;
  unsigned long	ptr_end;
  unsigned long	offset;
  char		*filename;
};

#endif      /* !MAP_ENTRY_H_ */
