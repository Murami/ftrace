##
## target.mk for raytracer in /home/vaur/epitech/inprogress/B-VPP-042/vpp_raytracer
##
## Made by vaur
## Login   <vaur@epitech.net>
##
## Started on  Mon Jun  9 18:04:08 2014 vaur
## Last update Sat Jul  5 16:28:03 2014 vaur
##

############################
## 	commande

AR=			ar rc
RM=			rm -f

############################
## 	Flags

ifeq ($(DEBUG),yes)
CFLAGS +=		-ggdb -g3
CFLAGS +=		-DDEBUG_=1
else
CFLAGS +=		-o3
CFLAGS +=		-DDEBUG_=0
CFLAGS +=		-Werror
CFLAGS +=		-pg
endif

CFLAGS += 		-Wextra -Wall

ifeq ($(DEBUG),yes)
LDFLAGS +=		-pg
endif


############################
## 	Info

ifeq ($(INFO),yes)
COM=			$(shell git log -n1 --format="%s")
VER=			$(shell git log -n1 --format="%h")
AUT=			$(shell git log -n1 --format="%cN")
endif


############################
##	SubDirectories SRC

SD_SRC=			$(shell cd $(D_SRC) && find . -type d | uniq)

############################
##	Compilation rules

ifeq ($(CC), g++)
OBJ =			$(SRC:.cpp=.o)
else
OBJ =			$(SRC:.c=.o)
endif

############################
##	Paths

CSRC =			$(addprefix $(D_SRC),$(SRC))
COBJ =			$(addprefix $(D_OBJ),$(OBJ))
CNAME =			$(addprefix $(D_NAME),$(NAME))

##################
## 	display

