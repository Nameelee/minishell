#include "exec.h"

//static void execute_pipe_recursive(t_token *node, char ***envp);
int ft_execute_builtin(t_token *node, char ***envp);

/**
 * @brief 단어의 앞뒤에 일치하는 따옴표(' 또는 ")가 있으면 제거하고 새로 할당된 문자열을 반환합니다.
 * 따옴표가 없으면 원본 문자열을 복제하여 반환합니다.
 * @param word 원본 단어 문자열.
 * @return 따옴표가 제거되었거나 복제된, 새로 할당된 문자열. 메모리 할당 실패 시 NULL.
 * @note 반환된 문자열은 호출자가 free() 해야 합니다.
*/ 
static char *remove_surrounding_quotes(const char *word)
{
    size_t len = 0;
    char *processed_word = NULL;

    if (!word) return (NULL); // NULL 입력 방지
    len = strlen(word);
	//fprintf(stderr, "[DEBUG quotes] Input word: [%s] (len %zu)\n", word, len); fflush(stderr); // 입력 확인
	
    // 길이가 2 이상이고 앞뒤로 같은 따옴표가 있는지 확인
    if (len >= 2 &&
        ((word[0] == '"' && word[len - 1] == '"') ||
         (word[0] == '\'' && word[len - 1] == '\'')))
    {
        // 따옴표 제외한 길이만큼 메모리 할당 (널 종료 문자 포함 +1)
        processed_word = malloc(len - 1); // (len - 2) + 1
        if (processed_word) {
            // 따옴표 안의 내용만 복사
            strncpy(processed_word, word + 1, len - 2);
            processed_word[len - 2] = '\0'; // 널 종료 확실히 하기
        } else {
            perror("malloc failed in remove_surrounding_quotes");
            // 실패 시 NULL 반환됨
        }
		//fprintf(stderr, "[DEBUG quotes] Quotes detected! Processed word: [%s]\n", processed_word); fflush(stderr); // 제거 후 결과 확인
    } else {
        // 따옴표가 없으면 원본 문자열 복제
        // ft_strdup이 필요 (libft 함수라고 가정)
        processed_word = ft_strdup(word);
        if (!processed_word) {
             perror("strdup failed in remove_surrounding_quotes");
             // 실패 시 NULL 반환됨
        }
		//fprintf(stderr, "[DEBUG quotes] No surrounding quotes. Duplicated word: [%s]\n", processed_word); fflush(stderr); // 복제 결과 확인
    }
    return (processed_word);
}




void close_all_heredoc_fds_in_tree(t_token *node) {

    if (!node) {
        return;
    }
    // Close FD if present and VALID (>= 0) for this node
    if (node->heredoc_pipe_fd >= 0) { // Check if FD is valid
        if (close(node->heredoc_pipe_fd) == -1) {
            perror("close (heredoc fd in tree)");
            // Continue anyway, try to close others
        }
        // Mark FD as invalid after closing. Optionally reset state.
        node->heredoc_pipe_fd = -1;
        // node->heredoc_state = HD_NOT_PROCESSED; // Optional: Reset state? Or keep as OK/FAILED? Better keep.
    }
    // Recurse through children
    close_all_heredoc_fds_in_tree(node->left);
    close_all_heredoc_fds_in_tree(node->right);
}

