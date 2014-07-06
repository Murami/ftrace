#include <stdlib.h>
#include <stdio.h>
#include "ftrace.h"

void			a()
{
}

static inline void	usage(char* command)
{
  printf("usage: %s [-p pid] [command]\n", command);
  a();
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
