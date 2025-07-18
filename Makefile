NAME= minishell

##########################  OS check
OS = $(shell uname)

# Define OS-specific flags and compiler
ifeq ($(OS), Darwin)
	CC=cc
	GFLAGS= -Werror -Wall -Wextra
	# For macOS, set readline flags using Homebrew prefix
	BREW_PREFIX = $(shell brew --prefix)
	# Compiler flag to find readline headers
	INC_RL = -I$(BREW_PREFIX)/opt/readline/include
	# Linker flags to find readline library
	LIB_RL = -L$(BREW_PREFIX)/opt/readline/lib -lreadline
else ifeq ($(OS), Linux)
	CC=gcc
	GFLAGS= -Werror -Wall -Wextra
	# For Linux, readline is usually in a standard path
	INC_RL =
	LIB_RL = -lreadline
endif

PROD=0

MEMORY_CHECK_PATH= error_managment/valgrind

SRCS_MAIN= srcs/main.c
SRCS_PARSER= $(wildcard srcs/parser/lexer/*.c) $(wildcard srcs/parser/tokenizer/*.c) $(wildcard srcs/parser/*.c)
SRCS_EXEC= $(wildcard srcs/exec/*.c)
SRCS_BUILTIN= $(wildcard srcs/builtin/*/*.c) $(wildcard srcs/builtin/*/*/*.c)
SRCS_TEST= $(wildcard test_unit/*.c)

LIBFT_DIR = libft
LIBFT_A = $(LIBFT_DIR)/libft.a

# --- Check if libft directory exists at the beginning ---
ifeq ($(wildcard $(LIBFT_DIR)/.),)
	$(error The libft directory '$(LIBFT_DIR)' was not found. Please ensure it exists.)
endif

%.o:%.c
	$(CC) $(GFLAGS) $(INC_RL) -g -c $< -o $@

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

.PHONY: all clean fclean run git testenv var re lib

# 'all' is the default goal, it depends on our final executable.
all: $(NAME)

# --- 모든 명령어 라인은 반드시 탭(Tab)으로 시작해야 합니다. ---
$(NAME): $(OBJS) $(LIBFT_A)
	echo "---[ All prerequisites are ready. Linking $(NAME) ]---"
	$(CC) $(GFLAGS) $(OBJS) -L$(LIBFT_DIR) -lft -o $(NAME) $(LIB_RL)
	echo "\n\033[1;32mSuccessfully compiled $(NAME)!\033[0m"

$(LIBFT_A):
	echo "---[ Prerequisite '$(LIBFT_A)' is missing. Running 'make' in $(LIBFT_DIR) ]---"
	$(MAKE) -C $(LIBFT_DIR) bonus
	echo "---[ Finished 'make' in $(LIBFT_DIR). Checking for result... ]---"
	if [ ! -f $(LIBFT_A) ]; then \
		echo "\n\033[1;31mFATAL ERROR: libft.a was NOT created by 'make -C $(LIBFT_DIR) bonus'.\033[0m"; \
		echo "This is the root cause of the 'cannot find -lft' linker error."; \
		echo "Check the compilation log from libft above for any errors."; \
		exit 1; \
	fi
	echo "---[ Found $(LIBFT_A). Proceeding. ]---"

# 사용자가 'make lib'를 직접 실행할 수 있도록 남겨두는 규칙
lib:
	$(MAKE) $(LIBFT_A)

run: $(NAME)
	valgrind --leak-check=full --log-file=val_report -s ./$(NAME)

# cleaning rules
clean:
	echo "---[ Cleaning object files ]---"
	rm -f $(OBJS) $(OBJS_TEST)
	rm -f test_unit/test_unit
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	echo "---[ Full clean ]---"
	rm -f $(NAME) test_unit/test
	rm -f valg_test
	$(MAKE) -C $(LIBFT_DIR) fclean

mclean:
	rm -f $(MEMORY_CHECK_PATH)/*

tenv: $(LIBFT_A) $(OBJS_T)
ifeq ($(PROD), 0)
	echo "\033[44m *** Start $(NAME) in test env \033[0m"
ifeq ($(NOFLAGS), 1)
	echo "\033[41m *** NO FLAGS! \033[0m\n"
endif
ifeq ($(OS), Darwin)
	$(CC) $(GFLAGS) $(INC_RL) -fsanitize=address  $(OBJS_T) -L $(LIBFT_DIR) -lft $(LIB_RL) -o bin/test
	bin/test
else ifeq ($(OS), Linux)
	$(CC) $(GFLAGS) -g $(OBJS_T) -L$(LIBFT_DIR) -lft $(LIB_RL) -o test
	valgrind --leak-check=full --track-origins=yes --log-file=valg_test  -s ./test
endif
else ifeq ($(PROD), $(EMPTY))
	echo "\033[0;32m *** Start $(NAME) in prod env \033[0m"
	$(MAKE) run
endif

git: fclean
ifeq ($(gcom), $(EMPTY))
	git add .
	git commit -m $(NAME)/$(DATE)
else
	git add .
	git commit -m $(NAME)/$(gcom)/$(DATE)
endif
	git push origin $(BRANCH)

re: fclean all