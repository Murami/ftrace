/*
** parse_elf_internal.h for ftrace in /home/vaur/epitech/inprogress/B-PSU-380/AUSP_ftrace/src
**
** Made by vaur
** Login   <vaur@epitech.net>
**
** Started on  Sat Jul  5 18:55:06 2014 vaur
** Last update Sat Jul  5 20:40:32 2014 vaur
*/

#ifndef		PARSE_ELF_INTERNAL_H
# define	PARSE_ELF_INTERNAL_H

/*
** Include
*/

# include	<libelf.h>
# include	"list.h"

/*
** Define
*/

# define	MSG_DECORATION	"============================="
# define	PROC_PATH	"/proc/"

/*
** Prototype
*/

/*
** parse_elf_read.c:
*/

void		read_elf(Elf *elf);

/*
** parse_elf_ldconf.c:
*/

t_list		*load_ldconf();

#endif      /* !PARSE_ELF_INTERNAL_H_ */
