# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cleticia <cleticia@student.42sp.org.br>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/03 13:28:38 by cleticia          #+#    #+#              #
#    Updated: 2023/08/24 17:45:41 by cleticia         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= webserv

CC		= c++

CFLAGS	= -Wall -Wextra -Werror -g -std=c++98  

SRC		= src/main.cpp\
		src/ConfigParser.cpp\
		src/WebServ.cpp\
		src/SocketS.cpp\

INC		= -I ./inc/ 

RM		= rm -rf

OBJ		= ${SRC:.cpp=.o}

all: $(NAME)

$(NAME): ${OBJ}
	@$(CC) $(CFLAGS) $(SRC) $(INC) -o $@
	@echo "\033[1;36m[SUCCESS] Compilation completed!\033[0m"
	@echo "To run the program, type: \033[1;36m./$(NAME)\033[0m"

.cpp.o:
	@$(CC) -g $(CFLAGS) $(INC) -c $< -o $@
	@echo "Compiling \033[1;36m./$<\033[0m"	

clean:
	@$(RM) ${OBJ}
	@echo "\33[1;93m[SUCCESS] Objects removed!\33[0m"

fclean: clean
	@$(RM) $(NAME) ${OBJ}
	@echo "\033[1;93m[SUCCESS] Full clean done!\33[0m"

re: fclean all

.PHONY: all clean fclean re