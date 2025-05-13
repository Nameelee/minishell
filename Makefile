NAME= minishell

##########################  OS check

OS = $(shell uname)
ifeq ($(OS), Darwin)
CC=cc
GFLAGS= -Werror -Wall -Wextra
else ifeq ($(OS), Linux)
CC=gcc
GFLAGS= -Werror -Wall -Wextra
endif

PROD=0

MEMORY_CHECK_PATH= error_managment/valgrind

SRCS_MAIN= srcs/main.c
SRCS_PARSER= $(wildcard srcs/parser/lexer/*.c) $(wildcard srcs/parser/tokenizer/*.c) $(wildcard srcs/parser/*.c)
SRCS_EXEC= $(wildcard srcs/exec/*.c)
SRCS_BUILTIN= $(wildcard srcs/builtin/*/*.c) $(wildcard srcs/builtin/*/*/*.c)
SRCS_TEST= $(wildcard test_unit/*.c)

LIBFT= libft


%.o:%.c
	$(CC) $(GFLAGS)  -g -c $< -o $@

OBJS_MAIN=$(SRCS_MAIN:%.c=%.o)
OBJS_PARSER=$(SRCS_PARSER:%.c=%.o)
OBJS_EXEC=$(SRCS_EXEC:%.c=%.o)
OBJS_BUILTIN=$(SRCS_BUILTIN:%.c=%.o)

# Test env:

OBJS_TEST=$(SRCS_TEST:%.c=%.o)

DATE=$(shell date -Iseconds)
BRANCH=$(shell git branch --show-current)
gcom=
EMPTY=


OBJS= $(OBJS_MAIN) $(OBJS_BUILTIN) $(OBJS_PARSER)  $(OBJS_EXEC)
OBJS_T= $(OBJS_TEST) $(OBJS_BUILTIN) $(OBJS_EXEC) $(OBJS_PARSER)

.PHONY: clean fclean run git testenv var lib tenv

$(NAME): $(OBJS)
	$(CC) $(GFLAGS) $(OBJS) -L$(LIBFT) -lft -o $(NAME) -lreadline
	
run: $(NAME)
ifeq ($(OS), Darwin)
	./$(NAME)
else ifeq ($(OS), Linux)
	valgrind --leak-check=full --log-file=val_report -s ./$(NAME)
endif

# cleaning rules

clean:
	rm -f $(OBJS) $(OBJS_TEST)
	rm -f test_unit/test_unit

fclean: clean
	rm -f $(NAME) test_unit/test
	rm -f valg_test 

mclean:
	rm -f $(MEMORY_CHECK_PATH)/*

tenv: $(OBJS_T)
# check if the PROD variable env value. The PROD variable define the path file for compilation especialy for the main().
ifeq ($(PROD), 0)
	@echo "\033[44m *** Start $(NAME) in test env \033[0m"
ifeq ($(NOFLAGS), 1)
	@echo "\033[41m *** NO FLAGS! \033[0m\n"
endif
ifeq ($(OS), Darwin)
	$(CC) $(GFLAGS) -fsanitize=address  $(OBJS_T) -L $(LIBFT) -lft  -lreadline -o bin/test
	@bin/test
else ifeq ($(OS), Linux)
	@$(CC) $(GFLAGS) -g $(OBJS_T) -L$(LIBFT) -lft -lreadline -o test
	@valgrind --leak-check=full --track-origins=yes --log-file=valg_test  -s ./test
endif
else ifeq ($(PROD), $(EMPTY))
	@echo "\033[0;32m *** Start $(NAME) in prod env \033[0m"
	@$(MAKE) run
endif
# Check the NOFLAGS variable. If NOFLAGS=1, No flags are used for the compilation and a warning message is displayed.

# check if the of is darwin/mac



git: fclean
ifeq ($(gcom), $(EMPTY))
	git add .
	git commit -m $(NAME)/$(DATE)
else
	git add .
	git commit -m $(NAME)/$(gcom)/$(DATE)
endif
	git push origin $(BRANCH)


all: $(NAME)

re: clean fclean all

lib:
	cd libft   && make bonus
