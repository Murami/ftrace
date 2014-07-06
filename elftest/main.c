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
      /* printf("%s -- %s -- %d\n", elf_strptr(elf, ehdr.e_shstrndx, shdr.sh_name), */
      /* 	     name, */
      /* 	     strcmp(elf_strptr(elf, ehdr.e_shstrndx, shdr.sh_name), name)); */
    }
  while (section != NULL && strcmp(elf_strptr(elf, ehdr.e_shstrndx, shdr.sh_name), name) != 0);
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

t_list*		parse_elf(char* elf_file)
{
  Elf*		elf;
  int		fd;

  elf_version(EV_CURRENT);

  if((fd = open(elf_file, O_RDONLY)) == -1)
    return;
  if (!(elf = elf_begin(fd, ELF_C_READ, NULL)))
    return;

  /* STATIC SYMBOLS */
  Elf_Data*	data;
  Elf_Scn*	section;
  Elf64_Shdr	shdr;
  GElf_Sym	sym;
  int		i;
  section = get_section_by_name(elf, ".symtab");
  gelf_getshdr(section, &shdr);
  data = elf_getdata(section, NULL);
  for (i = 0; i < shdr.sh_size / shdr.sh_entsize; i++)
    {
      gelf_getsym(data, i, &sym);
      if (ELF64_ST_TYPE(sym.st_info) == STT_FUNC && sym.st_value != 0)
  	printf("%p -- %s\n", (void*)sym.st_value, elf_strptr(elf, shdr.sh_link, sym.st_name));
    }

  /* FIND SECTIONS */
  /* Elf64_Ehdr*	ehdr; */
  /* Elf_Scn*	section; */
  /* Elf64_Shdr	shdr; */

  /* ehdr = elf64_getehdr(elf); */
  /* section = NULL; */
  /* do */
  /*   { */
  /*     section = elf_nextscn(elf, section); */
  /*     gelf_getshdr(section, &shdr); */
  /*     if (shdr.sh_type == SHT_RELA) */
  /* 	printf("%s\n", elf_strptr(elf, ehdr->e_shstrndx, shdr.sh_name)); */
  /*   } */
  /* while (section != NULL); */

  /* FIND SYMBOLS DYNAMIC */
  Elf_Scn*	dynstr;
  Elf_Scn*	dynsym;
  /* Elf_Scn*	section; */
  /* GElf_Shdr	shdr; */
  GElf_Rela	rela;
  /* GElf_Sym	sym; */
  /* int		i; */
  unsigned long	symval;

  dynsym = get_section_by_name(elf, ".dynsym");
  dynstr = get_section_by_name(elf, ".dynstr");

  section = NULL;
  while (section = elf_nextscn(elf, section))
    {
      gelf_getshdr(section, &shdr);
      if (shdr.sh_type == SHT_RELA)
	{
	  for (i = 0; i < shdr.sh_size / shdr.sh_entsize; i++)
	    {
	      gelf_getrela(elf_getdata(section, NULL), i, &rela);
	      if (ELF64_R_TYPE(rela.r_info) == R_386_JMP_SLOT)
		{
		  gelf_getsym(elf_getdata(dynsym, NULL), ELF64_R_SYM(rela.r_info), &sym);
		  lseek(fd, rela.r_offset - 0x600000, SEEK_SET);
		  read(fd, &symval, 8);
		  printf("0x%lx -- %s\n", symval, elf_strptr(elf, elf_ndxscn(dynstr), sym.st_name));
		}
	    }
	}
    }
  elf_end(elf);
}

void	usage(char* exec_name)
{
  printf("usages: %s [elf/.ar file]\n", exec_name);
}

int	main(int ac, char** av)
{
  if (ac == 2)
    parse_elf(av[1]);
  else
    usage(av[0]);
  return (EXIT_SUCCESS);
}
