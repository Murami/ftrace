#include <stdio.h> /* DBG */
#include <sys/ptrace.h>
#include <sys/user.h>
#include "ftrace.h"

long int			*long_int_alloc(long int addr)
{
  long int *ret;

  ret = malloc(sizeof(long int));
  *ret = addr;
  return (ret);
}

int				link_exist(unsigned long parent, unsigned long son, t_data *data)
{
  t_list_iterator it;


  it = list_begin(data->link_list);
  while (it != list_end(data->link_list))
    {
      if (((t_link*)it->data)->parent == parent && ((t_link*)it->data)->son == son)
	return (1);
      it = list_iterator_next(it);
    }
    return (0);
}

void				write_file(unsigned long call_addr, t_data *data)
{
  char		string[42];
  t_link	*link;

  if (!list_empty(data->call_stack))
    {
      if (!link_exist(*((long*)(list_begin(data->call_stack)->data)), call_addr, data))
	{
	  link = malloc(sizeof(t_link));
	  link->parent = *((long*)(list_begin(data->call_stack)->data));
	  link->son = call_addr;
	  list_push_front(data->link_list, link);
	  sprintf(string, "\"%lx\"", *((long*)(list_begin(data->call_stack)->data)));
	  write(data->file, string, strlen(string));
	  write(data->file, " -> ", 4);
	  if (call_addr < 350)
	    write(data->file, "syscall", 7);
	  else
	    {
	      sprintf(string, "\"%lx\"", call_addr);
	      write(data->file, string, strlen(string));
	    }
	  write(data->file, "\n", strlen("\n"));
	}
    }
}

static inline void		call_relative(int pid, unsigned long instruction,
					      struct user_regs_struct* registers,
					      t_rex* rex, t_data *data)
{
  unsigned long	call_addr;
  int		offset;

  _(pid);
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
  write_file(call_addr, data);
  list_push_front(data->call_stack, long_int_alloc(call_addr));
  printf("<relative call [%lx]> {%lx}\n", call_addr, instruction);
}

static inline void		call_absolute(int pid, unsigned long instruction,
					      struct user_regs_struct* registers,
					      t_rex* rex, t_data *data)
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

  write_file(addr, data);
  list_push_front(data->call_stack, long_int_alloc(addr));
  printf("<absolute call [%lx]>  {%lx}\n", addr, instruction);
}

void		call_infos(int pid, unsigned long instruction,
			   struct user_regs_struct* registers, t_data *data)
{
  t_rex		rex = {0, 0, 0, 0};

  if ((instruction & 0xF0) == 0x40)
    {
      rex.w = instruction & 0x4;
      rex.r = instruction & 0x3;
      rex.x = instruction & 0x2;
      rex.b = instruction & 0x1;
      registers->rip++;
      instruction = get_data(pid, registers->rip);
      if (instruction == 0)
	return;
    }
  if ((instruction & 0x000000ff) == 0xe8)
    call_relative(pid, instruction, registers, &rex, data);
  if ((instruction & 0x000000ff) == 0xff && (instruction & 0x3800) == 0x1000)
    call_absolute(pid, instruction, registers, &rex, data);
}
