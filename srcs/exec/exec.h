#ifndef EXEC_H
#define EXEC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

typedef struct s_token t_token;

#include "../parser/tokenizer/tokenize.h"
#include "../builtin/builtin.h"


void execute_ast(t_token *node, char ***envp, bool is_top_level);
int     read_heredoc_to_pipe(const char *delimiter);

#endif