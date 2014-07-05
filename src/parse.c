#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "ftrace.h"

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
