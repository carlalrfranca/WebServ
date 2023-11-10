# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lfranca- <lfranca-@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/03 13:28:38 by cleticia          #+#    #+#              #
#    Updated: 2023/10/11 17:50:40 by lfranca-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# program name
NAME = webserv

# compiler
CC = g++

# directories
DIR_SRCS = src
DIR_OBJ = obj

# origin files
OBJS = $(patsubst $(DIR_SRCS)/%.cpp,$(DIR_OBJ)/%.o,$(SRCS))
SRCS = $(wildcard $(DIR_SRCS)/*.cpp)

# compilation options
CFLAGS = -Wall -Wextra -Werror -Wno-missing-field-initializers -Wno-missing-braces -g -std=c++98

ifeq ($(SANITIZE_A),true)
    CFLAGS += -fsanitize=address -fno-omit-frame-pointer
endif

ifeq ($(SANITIZE_L),true)
    CFLAGS += -fsanitize=leak -fno-omit-frame-pointer
endif

ifeq ($(DEBUG),true)
    CFLAGS += -g -fno-limit-debug-info -DDEBUG
endif

# headers
INC	= -I ./inc/ 

# command remove
RM	= rm -rf

all: $(NAME)

banner:
	@printf "\n"
	@printf "█▄─█▀▀▀█─▄█▄─▄▄─█▄─▄─▀█─▄▄▄▄█▄─▄▄─█▄─▄▄▀█▄─█─▄█\n"
	@printf "██─█─█─█─███─▄█▀██─▄─▀█▄▄▄▄─██─▄█▀██─▄─▄██▄▀▄██\n"
	@printf "▀▀▄▄▄▀▄▄▄▀▀▄▄▄▄▄▀▄▄▄▄▀▀▄▄▄▄▄▀▄▄▄▄▄▀▄▄▀▄▄▀▀▀▄▀▀▀\n"
	@printf "	By Lfranca- & Cleticia\n"
	@printf "\n"

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(INC) -o $@ $^
	@echo "\033[1;36m[SUCCESS] Compilation completed!\033[0m"
	@echo "To run the program, type: \033[1;36m./$(NAME) file.extension\033[0m"

$(DIR_OBJ)/%.o: $(DIR_SRCS)/%.cpp
	@mkdir -p $(DIR_OBJ)
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@
	@echo "Compiling \033[1;36m$<\033[0m"

$(PYTHON):
	sudo apt-get update -y && sudo apt-get install -y python3

$(SIEGE):
	sudo apt-get install -y siege

run: $(NAME)
	@make banner
	@sudo ./$(NAME) ./cfgs/default_config.txt

clean:
	@$(RM) $(DIR_OBJ)
	@echo "\33[1;93m[SUCCESS] Objects removed!\33[0m"

fclean: clean
	@$(RM) $(NAME)
	@echo "\033[1;93m[SUCCESS] Executable removed!\033[0m"
	@$(RM) -r $(DIR_OBJ)
	@echo "\033[1;93m[SUCCESS] Object directory removed!\033[0m"

re: fclean all

.PHONY: all clean fclean re
