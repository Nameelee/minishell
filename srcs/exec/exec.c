// 파일: srcs/exec/exec.c
#include "exec.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
// readline 관련 헤더 (read_heredoc_to_pipe에서 필요할 수 있음)
#include <readline/readline.h>
#include <readline/history.h>


// ****** 추가: Heredoc 관련 임시 빈 함수 정의 (컴파일 오류 해결용) ******
// TODO: 실제 heredoc 기능 구현 시 이 함수들을 채워야 합니다.
int preprocess_heredocs(t_token *node) {
    // 이 함수는 AST를 순회하며 heredoc (<<)을 찾아 처리하고,
    // 그 결과를 t_token 노드의 heredoc_pipe_fd와 heredoc_state에 저장해야 합니다.
    // 현재는 임시로 성공(0)을 반환합니다.
    (void)node; 
    // fprintf(stderr, "DEBUG: preprocess_heredocs called (dummy implementation)\n");
    return 0; 
}

void close_all_heredoc_fds_in_tree(t_token *node) {
    // 이 함수는 AST를 순회하며 열려있는 모든 heredoc_pipe_fd를 닫아야 합니다.
    (void)node; 
    // fprintf(stderr, "DEBUG: close_all_heredoc_fds_in_tree called (dummy implementation)\n");
}
// ****** 빈 함수 정의 끝 ******

// read_heredoc_to_pipe 함수 (사용자 제공 원본 또는 이전 수정본)
// 이 함수는 실제 구현이 필요하며, 사용자님의 코드에 이미 존재할 가능성이 높습니다.
// 만약 없다면, 아래는 기본적인 readline을 사용한 예시입니다.
int read_heredoc_to_pipe(const char *delimiter) {
    int pipefd[2];
    char *line = NULL;

    if (pipe(pipefd) == -1) {
        perror("minishell: pipe for heredoc failed");
        return -1;
    }

    while (1) {
        line = readline("> "); // heredoc 프롬프트
        if (!line) { // EOF (Ctrl+D)
            fprintf(stderr, "minishell: warning: here-document delimited by end-of-file (wanted `%s')\n", delimiter);
            break; 
        }
        if (strcmp(line, delimiter) == 0) {
            free(line);
            break;
        }
        write(pipefd[1], line, strlen(line));
        write(pipefd[1], "\n", 1);
        free(line);
    }
    close(pipefd[1]); // 쓰기 파이프 끝을 닫음
    return pipefd[0]; // 읽기 파이프 끝을 반환
}


// build_argv_from_ast 함수 (이전 답변의 안정화된 버전)
static char **build_argv_from_ast(t_token *cmd_node, char ***envp_ptr) { // Added envp_ptr
    if (!cmd_node) return NULL;
    int count = 0;
    t_token *curr = cmd_node;
    // Count command and its arguments (WORD, VAR tokens to the right)
    while (curr && (curr->token == CMD || curr->token == BUILTIN || curr->token == WORD || curr->token == VAR)) {
        count++;
        curr = curr->right;
    }
    if (count == 0) return NULL;

    char **argv = (char **)malloc(sizeof(char *) * (count + 1));
    if (!argv) {
        perror("minishell: malloc for argv failed");
        return NULL;
    }

    curr = cmd_node;
    for (int i = 0; i < count; i++) {
        if (curr && curr->string) {
            // Use the new expansion function
            argv[i] = expand_all_variables(curr->string, *envp_ptr, curr->single_quote, curr->double_quote);
            if (!argv[i]) { // Expansion failed (e.g., malloc error inside)
                perror("minishell: argument expansion failed");
                for (int k = 0; k < i; k++) free(argv[k]);
                free(argv);
                return NULL;
            }
        } else {
            fprintf(stderr, "minishell: build_argv_from_ast: encountered NULL string or node unexpectedly.\n");
            for (int k = 0; k < i; k++) free(argv[k]); // Free previously allocated strings
            free(argv);
            return NULL;
        }
        curr = curr->right;
    }
    argv[count] = NULL;
    return argv;
}


// ft_execute_builtin 함수 (이전 답변의 안정화된 버전)
int ft_execute_builtin(t_token *node, char ***envp_ptr) 
{
	//int squote = 0;
    //int dquote = 0;
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
    // ... (export, unset, env, exit need to use original_argv directly)
    // For export, unset, env, if they do their own parsing/expansion, ensure they respect quotes.
    // For ft_exit, it parses its numeric arg; ensure its input is correctly pre-processed if needed.
    // The `expand_all_variables` should correctly prepare strings for these too.
    else if (ft_strncmp(original_argv[0], "export", 7) == 0) {
        exit_status = ft_export(envp_ptr, original_argv);
    } else if (ft_strncmp(original_argv[0], "unset", 6) == 0) {
        exit_status = ft_unset(original_argv, envp_ptr);
    } else if (ft_strncmp(original_argv[0], "env", 4) == 0) {
        exit_status = ft_env(original_argv, envp_ptr);
    } else if (ft_strncmp(original_argv[0], "exit", 5) == 0) {
        ft_exit(original_argv); // ft_exit handles its own exit, status might be set globally
        exit_status = g_exit_status; // Or get it from g_exit_status if ft_exit updates it
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

// execute_ast 함수 (heredoc 호출 부분 주석 해제)
void execute_ast(t_token *node, char ***envp, bool is_top_level) {

    if (!node) {
        if (is_top_level) g_exit_status = 0;
        return;
    }
	// printf("Current: '%s'\n", node->string);
	// if (node->parent)
	// 	printf("Parent: '%s'\n", node->parent->string);
	// if (node->left)
	// 	printf("Left: '%s'\n", node->left->string);
	// if (node->right)
	// 	printf("Right: '%s'\n", node->right->string);
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

        // 리다이렉션 정보 추출 (AST 구조에 따라 이 부분은 달라질 수 있음)
        // 현재 AST 구조: REDIR_NODE -> left = CMD_CHAIN, right = FILENAME_NODE
        // CMD_CHAIN: CMD_TOKEN -> right = ARG1 -> right = ARG2 ...
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
            if (!argv || !argv[0]) {
                if (argv) free(argv); // argv 배열만 해제
                fprintf(stderr, "minishell: Error building argv for command (token: %s)\n", command_node_to_exec->string ? command_node_to_exec->string : "N/A");
                exit(127); 
            }
            execvp(argv[0], argv);
            perror(argv[0]); // execvp 실패 시에만 실행됨
            if (argv) free(argv); // argv 배열만 해제
            exit(127); 
        }
        else {


            if(ft_is_variable(command_node_to_exec->string) && !is_expendable_variable(command_node_to_exec->string, *envp))
                exit(0);
            else if (ft_is_variable(command_node_to_exec->string) && is_expendable_variable(command_node_to_exec->string, *envp) == 1)
            {
                write(STDERR_FILENO, " Is a directory\n", ft_strlen(" Is a directory\n"));
                exit(126);
            }
            else if(is_expendable_variable(command_node_to_exec->string, *envp) == 2)
            {
                write(STDERR_FILENO, " command not found\n", ft_strlen(" command not found\n"));
                exit(127);
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
