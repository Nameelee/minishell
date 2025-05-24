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


// execute_ast는 이미 선언되어 있을 것으로 예상됩니다.
void execute_ast(t_token *node, char ***envp, bool is_top_level);

// 빌트인 실행 함수 (만약 exec.c에 정의되어 있다면)
int ft_execute_builtin(t_token *node, char ***envp_ptr); // 사용자 코드에 이미 있음

// Heredoc 관련 함수 선언 (만약 이들을 구현하고 사용한다면 여기에 추가)
int preprocess_heredocs(t_token *node);
void close_all_heredoc_fds_in_tree(t_token *node);
int read_heredoc_to_pipe(const char *delimiter); // 이것은 이미 exec.h에 있습니다.
char *expand_all_variables(const char *input_str, char **envp, bool is_single_quoted, bool is_double_quoted);

#endif