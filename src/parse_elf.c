#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <elf.h>
#include <libelf.h>
#include <gelf.h>
#include <unistd.h>
#include "list.h"
#include "ftrace.h"

#define PROC_PATH	"/proc/"

static char	*get_prog_path(int pid)
{
  char		path_link[100];
  char		path[256];

  memset(path, 0, 256);
  sprintf(path_link, "%s%d/exe", PROC_PATH, pid);
  if (readlink(path_link, path, 256) == -1)
    {
      fprintf(stderr, "readlink \"%s\": %s\n", path_link, strerror(errno));
      exit(EXIT_FAILURE);
    }
  /* printf("path_link: %s\n", path_link); */
  /* printf("path: %s\n", path); */
  return (strdup(path));
}

Elf_Scn*	get_section_by_type(Elf* elf, unsigned int type)
{
  Elf_Scn*	section;
  Elf64_Shdr	shdr;

  section = NULL;
  do
    {
      section = elf_nextscn(elf, section);
      gelf_getshdr(section, &shdr);
    }
  while (section != NULL && shdr.sh_type != type);
  return (section);
}

Elf_Scn*	get_section_by_name(Elf* elf, const char* name)
{
  Elf_Scn*	section;
  GElf_Ehdr	ehdr;
  Elf64_Shdr	shdr;

  gelf_getehdr(elf, &ehdr);
  section = NULL;
  do
    {
      section = elf_nextscn(elf, section);
      gelf_getshdr(section, &shdr);
    }
  while (section != NULL && strcmp(elf_strptr(elf, ehdr.e_shstrndx, shdr.sh_name), name) != 0);
  return (section);
}

t_list*		parse_elf(int pid)
{
  Elf*		elf;
  Elf_Data*	data;
  Elf_Scn*	section;
  Elf64_Shdr	shdr;
  GElf_Sym	sym;
  int		i;
  int		fd;
  Elf_Scn*	dynstr;
  Elf_Scn*	dynsym;
  GElf_Rela	rela;
  unsigned long	symval;
  char*		filename;
  t_list*	list;
  t_symbol	*symbol;

  elf_version(EV_CURRENT);
  filename = get_prog_path(pid);
  if((fd = open(filename, O_RDONLY)) == -1)
    return (NULL);
  if (!(elf = elf_begin(fd, ELF_C_READ, NULL)))
    return (NULL);
  list = list_new();
  section = get_section_by_type(elf, SHT_SYMTAB);
  if (!section)
    section = get_section_by_type(elf, SHT_DYNSYM);
  gelf_getshdr(section, &shdr);
  data = elf_getdata(section, NULL);
  for (i = 0; (unsigned)i < shdr.sh_size / shdr.sh_entsize; i++)
    {
      gelf_getsym(data, i, &sym);
      if (ELF64_ST_TYPE(sym.st_info) == STT_FUNC && sym.st_value != 0)
	{
	  symbol = malloc(sizeof (t_symbol));
	  symbol->name = strdup(elf_strptr(elf, shdr.sh_link, sym.st_name));
	  symbol->addr = sym.st_value;
	  symbol->is_dynamic = 0;
	  list_push_front(list, symbol);
	  printf("%p -- %s\n", (void*)sym.st_value, elf_strptr(elf, shdr.sh_link, sym.st_name));
	}
    }

  dynsym = get_section_by_name(elf, ".dynsym");
  dynstr = get_section_by_name(elf, ".dynstr");
  section = NULL;
  while ((section = elf_nextscn(elf, section)))
    {
      gelf_getshdr(section, &shdr);
      if (shdr.sh_type == SHT_RELA)
	{
	  for (i = 0; (unsigned)i < shdr.sh_size / shdr.sh_entsize; i++)
	    {
	      gelf_getrela(elf_getdata(section, NULL), i, &rela);
	      if (ELF64_R_TYPE(rela.r_info) == R_386_JMP_SLOT)
		{
		  gelf_getsym(elf_getdata(dynsym, NULL), ELF64_R_SYM(rela.r_info), &sym);
		  lseek(fd, rela.r_offset - 0x600000, SEEK_SET);
		  read(fd, &symval, 8);

		  symbol = malloc(sizeof (t_symbol));
		  symbol->name = strdup(elf_strptr(elf, elf_ndxscn(dynstr), sym.st_name));
		  symbol->addr = symval;
		  symbol->is_dynamic = 1;
		  list_push_front(list, symbol);
		  printf("0x%lx -- %s\n", symval, elf_strptr(elf, elf_ndxscn(dynstr), sym.st_name));
		}
	    }
	}
    }
  elf_end(elf);
  close(fd);
  free(filename);
  return (list);
}