int preprocess_heredocs(t_token *node) 
{
    if (!node) {
        return 0; // Success for null node
    }

    // --- Check State for THIS node FIRST ---
    // If it's a HEREDOC node that has already been processed or failed,
    // skip processing it again, but MUST still process children.
    if (node->token == HEREDOC && node->heredoc_state != HD_NOT_PROCESSED) {
        // Still need to traverse children for *other* potential heredocs below this one.
        if (preprocess_heredocs(node->left) == -1) return -1;
        if (preprocess_heredocs(node->right) == -1) return -1;
        // Return the stored status of this node.
        return (node->heredoc_state == HD_PROCESSING_FAILED) ? -1 : 0;
    }

    // --- Process THIS node (if it's an unprocessed HEREDOC) ---
    if (node->token == HEREDOC /* && node->heredoc_state == HD_NOT_PROCESSED implied */) 
	{
        if (!node->right || !node->right->string) 
		{
            //fprintf(stderr, "minishell: syntax error near heredoc (missing delimiter)\n");
            node->heredoc_state = HD_PROCESSING_FAILED; // Mark as failed
            node->heredoc_pipe_fd = -1;
            return -1; // Failure
        }

		// Inside preprocess_heredocs, after the call:
		node->heredoc_pipe_fd = read_heredoc_to_pipe(node->right->string);

		if (node->heredoc_pipe_fd == -1) {
			//fprintf(stderr, "[DEBUG preprocess] Setting state to HD_PROCESSING_FAILED for '%s'\n", node->right->string); // DEBUG Print
			node->heredoc_state = HD_PROCESSING_FAILED;
			return -1;
		}
		// Success!
		////fprintf(stderr, "[DEBUG preprocess] Setting state to HD_PROCESSED_OK for '%s'\n", node->right->string); // DEBUG Print
		node->heredoc_state = HD_PROCESSED_OK;
    }

    // --- Recursively process children AFTER potentially processing this node ---
    // This ensures nested heredocs are found and processed.
    if (preprocess_heredocs(node->left) == -1) {
        // Propagate failure. If this node just opened an FD, it will be
        // cleaned up later by close_all_heredoc_fds_in_tree if execution fails.
        return -1;
    }
    if (preprocess_heredocs(node->right) == -1) {
        // Propagate failure.
        return -1;
    }

    // Return success (0) if this node wasn't a HEREDOC, or if it was a HEREDOC
    // and was successfully processed just now (state is HD_PROCESSED_OK).
    // If it was a HEREDOC and failed just now, -1 was already returned.
    // If it was a HEREDOC processed previously, the check at the top handles it.
    if (node->token == HEREDOC) {
         return (node->heredoc_state == HD_PROCESSING_FAILED) ? -1 : 0;
    } else {
         return 0; // Success for non-heredoc node if children succeeded.
    }
}

int read_heredoc_to_pipe(const char *delimiter) {
    int pipefd[2];
    if (pipe(pipefd) == -1) { // Possibility 1: pipe() failed
        perror("pipe (heredoc)");
        return -1;
    }

    char *line = NULL;
    bool read_success = true; // Assuming you added this flag

        ////fprintf(stderr, "[DEBUG read_heredoc] Expecting delimiter: \"%s\"\n", delimiter); // DEBUG

    while (1)
    {
        line = readline("> ");
        if (!line) {
            ////fprintf(stderr, "minishell: warning: here-document delimited by end-of-file (wanted `%s')\n", delimiter);
            read_success = false;
            break;
        }

        // The comparison:
        if (strcmp(line, delimiter) == 0) {
            ////fprintf(stderr, "[DEBUG read_heredoc] strcmp SUCCESS!\n"); // DEBUG
            free(line);
            break;
        } 

        // ... write to pipe ...
         ssize_t line_len = strlen(line);
         if (write(pipefd[1], line, line_len) != line_len || write(pipefd[1], "\n", 1) != 1) {
             perror("write (heredoc pipe)");
             free(line);
             read_success = false;
             close(pipefd[0]);
             close(pipefd[1]);
             return -1;
         }
        free(line);
    }
    // ... close FDs and return ...
    close(pipefd[1]);
    if (read_success) {
        return pipefd[0];
    } else {
        close(pipefd[0]);
        return -1;
    }
}


