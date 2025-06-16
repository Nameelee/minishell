/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   exec.c                                              :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/12 14:09:23 by jelee          #+#    #+#                */
/*   Updated: 2025/06/12 14:09:26 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>


int preprocess_heredocs(t_token *node)
{
    (void)node; 
    return (0); 
}

void close_all_heredoc_fds_in_tree(t_token *node)
{
    (void)node; 
}

/**
 * @brief Reads user input for a heredoc and writes it to the provided pipe.
 * @param write_fd  The file descriptor for the write-end of the pipe.
 * @param delimiter The string that signals the end of input.
 */
static void heredoc_input_loop(int write_fd, const char *delimiter)//1
{
    char *line;

    while (1)
    {
        line = readline("> ");
        if (!line) // Handle EOF (Ctrl+D)
        {
            fprintf(stderr, 
                "minishell:here-doc delimited by end-of-file (wanted `%s')\n", 
                delimiter);
            break;
        }
        if (strcmp(line, delimiter) == 0) // Handle delimiter
        {
            free(line);
            break;
        }
        write(write_fd, line, strlen(line));
        write(write_fd, "\n", 1);
        free(line);
    }
}

/**
 * @brief Creates a pipe and fills it with heredoc input.
 * @param delimiter The heredoc delimiter string.
 * @return The file descriptor for the read-end of the pipe, or -1 on error.
 */
int read_heredoc_to_pipe(const char *delimiter)//1
{
    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        perror("minishell: pipe for heredoc failed");
        return (-1);
    }
    heredoc_input_loop(pipefd[1], delimiter);
    close(pipefd[1]); // Close the write-end, as it's no longer needed.
    return (pipefd[0]); // Return the read-end.
}

// build_argv_from_ast: 비어있는 변수 확장을 처리하도록 수정된 함수
static char **build_argv_from_ast(t_token *cmd_node, char ***envp_ptr)
{
    if (!cmd_node)
        return (NULL);
    
    int max_count = 0;
    t_token *curr = cmd_node;
    while (curr && (curr->token == CMD || curr->token == BUILTIN || curr->token == WORD || curr->token == VAR))
    {
        max_count++;
        curr = curr->right;
    }
    if (max_count == 0)
        return (NULL);

    char **argv = (char **)malloc(sizeof(char *) * (max_count + 1));
    if (!argv)
    {
        perror("minishell: malloc for argv failed");
        return (NULL);
    }

    curr = cmd_node;
    int actual_i = 0;
    for (int token_i = 0; token_i < max_count; token_i++)
    {
        if (curr && curr->string)
        {
            char *expanded_str = expand_all_variables(curr->string, *envp_ptr, curr->single_quote, curr->double_quote);
            if (!expanded_str)
            {
                perror("minishell: argument expansion failed");
                for (int k = 0; k < actual_i; k++)
                    free(argv[k]);
                free(argv);
                return (NULL);
            }
            
            // 핵심 변경사항: 확장 결과가 비어있고, 큰따옴표로 묶이지 않았다면 인자 리스트에서 제거
            if (expanded_str[0] == '\0' && curr->double_quote == 0)
            {
                free(expanded_str);
            }
            else
            {
                argv[actual_i] = expanded_str;
                actual_i++;
            }
        }
        else
        {
            fprintf(stderr, "minishell: build_argv_from_ast: encountered NULL string or node unexpectedly.\n");
            for (int k = 0; k < actual_i; k++)
                free(argv[k]);
            free(argv);
            return (NULL);
        }
        curr = curr->right;
    }
    argv[actual_i] = NULL;
    
    // 모든 인자가 비어있는 문자열로 확장되어 사라진 경우
    if (actual_i == 0)
    {
        free(argv);
        return (NULL);
    }
    return (argv);
}

