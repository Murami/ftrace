#include "elf_file.h"

/* CTOR/DTOR */

void		elf_file_constructor(elf_file* this)
{
  this->elf_file_handle = NULL;
  this->m_opened = false;
  this->elf_file_fd = -1;
}

void		elf_file_destructor(elf_file* this)
{
  elf_end(this->elf_file_handle);
  close(this->elf_file_fd);
}

/* METHODS */

bool		elf_file_open(t_elf_file* this, const char* filename)
{
  m_file_descriptor = open(filename, O_RDONLY);
  if (m_file_descriptor == -1)
    return (false);
  m_elf_handle = elf_begin(m_file_descriptor, ELF_C_READ, NULL);
  if (m_elf_handle == NULL)
    return (false);
  this->m_opened = true;
  return (true);
}

bool		elf_file_is_open(t_elf_file* this)
{
  return (this->m_opened);
}

char*		elf_file_get_symbol_name(t_elf_file* this, unsigned int symbole_addr)
{
  Elf_Scn*	section;
  Elf64_Shdr	shdr;
  Elf_Data*	data;
  GElf_Sym	sym;
  int		symbol_count = shdr.sh_size / shdr.sh_entsize;
  int		i;

  section = NULL;
  do
    {
      section = elf_nextscn(elf_file_handle, section);
      if (section)
	gelf_getshdr(section, &shdr);
    }
  while (section != NULL && shdr.sh_type != SHT_SYMTAB);
  data = elf_getdata(section, NULL);
  for (i = 0; i < symbol_count; i++)
    {
      gelf_getsym(data, i, &sym);
      if (sym.st_value == ptr_sym)
	return (elf_strptr(elf_file_handle, shdr.sh_link, sym.st_name));
    }
  return (NULL);
}

/* OPERATORS */

t_elf_file*	elf_file_new()
{
  t_elf_file*	elf_file;

  elf_file = malloc(sizeof(t_elf_file));
  elf_file_constructor(elf_file);
  return (elf_file);
}

void		elf_file_delete(t_elf_file* this)
{
  elf_file_destructor(this);
  free(this);
}
