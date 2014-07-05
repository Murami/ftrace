#include <sys/user.h>
#include <sys/ptrace.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

# define SYSCALL	1
# define CALL		2
# define RET		3

/* # define MOD_REGISTER_INDIRECT		(0x0) */
/* # define MOD_ONE_BYTE_DISPLACEMENT	(0x1) */
/* # define MOD_FOUR_BYTE_DISPLACEMENT	(0x2) */
/* # define MOD_REGISTER_DIRECT		(0x3) */

/* # define REG_RAX			(0x0) */
/* # define REG_RCX			(0x1) */
/* # define REG_RDX			(0x2) */
/* # define REG_RBX			(0x3) */
/* # define REG_RSP			(0x4) */
/* # define REG_RBP			(0x5) */
/* # define REG_RSI			(0x6) */
/* # define REG_RDI			(0x7) */

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

long				get_data(int pid, long addr)
{
  long				instr;

  if ((instr = ptrace(PTRACE_PEEKDATA, pid, (void*)addr), 0) == -1)
    return (0);
  return (instr);
}

int				get_registers(int pid, struct user_regs_struct* registers)
{
  if (ptrace(PTRACE_GETREGS, pid, 0, registers) == -1)
    return (0);
  return (1);
}

long				get_instruction(int pid, struct user_regs_struct* registers)
{
  long				instr;

  if ((instr = ptrace(PTRACE_PEEKDATA, pid, (void*)registers->rip), 0) == -1)
    return (0);
  return (instr);
}

int				get_instruction_type(long instr)
{
  if ((instr & 0x000000ff) == 0xc2 ||
      (instr & 0x000000ff) == 0xc3 ||
      (instr & 0x000000ff) == 0xca ||
      (instr & 0x000000ff) == 0xcb)
    return (RET);

  if ((instr & 0x000000ff) == 0xe8 ||
      (instr & 0x000000ff) == 0xff)
    return (CALL);

  if ((instr & 0x0000ffff) == 0x050f ||
      (instr & 0x0000ffff) == 0x340f ||
      (instr & 0x0000ffff) == 0x80cd)
    return (SYSCALL);
  return (0);
}

int	parser(t_config *config, int ac, char **av)
{
  config->pid = -1;
  if (ac < 1)
    return (0);
  if (strcmp(av[0], "-p") == 0)
    {
      if (ac < 2)
	return (0);
      else
	{
	  config->pid = strtol(av[1], NULL, 10);
	  if (errno == ERANGE || errno == EINVAL || config->pid == 0)
	    return (0);
	  return (1);
	}
    }
  else
    {
      config->av = av;
      config->ac = ac;
      config->command = config->av[0];
    }
  return (1);
}

void		syscall_infos(int pid, unsigned long instruction, struct user_regs_struct* registers)
{
  unsigned long			syscall_no;

  syscall_no = registers->rax;
  printf("<syscall [%ld]>\n", syscall_no);
}

void		call_relative(int pid, unsigned long instruction,
			      struct user_regs_struct* registers, t_rex* rex)
{
  unsigned long	call_addr;
  int		offset;

  offset = (int)(instruction >> 8);
  if (rex->w)
    {
      /* WUT ? ca fait la meme chose que instruction >> 8, non ??? */
      /* offset = ptrace(PTRACE_PEEKDATA, pid, registers->rip + 1); */
      call_addr = registers->rip + offset + 9;
    }
  else
    {
      call_addr = registers->rip + offset + 5;
    }
  printf("<relative call [%lx]>\n", call_addr);
}

unsigned long	get_sib_index_with_rexx(char index, struct user_regs_struct* registers)
{
  switch (index)
    {
    case 0:
      return (registers->r8);
    case 1:
      return (registers->r9);
    case 2:
      return (registers->r10);
    case 3:
      return (registers->r11);
    case 4:
      return (registers->r12);
    case 5:
      return (registers->r13);
    case 6:
      return (registers->r14);
    case 7:
      return (registers->r15);
    default:
      return (0);
    }
}

