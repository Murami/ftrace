##
## Makefile for bomberman in /home/vaur/epitech/inprogress/B-PAV-330/cpp_bomberman
## 
## Made by vaur
## Login   <vaur@epitech.net>
## 
## Started on  Sun Jun 15 19:49:39 2014 vaur
## Last update Sun Jul  6 21:52:01 2014 vaur
##

########################################################################
##				User's  Variable
########################################################################

##################
## 	Src

NAME =			ftrace

SRC=			call.c			\
			call_rm_0x1.c		\
			call_rm_0x5.c		\
			call_rm_0x9.c		\
			call_rm_0xD.c		\
			list.c			\
			main.c			\
			parse.c			\
			parse_elf.c		\
			parse_elf_ldconf.c	\
			parse_map.c		\
			ret.c			\
			sib.c			\
			syscall.c		\
			syscallent.c		\
			trace.c			\
			trace_get_infos.c	\
			write_syscall.c		\
			xmalloc.c

##################
## 	Author

AUTH=			guerot_a	\
			devill_x	\

##################
## 	Makeflags

DEBUG=			yes
export DEBUG

INFO=			NO

##################
## 	Flags

CFLAGS +=		-Iinclude			\
			`pkg-config libelf --cflags`

LDFLAGS +=		`pkg-config libelf --libs`

##################
## Directories

D_SRC =			./src/
D_OBJ = 		./obj/
D_NAME =		./

##################
## 	lib

LDIR=			NONE

##################
## 	Compilator

CC=			gcc

#################
##	makefile's core

include			.libmake.mk
