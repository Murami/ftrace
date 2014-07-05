##
## Makefile for bomberman in /home/vaur/epitech/inprogress/B-PAV-330/cpp_bomberman
## 
## Made by vaur
## Login   <vaur@epitech.net>
## 
## Started on  Sun Jun 15 19:49:39 2014 vaur
## Last update Sat Jul  5 19:42:46 2014 vaur
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
			main.c			\
			parse.c			\
			parse_elf.c		\
			ret.c			\
			sib.c			\
			syscall.c		\
			trace.c			\
			trace_get_infos.c	\
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

CFLAGS +=		-Iinclude
LDFLAGS +=		-lelf -L./libelf

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