ifeq ($(DEBUG),yes)
C_IN=			\x1b[33m\033[1m
else
C_IN=			\x1b[32m\033[1m
endif
C_GRE=			\x1b[32m\033[1m
C_RED=			\e[0;31m\033[1m
C_YEL=			\x1b[33m\033[1m
C_OUT=			\033[0m
D_IN=			$(C_IN)==\t
D_OUT=			\t==$(C_OUT)

CO_P=			[$(C_YEL)...$(C_OUT)]
CO_O=			[$(C_GRE)OK$(C_OUT)]
CO_E=			[$(C_RED)Error$(C_OUT)]
E=			/bin/echo -e
SEP=			----------------------------------

########################################################################
##				Target
########################################################################

############################
##	compilation

all:			disp_init buildrepo lib $(NAME)
ifeq ($(INFO),yes)
			@$(E)  "$(D_IN)Commit by $(AUT)$(D_OUT)"
			@$(E)  "$(SEP)"
			@$(E)  "$(C_IN)- msg: $(C_OUT)$(COM)"
			@$(E)  "$(SEP)"
endif
ifeq ($(DEBUG),yes)
			@$(E)  "$(D_IN)DEBUG MODE\t$(D_OUT)"
endif
			@$(E)	"$(D_IN)$(NAME)\t$(D_OUT)"
			@$(E)	"$(D_IN)ready for use\t$(D_OUT)"
			@$(E)	"$(SEP)"


##################
## 	Binary

$(NAME):		$(COBJ)
			@$(E)  "$(D_IN)Compilation over$(D_OUT)"
			@rm -f error.log
			@$(E)  "$(D_IN)Linkage in progress$(D_OUT)"
			@$(CC) $(COBJ) -o $(CNAME) $(LDFLAGS)
			@$(E)  "$(D_IN)Linkage Over\t$(D_OUT)"

##################
## 	create Object

$(D_OBJ)%.o:		$(D_SRC)%.c
			@$(E) -n "$(C_IN)-$(C_OUT)$(CO_P)   Compiling $(C_IN)`echo $<| sed 's/src/ /g' | tr '/' '\t' `$(C_OUT)"
			@$(CC) $(CFLAGS) -c $< -o $@ 2> error.log && $(E) "\r$(C_IN)-$(C_OUT)$(CO_O)    Compiling $(C_IN)`echo $<| sed 's/src/ /g' | tr '/' '\t' `$(C_OUT)" || ($(E) "\r$(C_IN)-$(C_OUT)$(CO_E) Compiling $(C_IN)`echo $<| sed 's/src/ /g' | tr '/' '\t' `$(C_OUT)" && cat error.log && false)
			@cat error.log


$(D_OBJ)%.o:		$(D_SRC)%.cpp
			@$(E) -n "$(C_IN)-$(C_OUT)$(CO_P)   Compiling $(C_IN)`echo $<| sed 's/src/ /g' | tr '/' '\t' `$(C_OUT)"
			@$(CC) $(CFLAGS) -c $< -o $@ 2> error.log && $(E) "\r$(C_IN)-$(C_OUT)$(CO_O)    Compiling $(C_IN)`echo $<| sed 's/src/ /g' | tr '/' '\t' `$(C_OUT)" || ($(E) "\r$(C_IN)-$(C_OUT)$(CO_E) Compiling $(C_IN)`echo $<| sed 's/src/ /g' | tr '/' '\t' `$(C_OUT)" && cat error.log && false)
			@cat error.log

##################
## 	remove Object

clean:
			@$(E)  "$(D_IN)Cleaning .o\t$(D_OUT)"
			@$(RM) $(COBJ)
			@$(RM) -r $(D_OBJ)
			@$(RM) gmon.out
			@$(E)  "$(SEP)"

##################
## 	remove Binary

fclean:			clean libfclean
			@$(E)  "$(D_IN)Deleting $(NAME)$(D_OUT)"
			@$(RM) $(CNAME)
			@find . -name '*~' -delete
			@find . -name '#*#' -delete
			@$(RM) -r doc/html/
			@$(RM) -r doc/latex/
			@$(E)  "$(SEP)"

##################
##	remove and recompile

re:			fclean all

##################
##	LIB

lib:
ifneq ($(LDIR),NONE)
			@$(E)  "$(D_IN)Building library$(D_OUT)"
			@make -C $(LDIR)
endif

libfclean:
ifneq ($(LDIR),NONE)
			@$(E)  "$(D_IN)Cleaning library$(D_OUT)"
			@make -C $(LDIR) fclean
endif

############################################
## 	utilities

##################
##	display

disp_init:
			@$(E)	"$(SEP)"
			@$(E)	"$(D_IN)$(NAME)\t$(D_OUT)"
			@$(E)	"$(D_IN)made by : \t$(D_OUT)"
			@$(E)	"$(C_IN)-$(C_OUT) $(AUTH)"
ifeq ($(INFO),yes)
			@$(E)	"$(D_IN)Version: $(VER)$(D_OUT)"
endif

##################
##  switch between modes

debug:
			@$(E) "$(D_IN)Switching from\t$(D_OUT)"
ifeq ($(DEBUG),yes)
			@$(E) "$(D_IN)Debug Mode\t$(D_OUT)"
			@sed -i -e '/(sed)/!s/DEBUG=			yes/DEBUG=			no/' Makefile >> Makefile
			@$(E) "$(D_IN)To: Normal Mode\t$(D_OUT)"
else
			@$(E) "$(D_IN) Normal Mode\t$(D_OUT)"
			@sed -i -e '/(sed)/!s/DEBUG=			no/DEBUG=			yes/' Makefile >> Makefile
			@$(E) "$(D_IN)To: Debug Mode\t$(D_OUT)"
endif
			@make --no-print-directory re

##################
##	build directories
##	for obj

buildrepo:
			@$(E)  "$(D_IN)Checking repository$(D_OUT)"
			@mkdir -p $(D_OBJ)
			@for dir in $(SD_SRC); \
			do \
			if [ ! -d "$(D_OBJ)$$dir" ];then \
				mkdir -p $(D_OBJ)$$dir; \
			fi \
			done

##################
##	commit
git:			fclean
			@$(E) "$(D_IN)Sending files\t$(D_OUT)"
			@$(E) "$(C_IN)-$(C_OUT) commit msg$(C_IN): $(C_OUT)"
			read msg && git commit -a -m "$$msg"


##################
##	Documentation

doc:
			@$(E) "$(D_IN)Creating Documentation$(D_OUT)"
			@doxygen doc/Doxyfile
			@$(E) "$(D_IN)Documentation Generated$(D_OUT)"

##################
## 	.PHONY

.PHONY:			re clean fclean all fclean buildrepo disp_init git debug lib libclean libfclean doc
