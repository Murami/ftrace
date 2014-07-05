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

Elf*		open_elf(const char* filename)
{
  int		fd;
  Elf*		elf;

  if((fd = open(filename, O_RDONLY)) == -1)
    {
      fprintf(stderr, "open %s: %s", filename, strerror(errno));
      return (NULL);
    }
  if (!(elf = elf_begin(fd, ELF_C_READ, NULL)))
    {
      fprintf(stderr, "elf_begin %s: %s (%d)", filename, elf_errmsg(elf_errno()), elf_errno());
      return (NULL);
    }
  close(fd);
  return (elf);
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

/*
** Eviter de rechercher la section a chaque fois en la prenant en param
** Creer un structure par fichier elf qui contient tous les donnée pour
** une recherche plus rapide
*/

char*		get_static_symname(Elf* elf, unsigned int symaddr)
{
  Elf_Data*	data;
  Elf_Scn*	section;
  Elf64_Shdr	shdr;
  GElf_Sym	sym;
  int		i;

  section = get_section_by_type(elf, SHT_SYMTAB);
  if (section == NULL)
    printf("RAGE\n");
  gelf_getshdr(section, &shdr);
  data = elf_getdata(section, NULL);
  for (i = 0; i < shdr.sh_size / shdr.sh_entsize; i++)
    {
      gelf_getsym(data, i, &sym);
      if (sym.st_value == symaddr)
	return (elf_strptr(elf, shdr.sh_link, sym.st_name));
    }
  return (NULL);
}

unsigned int	hex_to_uint(char* hex)
{
  return (strtol(hex, NULL, 0));
}

void		symbol_finder(char* elf_file, char* str_ptr_sym)
{
  Elf*		elf;
  int		ptr_sym;

  ptr_sym = hex_to_uint(str_ptr_sym);
  elf_version(EV_CURRENT);
  elf = open_elf(elf_file);
  printf("symbol name : [%s]\n", get_static_symname(elf, ptr_sym));
  elf_end(elf);
}

void	usage(char* exec_name)
{
  printf("usages: %s [elf/.ar file]\n", exec_name);
}

int	main(int ac, char** av)
{
  if (ac == 3)
    symbol_finder(av[1], av[2]);
  else
    usage(av[0]);
  return (EXIT_SUCCESS);
}
