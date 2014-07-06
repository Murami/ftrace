#include	<unistd.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<sys/ptrace.h>
#include	<sys/user.h>
#include	<sys/types.h>
#include	<sys/wait.h>
#include	"ftrace.h"
#include	"parse_elf.h"

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
  return (pid);
}

static inline void		step_infos(int pid)
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

static inline int	trace_pid(int pid)
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

/*
** detacher le process !!!!!
*/

int		ftrace(t_config* config)
{
  int	pid;

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

  parse_elf(pid);

  /*
  ** INITIALISATION DE L'ARBRE
  */
  /* return (trace_pid(pid)); */
  return (0);
}
