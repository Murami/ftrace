typedef struct	s_config
{
  int		pid;
  char*		command;
  char**	av;
  int		ac;
}		t_config;

long				get_instruction(int pid)
{
  struct user_regs_struct	regs;
  long				instr;

  if (ptrace(PTRACE_GETREGS, pid, 0, &regs) == -1)
    return (0);
  if ((instr = ptrace(PTRACE_PEEKDATA, pid, (void*)regs.rip), 0) == -1)
    return (0);
  return (instr);
}

int				get_instruction_type(int pid)
{
  if ((instr & 0x000000ff) == 0xc2 ||
      (instr & 0x000000ff) == 0xc3 ||
      (instr & 0x000000ff) == 0xca ||
      (instr & 0x000000ff) == 0xcb)
    return (RET):

  if ((instr & 0x000000ff) == 0xe8 ||
      (instr & 0x000000ff) == 0xff)
    return (CALL);

  if ((instr & 0x0000ffff) == 0x050f ||
      (instr & 0x0000ffff) == 0x340f ||
      (instr & 0x0000ffff) == 0x80cd)
    return (SYCALL);
  return (0);
}

int	parser(t_config *config, int ac, char **av)
{
  data->pid = -1;
  if (argc < 1)
    return (0);
  if (strcmp(argv[0], "-p") == 0)
    {
      if (ac < 2)
	return (0);
      else
	{
	  config->pid = strtol(argv[1], NULL, 10);
	  if (errno == ERANGE || errno == EINVAL || data->pid == 0)
	    return (0);
	  return (1);
	}
    }
  else
    {
      data->av = av;
      data->ac = ac;
      data->command = data->av[1];
    }
  return (1);
}

void		syscall_infos(int pid, long instruction)
{
  printf("a syscall just occured\n");
}

void		call_infos(int pid, long instruction)
{
  printf("a call just occured\n");
}

void		ret_infos(int pid, long instruction)
{
  printf("a ret just occured\n");
}

void		step_infos(int pid)
{
  long		instruction;
  int		instruction_type;

  instruction = get_instruction(pid);
  instruction_type = get_instruction_type(instruction);
  switch (instruction_type)
    {
    case SYSCALL:
      syscall_infos(pid, instruction);
      break;

    case CALL:
      call_infos(pid, instruction);
      break;

    case RET:
      ret_infos(pid, instruction);
      break;

    default:
      /* WE DO NOTHING */
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
	  return (0);
	}
    }
  return (1);
}

int		ftrace(t_config* config)
{
  int	pid;

  if (config.pid != -1)
    {
      if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1)
	{
	  perror("error: can't attach process");
	  return (0);
	}
      pid = config.pid;
    }
  else
    pid = launch_command(command, av);
  if (pid == -1)
    return (0);
  return (trace_pid(pid));
}

int		main()
{
  t_config	config;

  if (!parser(&config, ac - 1, av + 1))
    usage();
  else if (ftrace(&config))
    return (EXIT_FAILURE);
  return (EXIT_SUCCESS);
}
