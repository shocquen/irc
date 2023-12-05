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
	@$(CC) $(CFLAGS) -I$(INC) -c $1 -o $2
	@echo "$(shell tput bold)$(shell tput setaf 2)√$(shell tput sgr0)"
endef

define linking
	@echo "$(shell tput bold)$(shell tput setaf 2)Linking $1 $(shell tput sgr0)\c"
	@$(CC) $(CFLAGS) -I$(INC) $(addprefix obj/, $(OBJS)) -o $(NAME)
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
Server.cpp \
Utils.cpp \


OBJS		= $(SRCS:.cpp=.o)
DEPS		= $(addprefix obj/, $(OBJS:.o=.d))

INC		= ./includes

NAME		= ft_irc
RM			= rm -rf
CC			= c++
AR			= ar -rcs
CFLAGS		= -Wall -Wextra -Werror -g3 -std=c++98 


${NAME}:	$(addprefix obj/, $(OBJS))
			$(call linking,$(NAME))

obj/%.o:	srcs/%.cpp
			@mkdir -p $(@D)
			$(call compiling,$<,$@,0)

all:	 $(NAME)

clean:	
			$(call removing, obj)

fclean:		clean
			$(call removing, $(NAME))

re:		fclean 
			@make all

signe:
			$(call sign)
			@echo

-include $(DEPS)
.PHONY : 	all clean fclean re
