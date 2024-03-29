#include <stdio.h> /* DBG */
#include <sys/ptrace.h>
#include <sys/user.h>
#include <unistd.h>
#include <string.h>
#include "ftrace.h"
#include "parse_map.h"
#include "map_entry.h"
#include "write_syscall.h"
#include "xfcts.h"

long	last_valid_call = 0;



char		*generate_name_by_addr(unsigned long addr)
{
  char		buffer[1024];

  sprintf(buffer, "0x%lx", addr);
  return (strdup(buffer));
}

char*				get_call_name(unsigned long call_addr, t_data *data)
{
  t_list_iterator		it;

  it = list_begin(data->sym_list);
  while (it != list_end(data->sym_list))
    {
      if (((t_symbol*)it->data)->addr == call_addr)
	return (((t_symbol*)it->data)->name);
      it = list_iterator_next(it);
    }
  return (generate_name_by_addr(call_addr));
}

t_stack_entry	*get_call_entry(long int addr, t_data *data)
{
  t_stack_entry	*entry;

  entry = xmalloc(sizeof(*entry));
  entry->addr = addr;
  if (strncmp("0x", get_call_name(addr, data), 2) == 0)
    entry->valid_call = false;
  else
    entry->valid_call = true;
  return (entry);
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

unsigned long		get_parent(t_data *data)
{
  t_list_iterator	it_list;
  t_stack_entry		*entry;

  it_list = list_begin(data->call_stack);
  while (it_list != list_end(data->call_stack))
    {
      entry = it_list->data;
      if (entry->valid_call == true)
	return (entry->addr);
      it_list = list_iterator_next(it_list);
    }
  return (0);
}

void				write_file(unsigned long call_addr, t_data *data)
{
  char		*name_parent;
  char		*name_son;
  t_link	*link;

  if (!list_empty(data->call_stack))
    {
      if (!link_exist(last_valid_call, call_addr, data) && get_parent(data) != 0)
	{
	  link = malloc(sizeof(t_link));
	  link->parent = get_parent(data);
	  link->son = call_addr;
	  name_parent = get_call_name(link->parent, data);
	  name_son    = get_call_name(link->son, data);
	  list_push_front(data->link_list, link);
	  write(data->file, "\"", 1);
	  write(data->file, name_parent, strlen(name_parent));
	  write(data->file, "\" -> ", 4);
	  if (call_addr < 350)
	    write_syscall(data->file, call_addr);
	  else
	    {
	      write(data->file, "\"", 1);
	      write(data->file, name_son, strlen(name_son));
	      write(data->file, "\"", 1);
	    }
	  write(data->file, "\n", strlen("\n"));
	}
    }
}

inline unsigned long	call_get_symbol_value(unsigned long call_addr, t_data* data)
{
  t_list_iterator	it;
  t_map_entry*		entry;

  it = list_begin(data->map_info);
  while (it != list_end(data->map_info))
    {
      entry = it->data;
      if (call_addr >= entry->ptr_start &&
	  call_addr <= entry->ptr_end)
	return (call_addr - entry->ptr_start + 0x400000);
      it = list_iterator_next(it);
    }
  return (call_addr);
}

static inline unsigned long		call_relative(int pid, unsigned long instruction,
						      struct user_regs_struct* registers,
						      t_rex* rex)
{
  unsigned long	call_addr;
  int		offset;

  _(pid);
  offset = (int)(instruction >> 8);
  if (rex->w)
    call_addr = registers->rip + offset + 9;
  else
    call_addr = registers->rip + offset + 5;
  return (call_addr);
}

static inline unsigned long		call_absolute(int pid, unsigned long instruction,
						      struct user_regs_struct* registers,
						      t_rex* rex)
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
  return (addr);
}

void		call_infos(int pid, unsigned long instruction,
			   struct user_regs_struct* registers, t_data *data)
{
  t_rex		rex = {0, 0, 0, 0};
  unsigned long	addr;
  char*		name;

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
    addr = call_relative(pid, instruction, registers, &rex);
  if ((instruction & 0x000000ff) == 0xff && (instruction & 0x3800) == 0x1000)
    addr = call_absolute(pid, instruction, registers, &rex);
  addr = call_get_symbol_value(addr, data);
  name = get_call_name(addr, data);
  if (name != NULL && strcmp(name, "main") == 0)
    data->map_info = parse_map(pid);
  write_file(addr, data);
  list_push_front(data->call_stack, get_call_entry(addr, data));
}