static int execute_pipe(t_token *node, char ***envp) 
{
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe (main)"); 
		return(1);
    }

    pid_t left_pid = fork();
    if (left_pid == -1) {
        perror("fork (pipe left)"); close(pipefd[0]); close(pipefd[1]); 
		return(1);
    }

    if (left_pid == 0) { // Left Child
        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2 stdout to pipe"); close(pipefd[1]); exit(EXIT_FAILURE);
        }
        close(pipefd[1]);
        execute_ast(node->left, envp, false); // Execute left side
        //exit(EXIT_SUCCESS); // Exit after execution
		exit(EXIT_FAILURE); // execute_ast는 보통 자체적으로 exit하므로 이 라인에 도달하지 않아야 함
    }

    pid_t right_pid = fork();
    if (right_pid == -1) {
        perror("fork (pipe right)"); close(pipefd[0]); close(pipefd[1]);
        waitpid(left_pid, NULL, 0); 
		return(1);
    }

    if (right_pid == 0) { // Right Child
        close(pipefd[1]);
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2 stdin from pipe"); close(pipefd[0]); exit(EXIT_FAILURE);
        }
        close(pipefd[0]);
        execute_ast(node->right, envp, false); // Execute right side
        //exit(EXIT_SUCCESS); // Exit after execution
		exit(EXIT_FAILURE); // execute_ast는 보통 자체적으로 exit하므로 이 라인에 도달하지 않아야 함
    }

    // Parent of pipe
    close(pipefd[0]);
    close(pipefd[1]);
    int left_status, right_status;
    waitpid(left_pid, &left_status, 0);
    waitpid(right_pid, &right_status, 0);
    
    int calculated_status_to_return; // 반환할 종료 코드를 저장할 변수

    // 오른쪽 자식의 상태를 기준으로 종료 코드 계산
    if (WIFEXITED(right_status)) {
        calculated_status_to_return = WEXITSTATUS(right_status);
    } else if (WIFSIGNALED(right_status)) {
        calculated_status_to_return = 128 + WTERMSIG(right_status);
    } else {
        calculated_status_to_return = 1; // 기본 오류
    }

    // ▼▼▼ 여기에 요청하신 디버그 프린트를 추가합니다 ▼▼▼
   // fprintf(stderr, "[PID %d] DEBUG_EXEC_PIPE: Right child finished (Raw status=%d). Returning calculated status: %d.\n",
    //        getpid(), right_status, calculated_status_to_return);
    // ▲▲▲ ▲▲▲ ▲▲▲ ▲▲▲ ▲▲▲ ▲▲▲

    return calculated_status_to_return; // 계산된 최종 상태 반환
}


void heredoc_redirect(t_token *node)
{
    if (!node || !node->right)
    {
        //fprintf(stderr, "Heredoc error: Missing delimiter or command.\n");
        return;
    }

    char *delimiter = node->right->string;
    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return;
    }
    else if (pid == 0) // Child process for heredoc reading
    {
        close(pipefd[0]); // Close the read end, we're only writing

        char *line;
        while (1)
        {
            //line = readline("> ");
			line = readline("heredoc> ");
            if (!line || strcmp(line, delimiter) == 0) // Stop when delimiter is found
            {
                free(line);
                break;
            }
            write(pipefd[1], line, strlen(line));
            write(pipefd[1], "\n", 1); // Add newline to each line
            free(line);
        }

        close(pipefd[1]); // Close the write end
        exit(0); // End the child process
    }
    else // Parent process
    {
        close(pipefd[1]); // Close write end, we're only reading
        waitpid(pid, NULL, 0);

        // Redirect stdin to read from the pipe
        if (dup2(pipefd[0], STDIN_FILENO) == -1)
        {
            perror("dup2 failed");
            close(pipefd[0]);
            return;
        }
        close(pipefd[0]);
    }
}

// --- Helper functions to build argv from AST ---

// Counts arguments (WORD tokens) chained to the right of a command node
static int count_ast_args(t_token *first_arg_node) {
    int count = 0;
    t_token *current = first_arg_node;
    // Traverse the right-child chain as long as it's a WORD
    while (current && current->token == WORD) {
        count++;
        current = current->right;
    }
    return count;
}

// Builds an argv array from a command node and its argument chain.
// Returns a NULL-terminated argv array (malloc'd). Caller must free the array.
// Returns NULL on malloc failure.
static char **build_argv_from_ast(t_token *cmd_node) {
    if (!cmd_node) return NULL;

    // Count arguments +1 for command name itself
    int argc = 1 + count_ast_args(cmd_node->right);

    // Allocate argv array (+1 for the final NULL pointer)
    char **argv = malloc(sizeof(char *) * (argc + 1));
    if (!argv) {
        perror("minishell: malloc failed for argv");
        return NULL;
    }

    // Fill argv array
    argv[0] = cmd_node->string; // Command name
    t_token *current_arg_node = cmd_node->right;
    int i = 1;
    while (current_arg_node && current_arg_node->token == WORD && i < argc) {
         argv[i] = current_arg_node->string; // Point to string in AST node
         current_arg_node = current_arg_node->right;
         i++;
    }
    argv[i] = NULL; // NULL-terminate the array

    // Basic sanity check
    if (i != argc) 
	{
         free(argv); // Free partially built array
         return NULL;
    }

    return argv;
}

