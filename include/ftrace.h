#ifndef FTRACE_H
#define FTRACE_H

#define	_(a)	((void)a)

#define SYSCALL		1
#define CALL		2
#define RET		3

struct user_regs_struct;

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
void		syscall_infos(int pid, unsigned long instruction, struct user_regs_struct* registers);
void		call_infos(int pid, unsigned long instruction, struct user_regs_struct* registers);
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
void		ret_infos(int pid, unsigned long instruction, struct user_regs_struct* registers);


#endif /* FTRACE_H */