// ft_execute_builtin 함수 (이전 답변의 안정화된 버전)
int ft_execute_builtin(t_token *node, char ***envp_ptr) 
{

    char **original_argv = NULL;
    int exit_status = 1;

    original_argv = build_argv_from_ast(node, envp_ptr); // Pass envp_ptr
    if (!original_argv || !original_argv[0]) {
        if (original_argv) free(original_argv); // Free if partially allocated then failed
        return (1);
    }

    if (ft_strncmp(original_argv[0], "echo", 5) == 0) {
                // cedric add get the single quote and double quote in node.right
        
        //squote = node->right->single_quote;
        //dquote = node->right->double_quote;
        exit_status = ft_echo(original_argv);
    }
    // ... other builtins cd, pwd ...
    else if (ft_strncmp(original_argv[0], "cd", 3) == 0) {
        exit_status = ft_cd(original_argv);
    }
    else if (ft_strncmp(original_argv[0], "pwd", 4) == 0) {
        exit_status = ft_pwd(original_argv);
    }
    else if (ft_strncmp(original_argv[0], "export", 7) == 0) {
        exit_status = ft_export(envp_ptr, original_argv);
    } else if (ft_strncmp(original_argv[0], "unset", 6) == 0) {
        exit_status = ft_unset(original_argv, envp_ptr);
    } else if (ft_strncmp(original_argv[0], "env", 4) == 0) {
        exit_status = ft_env(original_argv, envp_ptr);
    } else if (ft_strncmp(original_argv[0], "exit", 5) == 0) {
        ft_exit(original_argv);
        exit_status = g_exit_status;
    }
    else {
        fprintf(stderr, "minishell: %s: builtin not recognized in ft_execute_builtin\n", original_argv[0]);
        exit_status = 127;
    }

    // Free the argv array and its contents
    if (original_argv) {
        for (int i = 0; original_argv[i]; i++) {
            free(original_argv[i]);
        }
        free(original_argv);
    }
    return (exit_status);
}

// execute_pipe 함수 (이전 답변의 안정화된 버전)
static int execute_pipe(t_token *node, char ***envp) {
    int pipefd[2]; pid_t left_pid, right_pid; int left_status, right_status;
    if (pipe(pipefd) == -1) { perror("minishell: pipe failed"); return(1); }
    left_pid = fork();
    if (left_pid == -1) { perror("minishell: fork L failed"); close(pipefd[0]); close(pipefd[1]); return(1); }
    if (left_pid == 0) { 
        close(pipefd[0]); 
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) { perror("minishell: dup2 L_STDOUT"); close(pipefd[1]); exit(1); }
        close(pipefd[1]); 
        execute_ast(node->left, envp, false); exit(g_exit_status);
    }
    right_pid = fork();
    if (right_pid == -1) { perror("minishell: fork R failed"); close(pipefd[0]); close(pipefd[1]); waitpid(left_pid, NULL, 0); return(1); }
    if (right_pid == 0) { 
        close(pipefd[1]); 
        if (dup2(pipefd[0], STDIN_FILENO) == -1) { perror("minishell: dup2 R_STDIN"); close(pipefd[0]); exit(1); }
        close(pipefd[0]); 
        execute_ast(node->right, envp, false); exit(1);  
    }
    close(pipefd[0]); close(pipefd[1]);
    waitpid(left_pid, &left_status, 0); waitpid(right_pid, &right_status, 0);
    if (WIFEXITED(right_status)) { return WEXITSTATUS(right_status); }
    else if (WIFSIGNALED(right_status)) { return 128 + WTERMSIG(right_status); }
    return 1;
}

// argv 배열과 그 내용물을 해제하는 헬퍼 함수
static void free_argv(char **argv) {
    if (!argv) return;
    for (int i = 0; argv[i]; i++) {
        free(argv[i]);
    }
    free(argv);
}