// --- Revised ft_execute_builtin ---

int ft_execute_builtin(t_token *node, char ***envp_ptr) {

    int squote;
    int dquote;
    int exit_status;
    char **original_argv;
    
    exit_status = 1;
    dquote = 0;
    squote = 0;
    original_argv = build_argv_from_ast(node);
    if (!original_argv || !original_argv[0]) {
        
        if(original_argv) free(original_argv);
        return(1);
    }
    if (strcmp(original_argv[0], "echo") == 0) {

        // cedric add get the single quote and double quote in node.right
        
        squote = node->right->single_quote;
        dquote = node->right->double_quote;
        exit_status = ft_echo(original_argv, squote, dquote , *envp_ptr); // Call ft_echo with quote-removed args
    }
    else if (strcmp(original_argv[0], "pwd") == 0)
        exit_status = ft_pwd(original_argv); // Pass processed args
    else if (strcmp(original_argv[0], "env") == 0)
        exit_status = ft_env(original_argv, envp_ptr); // Pass processed args
    else if (strcmp(original_argv[0], "cd") == 0)
        exit_status = ft_cd(original_argv); // Pass processed args
    else if (strcmp(original_argv[0], "export") == 0) 
        exit_status = ft_export(envp_ptr, original_argv); // Pass processed args
    else if (strcmp(original_argv[0], "unset") == 0)
        exit_status = ft_unset(original_argv, envp_ptr); // Pass processed args
    else if (strcmp(original_argv[0], "exit") == 0)
        ft_exit(original_argv);
    else
        exit_status = 127;
    int i;

    i = 0;
    while (original_argv[i])
    {
        free(original_argv[i++]);
    }
    free(original_argv);
    return(exit_status);
}

bool is_state_modifying_builtin(const char *cmd_name) {
    if (!cmd_name) return false;
    // TODO: Improve this check if cmd_name can contain args (e.g., parse first word)
    return strcmp(cmd_name, "cd") == 0 ||
           strcmp(cmd_name, "export") == 0 ||
           strcmp(cmd_name, "unset") == 0 ||
           strcmp(cmd_name, "exit") == 0;
}