unsigned long	get_sib_index_without_rexx(char index, struct user_regs_struct* registers)
{
  switch (index)
    {
    case 0:
      return (registers->rax);
    case 1:
      return (registers->rcx);
    case 2:
      return (registers->rdx);
    case 3:
      return (registers->rbx);
    case 4:
      return (registers->rsp);
    case 5:
      return (registers->rbp);
    case 6:
      return (registers->rsi);
    case 7:
      return (registers->rdi);
    default:
      return (0);
    }
}

unsigned long	get_sib_index(char index, struct user_regs_struct* registers,
			      t_rex* rex)
{
  if (rex->x)
    return (get_sib_index_with_rexx(index, registers));
  else
    return (get_sib_index_without_rexx(index, registers));
}

unsigned long	get_sib_scale(char scale)
{
  switch (scale)
    {
    case 0:
      return (1);
    case 1:
      return (2);
    case 2:
      return (4);
    case 3:
      return (8);
    default:
      return (1);
    }
}

unsigned long	get_sib_base_with_rexb(char base, struct user_regs_struct* registers)
{
  switch (base)
    {
    case 0:
      return (registers->r8);
    case 1:
      return (registers->r9);
    case 2:
      return (registers->r10);
    case 3:
      return (registers->r11);
    case 4:
      return (registers->r12);
    case 5:
      return (registers->r13);
    case 6:
      return (registers->r14);
    case 7:
      return (registers->r15);
    default:
      return (0);
    }
}


unsigned long	get_sib_base_without_rexb(char base, struct user_regs_struct* registers)
{
  switch (base)
    {
    case 0:
      return (registers->rax);
    case 1:
      return (registers->rcx);
    case 2:
      return (registers->rdx);
    case 3:
      return (registers->rbx);
    case 4:
      return (registers->rsp);
    case 5:
      return (registers->rbp);
    case 6:
      return (registers->rsi);
    case 7:
      return (registers->rdi);
    default:
      return (0);
    }
}

unsigned long	get_sib_base(int pid, char base, struct user_regs_struct* registers,
			     t_rex* rex, char mod)
{
  if (!mod && base == 5)
    return (get_data(pid, registers->rip + 3) & 0xFFFFFFFF);
  if (rex->b)
    return (get_sib_base_with_rexb(base, registers));
  else
    return (get_sib_base_without_rexb(base, registers));
}

unsigned long	get_sib_addr(int pid, unsigned char sib_byte,
			     struct user_regs_struct* registers,
			     t_rex* rex, char mod)
{
  t_sib		sib;
  unsigned long	result = 0;

  sib.scale = sib_byte & 0xC0;
  sib.index = sib_byte & 0x38;
  sib.base = sib_byte & 0x07;

  result = get_sib_index(sib.index, registers, rex);
  result *= get_sib_scale(sib.scale);
  result += get_sib_base(pid, sib.base, registers, rex, mod);
  return (result);
}

inline unsigned long	call_rm_0xD_with_rexb(int pid, unsigned long instruction,
					      struct user_regs_struct* registers, t_rex* rex,
					      unsigned char rm)
{
  switch (rm)
    {
    case 0xD0:
      return (registers->r8);
    case 0xD1:
      return (registers->r9);
    case 0xD2:
      return (registers->r10);
    case 0xD3:
      return (registers->r11);
    case 0xD4:
      return (registers->r12);
    case 0xD5:
      return (registers->r13);
    case 0xD6:
      return (registers->r14);
    case 0xD7:
      return (registers->r15);
    default:
      return (0);
    }
}

inline unsigned long	call_rm_0xD_without_rexb(int pid, unsigned long instruction,
						 struct user_regs_struct* registers, t_rex* rex,
						 unsigned char rm)
{
  switch (rm)
  {
  case  0xD0:
    return (registers->rax);
  case  0xD1:
    return (registers->rcx);
  case  0xD2:
    return (registers->rdx);
  case  0xD3:
    return (registers->rbx);
  case  0xD4:
    return (registers->rsp);
  case  0xD5:
    return (registers->rbp);
  case  0xD6:
    return (registers->rsi);
  case  0xD7:
    return (registers->rdi);
  default:
    return (0);
  }
}