// execute_ast 함수 (heredoc 호출 부분 주석 해제)
void execute_ast(t_token *node, char ***envp, bool is_top_level) {

    if (!node) {
        if (is_top_level) g_exit_status = 0;
        return;
    }
    if (is_top_level) {
        // ****** Heredoc 관련 함수 호출부 유지 (이제 선언과 빈 정의가 있음) ******
        if (preprocess_heredocs(node) == -1) { 
            close_all_heredoc_fds_in_tree(node); 
            g_exit_status = 1; 
            return;
        }
        // ****** 호출부 유지 끝 ******

        pid_t pid_c1 = fork();
        if (pid_c1 == -1) {
            perror("minishell: fork for C1 failed");
            // ****** Heredoc 관련 함수 호출부 유지 ******
            close_all_heredoc_fds_in_tree(node); 
            // ****** 호출부 유지 끝 ******
            g_exit_status = 1; 
            return;
        }

        if (pid_c1 == 0) { 
            execute_ast(node, envp, false); 
            exit(1); // execute_ast(false)는 내부에서 exit하므로 이 라인 도달 시 오류
        } else { 
            // ****** Heredoc 관련 함수 호출부 유지 ******
            close_all_heredoc_fds_in_tree(node); 
            // ****** 호출부 유지 끝 ******

            int status_of_c1;
            waitpid(pid_c1, &status_of_c1, 0);

            if (WIFEXITED(status_of_c1)) {
                g_exit_status = WEXITSTATUS(status_of_c1);
            } else if (WIFSIGNALED(status_of_c1)) {
                g_exit_status = 128 + WTERMSIG(status_of_c1);
                if (WTERMSIG(status_of_c1) == SIGINT) { 
                    write(STDOUT_FILENO, "\n", 1); // 시그널 핸들러와 중복될 수 있으니 주의
                }
                else if (WTERMSIG(status_of_c1) == SIGQUIT) { 
                    fprintf(stderr, "Quit: %d\n", WTERMSIG(status_of_c1)); 
                }
            } else {
                g_exit_status = 1; 
            }
        }
    } else { // is_top_level == false: 자식 프로세스 내부 로직
        t_redir *redir_list = NULL;
        t_token *command_node_to_exec = node; 

        while (command_node_to_exec && 
               (command_node_to_exec->token == REDIR_OPEN ||
                command_node_to_exec->token == REDIR_WRITE ||
                command_node_to_exec->token == REDIR_WRITE_A ||
                command_node_to_exec->token == HEREDOC))
        {
            if (!command_node_to_exec->right || !command_node_to_exec->right->string) { // filename node check
                fprintf(stderr, "minishell: syntax error: missing or invalid filename for redirection\n");
                exit(1); 
            }
            add_redirection_to_list(&redir_list, command_node_to_exec->token, 
                                   command_node_to_exec->right->string, 
                                   (command_node_to_exec->token == HEREDOC) ? command_node_to_exec : NULL);
            command_node_to_exec = command_node_to_exec->left; // 실제 명령어 부분으로 이동
        }
        
        // 리다이렉션 적용 (tokenize.c 또는 이 파일에 정의된 함수 사용)
        if (apply_redirections(redir_list) == -1) {
            free_redir_list(redir_list); 
            exit(1); 
        }
        free_redir_list(redir_list); 

        if (!command_node_to_exec) { // 리다이렉션만 있고 명령어가 없는 경우 (예: "> out")
            exit(0); // Bash는 이런 경우 성공(0)으로 종료
        }

        // 명령어 실행
        if (command_node_to_exec->token == PIPE) {
            int pipe_status = execute_pipe(command_node_to_exec, envp);
            exit(pipe_status); 
        }
        else if (command_node_to_exec->token == BUILTIN) {
             int status = ft_execute_builtin(command_node_to_exec, envp);
             exit(status); 
        }
        else if (command_node_to_exec->token == CMD || command_node_to_exec->token == WORD) {
            char **argv = build_argv_from_ast(command_node_to_exec, envp);
            
            // 만약 argv가 NULL이면, 명령어 전체가 비어있는 변수 확장 등으로 사라졌다는 의미.
            // bash에서는 오류가 아니므로, 성공(0)으로 종료.
            if (!argv) {
                exit(0);
            }

            if (ft_strchr(argv[0], '/'))
            {
                struct stat path_stat;
                if (stat(argv[0], &path_stat) == 0) { // stat 성공, 경로 존재
                    if (S_ISDIR(path_stat.st_mode)) {
                        fprintf(stderr, "minishell: %s: Is a directory\n", argv[0]);
                        free_argv(argv);
                        exit(126);
                    }
                }
            }

            execvp(argv[0], argv);
            
            // execvp는 오류 발생 시에만 반환됨
            fprintf(stderr, "minishell: %s: ", argv[0]);
            if (errno == EACCES) {
                fprintf(stderr, "Permission denied\n");
                free_argv(argv);
                exit(126);
            } else if (errno == ENOENT) {
                if (ft_strchr(argv[0], '/')) {
                     fprintf(stderr, "No such file or directory\n");
                } else {
                     fprintf(stderr, "command not found\n");
                }
                free_argv(argv);
                exit(127);
            } else {
                perror(NULL); // 그 외 다른 오류 메시지 출력
                free_argv(argv);
                exit(127);
            }
        }
        else {

            if(is_expendable_variable(command_node_to_exec->string, *envp) == 2
                || !ft_strncmp(command_node_to_exec->string,"$?",ft_strlen_longest(command_node_to_exec->string, "$?")))
            {
                write(STDERR_FILENO, " command not found\n", ft_strlen(" command not found\n"));
                exit(127);
            }
            else if(ft_is_variable(command_node_to_exec->string) && !is_expendable_variable(command_node_to_exec->string, *envp))
                exit(0);
            else if (ft_is_variable(command_node_to_exec->string) && is_expendable_variable(command_node_to_exec->string, *envp) == 1)
            {
                write(STDERR_FILENO, " Is a directory\n", ft_strlen(" Is a directory\n"));
                exit(126);
            }
            
            // Gestion explicite des erreurs "type de nœud non exécutable ou inconnu"
            if (!command_node_to_exec || !command_node_to_exec->string) {
                fprintf(stderr, "minishell: command not found\n");
                exit(127);
            }

            
            // Par défaut : commande inconnue (type invalide ou expansion incorrecte)
            fprintf(stderr, "minishell: %s: command not found (token type %d)\n",
                    command_node_to_exec->string, command_node_to_exec->token);
            exit(127);
        }
    }
}
