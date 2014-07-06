#include	<unistd.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<sys/ptrace.h>
#include	<sys/user.h>
#include	<sys/types.h>
#include	<sys/wait.h>
#include	"ftrace.h"
#include	"parse_map.h"

static inline int	launch_command(char* command, char** av)
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
  /* ptrace(PTRACE_SYSCALL, pid, 0, 0); */
  /* ptrace(PTRACE_SINGLESTEP, pid, 0, 0); */
  /* usleep(1); */
  return (pid);
}

static inline void		step_infos(int pid, t_data *data)
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
      syscall_infos(pid, instruction, &registers, data);
      break;

    case CALL:
      call_infos(pid, instruction, &registers, data);
      break;

    case RET:
      ret_infos(pid, instruction, &registers, data);
      break;
    }
}

int			init_data(int pid, t_data *data)
{
  if ((data->file = open("outfile", O_WRONLY | O_CREAT | O_TRUNC,
			 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1)
    return (-1);
  if ((data->call_stack = list_new()) == NULL)
    return (-1);
  if ((data->link_list = list_new()) == NULL)
    return (-1);
  if ((data->sym_list = parse_elf(pid)) == NULL)
    return (-1);
  if ((data->map_info = parse_map(pid)) == NULL)
    return (-1);
  write(data->file, "digraph graphname {\n", 20);
  return (1);
}

static inline int	trace_pid(int pid, t_data* data)
{
  int		status;

  while (1)
    {
      waitpid(pid, &status, 0);
      if (WIFEXITED(status) ||
	  WIFSIGNALED(status))
	{
	  write(data->file, "}\n", 2);
	  break;
	}
      step_infos(pid, data);
      if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) == -1)
	{
	  perror("error: can't trace process");
	  continue;
	}
    }
  return (1);
}

/*
** detacher le process !!!!!
*/

int		ftrace(t_config* config)
{
  int		pid;
  t_data	data;

  if (config->pid != -1)
    {
      pid = config->pid;
      if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1)
	{
	  perror("error: can't attach process");
	  return (0);
	}
    }
  else
    pid = launch_command(config->command, config->av);
  if (pid == -1)
    return (0);
  if (init_data(pid, &data) == -1)
    {
      perror("error: can't init data struct");
      return (0);
    }
  return (trace_pid(pid, &data));
}