inline unsigned long	call_rm_0xD(int pid, unsigned long instruction,
				    struct user_regs_struct* registers, t_rex* rex,
				    unsigned char rm)
{
  if (rex->b)
    return (call_rm_0xD_with_rexb(pid, instruction, registers, rex, rm));
  else
    return (call_rm_0xD_without_rexb(pid, instruction, registers, rex, rm));
}

inline unsigned long	call_rm_0x1_with_rexb(int pid, unsigned long instruction,
					      struct user_regs_struct* registers, t_rex* rex,
					      unsigned char rm)
{
  switch (rm)
    {
    case 0x10:
      return (get_data(pid, registers->rax));
    case 0x11:
      return (get_data(pid, registers->rcx));
    case 0x12:
      return (get_data(pid, registers->rdx));
    case 0x13:
      return (get_data(pid, registers->rbx));
    case 0x16:
      return (get_data(pid, registers->rsi));
    case 0x17:
      return (get_data(pid, registers->rdi));
    default:
      return (0);
    }
}

inline	unsigned long	call_rm_0x1_without_rexb(int pid, unsigned long instruction,
						 struct user_regs_struct* registers, t_rex* rex,
						 unsigned char rm)
{
  switch (rm)
    {
    case 0x10:
      return (get_data(pid, registers->r8));
    case 0x11:
      return (get_data(pid, registers->r9));
    case 0x12:
      return (get_data(pid, registers->r10));
    case 0x13:
      return (get_data(pid, registers->r11));
    case 0x16:
      return (get_data(pid, registers->r14));
    case 0x17:
      return (get_data(pid, registers->r15));
    default:
      return (0);
    }
}

inline unsigned long	call_rm_0x1(int pid, unsigned long instruction,
				    struct user_regs_struct* registers, t_rex* rex,
				    unsigned char rm, int* offset_rip)
{
  unsigned long		addb;

  if (rm == 0x14)
    {
      /*
      ** YES, WHEN YOU THINK YOU HAVE ALMOST FINISHED SIB IS HERE !!!!
      */
      *offset_rip = 3;
      if ((((instruction & 0xFF0000) >> 16) & 0x07) == 5)
	*offset_rip = 7;
      return (get_data(pid, get_sib_addr(pid, (instruction & 0xFF0000) >> 16, registers, rex, 0)));
    }
  else if (rm == 0x15)
    {
      addb = get_data(pid, registers->rip + 2) & 0xFFFFFFFF;
      /* printf("----------- > addb = %lx\n", addb); */
      *offset_rip = 6;
      return (get_data(pid, registers->rip + 6 + addb));
    }
  else if (rex->b)
    return (call_rm_0x1_with_rexb(pid, instruction, registers, rex, rm));
  else
    return (call_rm_0x1_without_rexb(pid, instruction, registers, rex, rm));
}

inline unsigned long	call_rm_0x5_with_rexb(int pid, unsigned long instruction,
					      struct user_regs_struct* registers, t_rex* rex,
					      unsigned char rm, char addb)
{
  switch (rm)
    {
    case 0x50:
      return (get_data(pid, registers->r8 + addb));
    case 0x51:
      return (get_data(pid, registers->r9 + addb));
    case 0x52:
      return (get_data(pid, registers->r10 + addb));
    case 0x53:
      return (get_data(pid, registers->r11 + addb));
    case 0x55:
      return (get_data(pid, registers->r13 + addb));
    case 0x56:
      return (get_data(pid, registers->r14 + addb));
    case 0x57:
      return (get_data(pid, registers->r15 + addb));
    default:
      return (0);
    }
}

