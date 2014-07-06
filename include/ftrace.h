#ifndef FTRACE_H
#define FTRACE_H

#define	_(a)	((void)a)

#define SYSCALL		1
#define CALL		2
#define RET		3

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"

struct user_regs_struct;

typedef struct	s_link
{
  unsigned long parent;
  unsigned long son;
  bool		valid_call;
}		t_link;

typedef struct	s_stack_entry
{
  unsigned long	addr;
  bool		valid_call;
}		t_stack_entry;

typedef struct	s_symbol
{
  char		*name;
  unsigned long	addr;
  int		is_dynamic;
}		t_symbol;

typedef struct	s_data
{
  int		file;
  t_list	*call_stack;
  t_list	*link_list;
  t_list	*sym_list;
  t_list	*map_info;
}		t_data;

typedef struct	s_config
{
  int		pid;
  char*		command;
  char**	av;
  int		ac;
}		t_config;

typedef struct	s_rex
{
  char		w;
  char		r;
  char		x;
  char		b;
}		t_rex;

typedef struct	s_sib
{
  char		scale;
  char		index;
  char		base;
}		t_sib;

int		ftrace(t_config* config);
long		get_data(int pid, long addr);
int		get_registers(int pid, struct user_regs_struct* registers);
long		get_instruction(int pid, struct user_regs_struct* registers);
int		get_instruction_type(long instr);
void		syscall_infos(int pid, unsigned long instruction, struct user_regs_struct* registers, t_data *data);
void		call_infos(int pid, unsigned long instruction, struct user_regs_struct* registers, t_data *data);
unsigned long	call_rm_0x1(int pid, unsigned long instruction, struct user_regs_struct* registers,
			    t_rex* rex, unsigned char rm, int* offset_rip);
unsigned long	call_rm_0x5(int pid, unsigned long instruction, struct user_regs_struct* registers,
			    t_rex* rex, unsigned char rm, int* offset_rip);
unsigned long	call_rm_0x9(int pid, unsigned long instruction, struct user_regs_struct* registers,
			    t_rex* rex, unsigned char rm, int* offset_rip);
unsigned long	call_rm_0xD(int pid, unsigned long instruction, struct user_regs_struct* registers,
			    t_rex* rex, unsigned char rm);
unsigned long	get_sib_addr(int pid, unsigned char sib_byte,
			     struct user_regs_struct* registers,
			     t_rex* rex, char mod);
int		parser(t_config *config, int ac, char **av);
void		ret_infos(int pid, unsigned long instruction, struct user_regs_struct* registers, t_data *data);
t_list*		parse_elf(int pid);
void		write_file(unsigned long call_addr, t_data *data);

#endif /* FTRACE_H */
