#ifndef ELF_FILE_H
# define ELF_FILE_H

# include <stdbool.h>
# include "elf_section.h"

typedef struct		s_elf_file
{
  int			m_file_descriptor;
  Elf*			m_elf_handle;
  bool			m_opened;
}			t_elf_file;

/* CTOR/DTOR */

void		elf_file_constructor(t_elf_file* this);
void		elf_file_destructor(t_elf_file* this);

/* METHODS */

bool		elf_file_open(t_elf_file* this, const char* filename);
bool		elf_file_is_open(t_elf_file* this);
char*		elf_file_get_symbol_name(t_elf_file* this, unsigned int symbole_addr);

/* OPERATORS */

t_elf_file*	elf_file_new();
void		elf_file_delete(t_elf_file* this);

#endif /* ELF_FILE_H */
