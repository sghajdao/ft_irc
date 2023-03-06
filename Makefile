

NAME_P := server

CC = c++

CFLAGS = 

SRC = main.cpp server.cpp User.cpp

OBJECTS = $(SRC:.cpp=.o)

all: $(NAME_P)

$(NAME_P): $(OBJECTS)
	@echo "\033[0;32m\n\nCompiling server..."
	@$(CC) -o $(NAME_P) $(OBJECTS)
	@echo "\n\033[0;32mDone."

%.o:%.cpp server.hpp User.hpp
	@printf "\033[0;33mGenerating server objects... %-10.10s\r" $@
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "\033[0;31m\nDeleting objects..."
	@rm -f $(OBJECTS) $(OBJECTSB)
	@echo "\n\033[0;31mDeleted.\n"	

fclean: clean
	@rm -f $(NAME_P)

re: fclean all