inline unsigned long	call_rm_0x5_without_rexb(int pid, unsigned long instruction,
						 struct user_regs_struct* registers, t_rex* rex,
						 unsigned char rm, char addb)
{
  switch (rm)
    {
    case 0x50:
      return (get_data(pid, registers->rax + addb));
    case 0x51:
      return (get_data(pid, registers->rcx + addb));
    case 0x52:
      return (get_data(pid, registers->rdx + addb));
    case 0x53:
      return (get_data(pid, registers->rbx + addb));
    case 0x55:
      return (get_data(pid, registers->rbp + addb));
    case 0x56:
      return (get_data(pid, registers->rsi + addb));
    case 0x57:
      return (get_data(pid, registers->rdi + addb));
    default:
      return (0);
    }
}


inline unsigned long	call_rm_0x5(int pid, unsigned long instruction,
				    struct user_regs_struct* registers, t_rex* rex,
				    unsigned char rm, int* offset_rip)
{
  char			addb;

  *offset_rip = 3;
  addb = (instruction & 0xFF0000) >> 16;
  if (rm == 0x54)
    {
      *offset_rip = 4;
      return (get_data(pid, get_sib_addr(pid, (instruction & 0xFF0000) >> 16, registers, rex, 1) + addb));
    }
  if (rex->b)
    return (call_rm_0x5_with_rexb(pid, instruction, registers, rex, rm, addb));
  else
    return (call_rm_0x5_without_rexb(pid, instruction, registers, rex, rm, addb));
}

inline unsigned long	call_rm_0x9_with_rexb(int pid, unsigned long instruction,
					      struct user_regs_struct* registers, t_rex* rex,
					      unsigned char rm, unsigned long addb)
{
  switch (rm)
    {
    case 0x90:
      return (get_data(pid, registers->r8 + addb));
    case 0x91:
      return (get_data(pid, registers->r9 + addb));
    case 0x92:
      return (get_data(pid, registers->r10 + addb));
    case 0x93:
      return (get_data(pid, registers->r11 + addb));
    case 0x95:
      return (get_data(pid, registers->r13 + addb));
    case 0x96:
      return (get_data(pid, registers->r14 + addb));
    case 0x97:
      return (get_data(pid, registers->r15 + addb));
    default:
      return (0);
    }
}

inline unsigned long	call_rm_0x9_without_rexb(int pid, unsigned long instruction,
						 struct user_regs_struct* registers, t_rex* rex,
						 unsigned char rm, unsigned long addb)
{
  switch (rm)
    {
    case 0x90:
      return (get_data(pid, registers->rax + addb));
    case 0x91:
      return (get_data(pid, registers->rcx + addb));
    case 0x92:
      return (get_data(pid, registers->rdx + addb));
    case 0x93:
      return (get_data(pid, registers->rbx + addb));
    case 0x95:
      return (get_data(pid, registers->rbp + addb));
    case 0x96:
      return (get_data(pid, registers->rsi + addb));
    case 0x97:
      return (get_data(pid, registers->rdi + addb));
    default:
      return (0);
    }
}

inline unsigned long	call_rm_0x9(int pid, unsigned long instruction,
				    struct user_regs_struct* registers, t_rex* rex,
				    unsigned char rm, int* offset_rip)
{
  unsigned long		addb;

  addb = get_data(pid, registers->rip + 2) & 0xFFFFFFFF;
  *offset_rip = 6;
  if (rm == 0x94)
    {
      *offset_rip = 7;
      return (get_data(pid, get_sib_addr(pid, (instruction & 0xFF0000) >> 16, registers, rex, 2) + addb));
    }
  else if (rex->b)
    return (call_rm_0x9_with_rexb(pid, instruction, registers, rex, rm, addb));
  else
    return (call_rm_0x9_without_rexb(pid, instruction, registers, rex, rm, addb));
}

void		call_absolute(int pid, unsigned long instruction,
			      struct user_regs_struct* registers, t_rex* rex)
{
  unsigned char	rm;
  unsigned long	addr;
  int		offset_rip;

