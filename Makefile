# **************************************************************************** #;
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: shocquen <shocquen@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/11/23 09:28:22 by shocquen          #+#    #+#              #
#    Updated: 2023/10/02 17:16:10 by shocquen         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

define sign
	@clear
	@tput setaf 1
	@tput bold

	@sleep .1
	@echo "$(shell tput cup 0 0)..%%%%...%%..%%...%%%%....%%%%....%%%%%...%%%%...%%%%%%..%%..%%."	
	@sleep .2

	@echo "$(shell tput cup 0 0).....%%..%%..%%..%%..%%..%%..%%..%%..%%..%%..%%..%%......%%..%%."
	@echo "$(shell tput cup 1 0)..%%%%...%%..%%...%%%%....%%%%....%%%%%...%%%%...%%%%%%..%%..%%."	
	@sleep .15

	@echo "$(shell tput cup 0 0)..%%%%...%%%%%%..%%..%%..%%......%%.%%%..%%..%%..%%%%....%%.%%%."
	@echo "$(shell tput cup 1 0).....%%..%%..%%..%%..%%..%%..%%..%%..%%..%%..%%..%%......%%..%%."
	@echo "$(shell tput cup 2 0)..%%%%...%%..%%...%%%%....%%%%....%%%%%...%%%%...%%%%%%..%%..%%."	
	@sleep .2

	@echo "$(shell tput cup 0 0).%%......%%..%%..%%..%%..%%..%%..%%..%%..%%..%%..%%......%%%.%%."
	@echo "$(shell tput cup 1 0)..%%%%...%%%%%%..%%..%%..%%......%%.%%%..%%..%%..%%%%....%%.%%%."
	@echo "$(shell tput cup 2 0).....%%..%%..%%..%%..%%..%%..%%..%%..%%..%%..%%..%%......%%..%%."
	@echo "$(shell tput cup 3 0)..%%%%...%%..%%...%%%%....%%%%....%%%%%...%%%%...%%%%%%..%%..%%."	
	@sleep .2

	@echo "$(shell tput cup 0 0)..%%%%...%%..%%...%%%%....%%%%....%%%%...%%..%%..%%%%%%..%%..%%."
	@echo "$(shell tput cup 1 0).%%......%%..%%..%%..%%..%%..%%..%%..%%..%%..%%..%%......%%%.%%."
	@echo "$(shell tput cup 2 0)..%%%%...%%%%%%..%%..%%..%%......%%.%%%..%%..%%..%%%%....%%.%%%."
	@echo "$(shell tput cup 3 0).....%%..%%..%%..%%..%%..%%..%%..%%..%%..%%..%%..%%......%%..%%."
	@echo "$(shell tput cup 4 0)..%%%%...%%..%%...%%%%....%%%%....%%%%%...%%%%...%%%%%%..%%..%%."	
	@sleep .1

	@tput sgr0
endef

# Create a library
define creating
	@echo "$(shell tput bold)$(shell tput setaf 2)Create $1 $(shell tput sgr0)\c"
	@$(AR) $(NAME) $(addprefix obj/, $(OBJS))
	@echo "$(shell tput bold)$(shell tput setaf 2)√$(shell tput sgr0)"
endef

# Call another Makefile
define making
	@echo "$(shell tput setaf 6)Make $1 $(shell tput sgr0)\c"
	@make -C $1 > /dev/null
	@echo "$(shell tput bold)$(shell tput setaf 2)√$(shell tput sgr0)"
endef

define compiling
	@echo "$(shell tput bold)$(shell tput setaf 3)Compiling $1 $(shell tput sgr0)\c"
	@$(CC) $(CFLAGS) -I$(INC) -MMD -MF $(@:.o=.d) -c $1 -o $2
	@echo "$(shell tput bold)$(shell tput setaf 2)√$(shell tput sgr0)"
endef

define linking
	@echo "$(shell tput bold)$(shell tput setaf 2)Linking $1 $(shell tput sgr0)\c"
	@$(CC) $(CFLAGS) -I$(INC) $2 -o $1
	@echo "$(shell tput bold)$(shell tput setaf 2)√$(shell tput sgr0)"
endef

define cleaning
	@echo "$(shell tput bold)$(shell tput setaf 8)Cleaning $1 $(shell tput sgr0)\c" 
	@make $2 -sC $1 > /dev/null
	@echo "$(shell tput bold)$(shell tput setaf 2)√$(shell tput sgr0)"
endef

define removing
	@echo "$(shell tput bold)$(shell tput setaf 8)Removing $1 $(shell tput sgr0)\c"
	@$(RM) $1  > /dev/null 
	@echo "$(shell tput bold)$(shell tput setaf 2)√$(shell tput sgr0)"
endef

SRCS		= main.cpp \
Client.cpp \
Utils.cpp \
Cmd.cpp \
ip.cpp \
Server/cmdHandlers.cpp \
Server/Execption.cpp \
Server/Server.cpp \
Server/methods.cpp \
Channel/Channel.cpp \
Channel/methods.cpp \

SRCS_BOT = main.cpp


OBJS		= $(SRCS:.cpp=.o)
DEPS		= $(addprefix obj/, $(OBJS:.o=.d))
OBJS_BOT		= $(SRCS_BOT:.cpp=.o)
DEPS_BOT		= $(addprefix obj_bot/, $(OBJS_BOT:.o=.d))

INC		= ./includes

NAME		= ircserv
BOT_NAME	= Astarion
RM			= rm -rf
CC			= c++
AR			= ar -rcs
CFLAGS		= -Wall -Wextra -Werror -g3 -std=c++98  
# CFLAGS		+= -fsanitize=address


${NAME}:	$(addprefix obj/, $(OBJS))
			$(call linking,$(NAME), $(addprefix obj/, $(OBJS)))
-include $(DEPS);

obj/%.o:	srcs/%.cpp
			@mkdir -p $(@D)
			$(call compiling,$<,$@,0)

${BOT_NAME}:	$(addprefix obj_bot/, $(OBJS_BOT))
			$(call linking,$(BOT_NAME) $(addprefix obj_bot/, $(OBJS_BOT)))

obj_bot/%.o:	aBot/%.cpp
			@mkdir -p $(@D)
			$(call compiling,$<,$@,0)

all:	 $(NAME) $(BOT_NAME)

clean:	
			$(call removing, obj)

fclean:		clean
			$(call removing, $(NAME))
			$(call removing, $(BOT_NAME))

re:		fclean 
			@make all

signe:
			$(call sign)
			@echo

-include $(DEPS)
.PHONY : 	all clean fclean re