// execute_ast 함수 (디버깅 프린트 보강)
void execute_ast(t_token *node, char ***envp, bool is_top_level) 
{
    if (!node) 
    {
        //if (is_top_level) g_exit_status = 0; // 최상위 호출에서 노드가 없으면 성공으로 간주 (예: 빈 입력)
        return;
    }

    if (node->token == BUILTIN) {
        // ft_execute_builtin 호출 (이 함수가 내부에서 exit() 함)
        
        int r = ft_execute_builtin(node, envp);
        // 아래는 도달하면 안 됨
        g_exit_status = r;
    }
    else
    {
        
        if (is_top_level) {
            // --- 1. 최상위 호출일 때 ---
            if (preprocess_heredocs(node) == -1) {
                close_all_heredoc_fds_in_tree(node);
                g_exit_status = 1; 
                //fprintf(stderr, "[PID %d] DEBUG_MAIN_SHELL_TOP_LEVEL: Heredoc preprocess failed. g_exit_status set to %d.\n", getpid(), g_exit_status);
                fflush(stderr);
                return;
            }

            pid_t pid_c1 = fork();

            if (pid_c1 == -1) {
                perror("fork (execute_ast top level)");
                close_all_heredoc_fds_in_tree(node);
                g_exit_status = 1; 
                //fprintf(stderr, "[PID %d] DEBUG_MAIN_SHELL_TOP_LEVEL: Fork failed. g_exit_status set to %d.\n", getpid(), g_exit_status);
                fflush(stderr);
                return;
            }

            if (pid_c1 == 0) {
                // --- 자식 프로세스 (C1) ---
                execute_ast(node, envp, false); // 재귀 호출 (실제 실행 담당)
                exit(EXIT_FAILURE); 
            } else {
                // --- 부모 프로세스 (메인 쉘) ---
                close_all_heredoc_fds_in_tree(node);

                int status_of_c1;
                waitpid(pid_c1, &status_of_c1, 0);

                int actual_c1_exit_code = 0;
                if (WIFEXITED(status_of_c1)) {
                    actual_c1_exit_code = WEXITSTATUS(status_of_c1);
                } else if (WIFSIGNALED(status_of_c1)) {
                    actual_c1_exit_code = 128 + WTERMSIG(status_of_c1);
                    if (WTERMSIG(status_of_c1) == SIGINT) { fprintf(stderr, "\n"); }
                    else if (WTERMSIG(status_of_c1) == SIGQUIT) { fprintf(stderr, "Quit: %d\n", WTERMSIG(status_of_c1)); }
                } else {
                    actual_c1_exit_code = 255; 
                    //fprintf(stderr, "[PID %d] DEBUG_MAIN_SHELL_TOP_LEVEL: Child_C1 (PID %d) terminated abnormally (Raw_status=%d).\n", getpid(), pid_c1, status_of_c1);
                    fflush(stderr);
                }
                g_exit_status = actual_c1_exit_code;
                //fprintf(stderr, "[PID %d] DEBUG_MAIN_SHELL_TOP_LEVEL: Child_C1_PID=%d finished. Raw_status_C1=%d. Calculated_exit_code=%d. g_exit_status is now set to %d.\n",
                //        getpid(), pid_c1, status_of_c1, actual_c1_exit_code, g_exit_status);
                fflush(stderr);
            }
        } else {
            // --- 2. 재귀 호출일 경우 (is_top_level == false) ---
            // 이 블록은 자식 프로세스(C1 또는 그 자식) 내부에서 실행됨

            // ▼▼▼ 블록 진입 확인 (가장 먼저 실행되어야 할 디버그 프린트) ▼▼▼
            //fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: Entered. Node type: %d, Node string: [%s]\n", 
            //        getpid(), node->token, node->string ? node->string : "NULL_STRING");
            fflush(stderr);
            // ▲▲▲ ▲▲▲ ▲▲▲

            t_redir *redir_list = NULL;
            t_token *command_node = node; 

            // --- Step 1: 리다이렉션 정보 추출 ---
            int redir_loop_count = 0;
            //fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: Starting redirection extraction loop. Initial command_node: %p (type: %d, str: [%s])\n", 
            //        getpid(), (void *)command_node, command_node ? command_node->token : -1, command_node && command_node->string ? command_node->string : "NULL_STRING" );
            fflush(stderr);

            while (command_node && (command_node->token == REDIR_OPEN ||
                                    command_node->token == REDIR_WRITE ||
                                    command_node->token == REDIR_WRITE_A ||
                                    command_node->token == HEREDOC))
            {
            //    fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: In redir extraction loop iter %d. command_node type: %d, string: [%s]\n", 
            //            getpid(), redir_loop_count, command_node->token, command_node->string ? command_node->string : "NULL_STRING");
                
                if (!command_node->right || !command_node->right->string) {
            //        fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: Syntax error near redirection (missing filename/delimiter for token string [%s]). Exiting.\n", 
            //                getpid(), command_node->string ? command_node->string : "NULL_STRING");
                    fflush(stderr);
                    exit(1); 
                }
                // filename/delimiter가 있는지 확인 후 출력
            //    fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: Redir Op [%s], Filename/Delimiter is [%s]\n",
            //            getpid(), command_node->string, command_node->right->string);
                fflush(stderr);

                add_redirection_to_list(&redir_list, command_node->token, command_node->right->string, (command_node->token == HEREDOC) ? command_node : NULL);
                
            //    fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: Added redir type %d. Moving to command_node->left.\n", 
            //            getpid(), command_node->token);
                fflush(stderr);
                command_node = command_node->left; 
                redir_loop_count++;
            }
            //fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: Finished redirection extraction loop. Final command_node: %p (type: %d, str: [%s])\n", 
            //        getpid(), (void *)command_node, command_node ? command_node->token : -1, command_node && command_node->string ? command_node->string : "NULL_STRING");
            fflush(stderr);
            
            // --- apply_redirections 호출 전 디버그 프린트 ---
            //fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: Before calling apply_redirections. redir_list pointer is: %p\n", getpid(), (void *)redir_list);
            // if (redir_list) {
            //     fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: First redir item - Type: %d (Filename: [%s])\n", 
            //             getpid(), redir_list->type, 
            //             redir_list->filename ? redir_list->filename : "NULL_FILENAME");
            // } else {
            //     fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: redir_list is NULL.\n", getpid());
            // }
            // fflush(stderr);
            // --- ---

            // --- Step 2: 리다이렉션 적용 ---
            if (apply_redirections(redir_list) == -1) {
                //fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: apply_redirections failed. Exiting.\n", getpid());
                fflush(stderr);
                free_redir_list(redir_list); 
                exit(EXIT_FAILURE); 
            }
            free_redir_list(redir_list); 

            // --- Step 4: 명령어 노드 실행 ---
            if (!command_node) {
                //fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: No command_node after redirections. Exiting with SUCCESS.\n", getpid());
                fflush(stderr);
                exit(EXIT_SUCCESS); 
            }
            
            //fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: Dispatching command_node. Type: %d, String: [%s]\n", 
            //        getpid(), command_node->token, command_node->string ? command_node->string : "NULL_STRING");
            fflush(stderr);

            if (command_node->token == PIPE) {
                int pipeline_exit_status = execute_pipe(command_node, envp);
            //    fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE(PIPE): Pipeline status received: %d. This process exiting with this.\n", getpid(), pipeline_exit_status);
                fflush(stderr);
                exit(pipeline_exit_status);
            }
            else if (command_node->token == CMD || command_node->token == WORD )
            {
                // 외부 명령어 실행 로직
                char **original_argv = build_argv_from_ast(command_node);
                char **processed_argv = NULL; // $? 등 확장 및 따옴표 최종 제거된 argv
                int argc = 0;

                if (!original_argv || !original_argv[0]) { if(original_argv) free(original_argv); exit(127); }
                while(original_argv[argc] != NULL) argc++;

                processed_argv = (char **)malloc(sizeof(char *) * (argc + 1));
                if (!processed_argv) { perror("malloc processed_argv (external)"); free(original_argv); exit(EXIT_FAILURE); }
                
                // 명령어 이름은 보통 확장/따옴표 제거 대상이 아니지만, 일관성을 위해 strdup 또는 확장 처리
                processed_argv[0] = ft_strdup(original_argv[0]); // 안전하게 복사
                if (!processed_argv[0] && original_argv[0]) { /* malloc 에러 처리 */ }

                for (int k = 1; k < argc; k++) {
                    char *temp_arg = remove_surrounding_quotes(original_argv[k]);
                    if (!temp_arg) { /* 에러 처리 및 이전 할당 해제 */ }
                    // 여기에 $? 확장 함수 호출 추가: processed_argv[k] = expand_parameters(temp_arg, g_exit_status);
                    // 지금은 $? 확장이 없으므로, 일단 remove_surrounding_quotes 결과만 사용
                    processed_argv[k] = temp_arg; 
                    // free(temp_arg); // expand_parameters가 temp_arg를 쓰고 새 문자열 반환 시 해제
                }
                processed_argv[argc] = NULL;

            //    fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: Executing external command: [%s]\n", getpid(), processed_argv[0]); fflush(stderr);
                execvp(processed_argv[0], processed_argv); // envp는 execvp가 알아서 현재 환경 사용
                
                perror(processed_argv[0]); // execvp 실패 시에만 실행됨
                // Cleanup on execvp error
                if (processed_argv[0] != original_argv[0]) free(processed_argv[0]); // strdup 했을 경우
                for (int k = 1; k < argc; k++) if(processed_argv[k]) free(processed_argv[k]);
                free(processed_argv); 
                free(original_argv);
                exit(127); 

            } else {
            //    fprintf(stderr, "[PID %d] DEBUG_EXEC_AST_RECURSIVE: Invalid command node type %d. Exiting.\n", getpid(), command_node->token);
                fflush(stderr);
                exit(EXIT_FAILURE);
            }
        }
    }
} // End of execute_ast