  rm = (instruction & 0xFF00) >> 8;
  offset_rip = 2;

  if (rm >= 0xD0 && rm <= 0xD7) /* register direct */
    addr = call_rm_0xD(pid, instruction, registers, rex, rm);
  else if (rm >= 0x10 && rm <= 0x17) /* register indirect */
    addr = call_rm_0x1(pid, instruction, registers, rex, rm, &offset_rip);
  else if (rm >= 0x50 && rm <= 0x57) /* one-byte signed displacment */
    addr = call_rm_0x5(pid, instruction, registers, rex, rm, &offset_rip);
  else if (rm >= 0x90 && rm <= 0x97) /* four-byte signed displacement */
    addr = call_rm_0x9(pid, instruction, registers, rex, rm, &offset_rip);
  printf("<absolute call [%lx]>  {%lx}\n", addr, instruction);
}

void		call_infos(int pid, unsigned long instruction, struct user_regs_struct* registers)
{
  t_rex		rex = {0, 0, 0, 0};

  /*
  ** detect rex prefix
  */
  if ((instruction & 0xF0) == 0x40)
    {
      rex.w = instruction & 0x4;
      rex.r = instruction & 0x3;
      rex.x = instruction & 0x2;
      rex.b = instruction & 0x1;
      registers->rip++;
      instruction = ptrace(PTRACE_PEEKDATA, pid, (void*)registers->rip, 0);
      if (instruction == -1)
	return;
    }

  /*
  ** detect call type
  */
  if ((instruction & 0x000000ff) == 0xe8)
    call_relative(pid, instruction, registers, &rex);
  if ((instruction & 0x000000ff) == 0xff && (instruction & 0x3800) == 0x1000)
    call_absolute(pid, instruction, registers, &rex);
}

void		ret_infos(int pid, unsigned long instruction, struct user_regs_struct* registers)
{
  printf("<ret>\n");
}

void		step_infos(int pid)
{
  struct user_regs_struct	registers;
  unsigned long			instruction;
  int				instruction_type;

  if (!get_registers(pid, &registers))
    return;
  instruction = get_instruction(pid, &registers);
  if (!instruction)
    return;
  instruction_type = get_instruction_type(instruction);
  if (!instruction_type)
    return;
  switch (instruction_type)
    {
    case SYSCALL:
      syscall_infos(pid, instruction, &registers);
      break;

    case CALL:
      call_infos(pid, instruction, &registers);
      break;

    case RET:
      ret_infos(pid, instruction, &registers);
      break;
    }
}

int		trace_pid(int pid)
{
  int		status;

  while (1)
    {
      waitpid(pid, &status, 0);
      if (WIFEXITED(status) ||
	  WIFSIGNALED(status))
	break;
      step_infos(pid);
      if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) == -1)
	{
	  perror("error: can't trace process");
	  continue;
	}
    }
  return (1);
}

int		launch_command(char* command, char** av)
{
  int	pid;

  if ((pid = fork()) == -1)
    return (-1);
  if (!pid)
    {
      ptrace(PTRACE_TRACEME, 0, 0, 0);
      execvp(command, av);
      exit(EXIT_FAILURE);
    }
  return (pid);
}

int		ftrace(t_config* config)
{
  int	pid;

  if (config->pid != -1)
    {
      if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1)
	{
	  perror("error: can't attach process");
	  return (0);
	}
      pid = config->pid;
    }
  else
    pid = launch_command(config->command, config->av);
  if (pid == -1)
    return (0);
  return (trace_pid(pid));
  /* return (0); */
}

void		usage(char* command)
{
  printf("usage: %s [-p pid] [command]\n", command);
}

int		main(int ac, char** av)
{
  t_config	config;

  if (!parser(&config, ac - 1, av + 1))
    usage(av[0]);
  else if (ftrace(&config))
    return (EXIT_FAILURE);
  return (EXIT_SUCCESS);
}
