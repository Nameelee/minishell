#include "tokenize.h"
#include <assert.h>
#include <errno.h>

//static t_token *handle_operator_node(t_token *new_node, t_token **root_ptr);
// --- Forward Declarations for Helpers ---
static bool is_whitespace(char c);
static bool is_operator_start(char c);
//static t_token *add_and_advance(t_token **list_head, char *str, size_t len, int token_type, size_t *index);
void ft_add_back_node(t_token **lst, t_token *node);
static int ft_get_token_type_from_str(const char *str); // Needs implementation
static void free_token_list_on_error(t_token *list); // Simplified cleanup
static void free_ast_recursive(t_token *node);
static void free_single_token_node_content(t_token *node);
static t_token *find_last_node_in_simple_cmd(t_token *command_node);

static void free_ast_recursive(t_token *node) {
    if (!node) {
        return;
    }
    // 자식 노드들을 먼저 재귀적으로 해제
    // (주의: ft_create_ast 에러 경로에서는 left/right가 이미 다른 곳에서 해제되었거나,
    //  아직 제대로 연결되지 않았을 수 있으므로, 이 함수는 주로 완성된 AST 해제용입니다.
    //  ft_create_ast 내부 에러 시에는 좀 더 세심한 해제가 필요할 수 있습니다.)
    if (node->left) {
        free_ast_recursive(node->left);
        node->left = NULL; 
    }
    if (node->right) {
        free_ast_recursive(node->right);
        node->right = NULL;
    }
    // 현재 노드의 문자열과 노드 자체를 해제
    if (node->string) {
        free(node->string);
        node->string = NULL;
    }
    free(node);
}

static void free_single_token_node_content(t_token *node) {
    if (!node) {
        return;
    }
    if (node->string) {
        free(node->string);
        node->string = NULL;
    }
    free(node);
}

static t_token *find_last_node_in_simple_cmd(t_token *command_node) {
    if (!command_node) return NULL;
    t_token *current = command_node;
    // 인자는 WORD, STRING, VAR 토큰일 수 있음 (사용자 정의에 따름)
    while (current->right && 
           (current->right->token == WORD ||  
            current->right->token == ARG || current->right->token == VAR)) {
        current = current->right;
    }
    return current;
}
/*
void merge_word_tokens(t_token **token_list_head) { // 또는 void _tokens(...)
    (void)token_list_head; // 이 함수가 아무 작업도 하지 않도록 함
    
    // 디버깅을 위해 이 함수가 호출되었지만 아무것도 안 했음을 알리는 메시지 추가
    fprintf(stderr, "DEBUG: merge_word_tokens is temporarily a NO-OP (doing nothing).\n");
    fflush(stderr);
    
    return; // 바로 반환하여 아무 병합도 수행하지 않음
}
*/
void merge_word_tokens(t_token **token_list_head) {
    if (!token_list_head || !*token_list_head) {
        return;
    }

    //t_token *prev = NULL; // 이전 토큰을 추적하기 위한 포인터
    t_token *current = *token_list_head;

    while (current != NULL && current->right != NULL) {
        t_token *next_node = current->right;
        bool should_merge = false; // 병합 여부 플래그

        // 기본 병합 조건: 현재와 다음 노드가 모두 WORD이고, 현재 노드 뒤에 공백이 없었음
        if (current->token == WORD && next_node->token == WORD && !current->followed_by_whitespace) {
            should_merge = true; // 일단 병합한다고 가정

            // ★★★ 추가된 조건: 이전 토큰(prev)이 리다이렉션 연산자였다면 병합하지 않음 ★★★
            // 이것은 <file"arg" 같은 경우 file과 arg가 합쳐지는 것을 막기 위함입니다.
			/*
            if (prev != NULL &&
                (prev->token == REDIR_OPEN || prev->token == REDIR_WRITE ||
                 prev->token == REDIR_WRITE_A || prev->token == HEREDOC)) {
                should_merge = false; // 병합하지 않음으로 변경
                fprintf(stderr, "[DEBUG MERGE] PREV_IS_REDIR: Skipping merge for filename candidate [%s] with [%s]\n",
                        current->string ? current->string : "NULL", next_node->string ? next_node->string : "NULL");
                fflush(stderr);
            }
			*/
        }

        if (should_merge) {
            // fprintf(stderr, "[DEBUG MERGE] Merging [%s] with [%s]\n",
            //         current->string ? current->string : "NULL", next_node->string ? next_node->string : "NULL");
            // fflush(stderr);
            
            size_t current_len = current->string ? strlen(current->string) : 0;
            size_t next_len = next_node->string ? strlen(next_node->string) : 0;
            char *merged_string = malloc(current_len + next_len + 1);

            if (!merged_string) {
                perror("minishell: merge_word_tokens: malloc failed");
                return; 
            }
            merged_string[0] = '\0';
            if (current->string) strcpy(merged_string, current->string);
            if (next_node->string) strcat(merged_string, next_node->string);
            
            if (current->string) free(current->string);
            current->string = merged_string;
            current->followed_by_whitespace = next_node->followed_by_whitespace;

            current->right = next_node->right;
            if (current->right && current->right->parent == next_node) { // parent 포인터 사용 시
                 current->right->parent = current;
            }

            if (next_node->string) free(next_node->string);
            free(next_node);
            
            continue; 
        }
        // 병합하지 않으면 prev와 current를 다음으로 이동
        //prev = current;
        current = current->right;
    }
}

/*
void merge_word_tokens(t_token **token_list_head) {
    if (!token_list_head || !*token_list_head) {
        return;
    }
    t_token *prev = NULL;
    t_token *current = *token_list_head;

    while (current != NULL && current->right != NULL) {
        t_token *next_node = current->right;
        bool should_merge = false;

        if (current->token == WORD && next_node->token == WORD && !current->followed_by_whitespace) {
            should_merge = true;
            if (prev != NULL &&
                (prev->token == REDIR_OPEN || prev->token == REDIR_WRITE ||
                 prev->token == REDIR_WRITE_A || prev->token == HEREDOC)) {
                should_merge = false;
            }
        }

        if (should_merge) {
            size_t current_len = current->string ? strlen(current->string) : 0;
            size_t next_len = next_node->string ? strlen(next_node->string) : 0;
            char *merged_string = malloc(current_len + next_len + 1);
            if (!merged_string) {
                perror("minishell: merge_word_tokens: malloc failed"); return; 
            }
            merged_string[0] = '\0'; 
            if (current->string) strcpy(merged_string, current->string);
            if (next_node->string) strcat(merged_string, next_node->string);
            
            if (current->string) free(current->string);
            current->string = merged_string;
            current->followed_by_whitespace = next_node->followed_by_whitespace;
            current->right = next_node->right;
            // parent 포인터 업데이트는 AST 빌드 시 수행되므로 여기서는 불필요

            if (next_node->string) free(next_node->string);
            free(next_node);
            continue; 
        }
        prev = current;
        current = current->right;
    }
}
*/

int ft_count_number_of_arg(char **split)
{
    int i;

    i = 1;
    while (split[i] && ft_get_token(split[i])  == WORD)
        i++;
    return(i - 1);
}

char *ft_join_cmd_and_arg(char **split)
{
    char *new_str;
    int i;

    new_str = ft_strdup(split[0]);
    i = 1;
    while (split[i] && ft_get_token(split[i])  == WORD)
    {
        new_str = ft_strjoin(new_str, " ");
        new_str = ft_strjoin(new_str, split[i]);
        i++;
    }
    return(new_str);
}

const char *get_token_type_string(int token_type) 
{
    switch (token_type) 
	{
        case 0: return "STRING";
        case 1: return "CMD";
        case 4: return "PIPE";
        case 5: return "INPUT_REDIRECT";
        case 6: return "OUTPUT_REDIRECT";
        case 7: return "APPEND_REDIRECT";
        case 10: return "HEREDOC (<<)";
        case 12: return "BUILTIN";
        default: return "UNKNOWN";
    }
}

void print_ast(t_token *node, int level) 
{
    if (node == NULL) {
        return;
    }

    for (int i = 0; i < level - 1; i++) {
        printf("|   ");
    }

    if (level > 0) {
        printf("|-- ");
    }

    printf("%s", get_token_type_string(node->token));
    if (node->string) {
        printf(" (%s)", node->string);
    }
    printf("\n");

    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
}

void print_ast_start(t_token *root) 
{
    print_ast(root, 0);
}

int ft_delete_token_lst(t_token **token_lst)
{
    t_token *tmp;
    t_token *node;


    while (*token_lst)
    {
        tmp = (*token_lst)->right;
        node = *token_lst;
        if(node->string)
        {
            free(node->string);
            node->string = NULL;
        }
		
        free(node);
        *token_lst = tmp;
    }
    free(token_lst);
    token_lst = NULL;
    return(1);
    
}

t_token *ft_new_token_node(char *str, int token)
{
    t_token *token_node;

	//fprintf(stderr, "[DEBUG] Attempting malloc for t_token\n"); // DEBUG
    token_node = malloc(sizeof(t_token));
    if(!token_node)
	{	
		perror("[ERROR] malloc failed for t_token"); // System error + message
        return(NULL);
	}
	//fprintf(stderr, "[DEBUG] t_token malloc OK: %p\n", (void*)token_node); // DEBUG

	//Debug    
	token_node->string = NULL; // Initialize before potential failure
    if (str) { // Only strdup if str is not NULL
        //fprintf(stderr, "[DEBUG] Attempting ft_strdup for string: [%s]\n", str); // DEBUG
         token_node->string = ft_strdup(str); // Assuming ft_strdup uses malloc
         if (!token_node->string) {
             perror("[ERROR] ft_strdup failed for token string");
             free(token_node); // IMPORTANT: Free the allocated node
             return (NULL);    // Check IMMEDIATELY
         }
         //fprintf(stderr, "[DEBUG] ft_strdup OK: %p\n", (void*)token_node->string); // DEBUG
    }
	//End of Debug
    token_node->token = token;
	token_node->followed_by_whitespace = false;
    token_node->asso = ft_get_associativity(token);
    token_node->precedence = ft_get_precedence(token);
	token_node->heredoc_pipe_fd = -1; // Initialize FD as invalid
	token_node->heredoc_state = HD_NOT_PROCESSED; // Initialize state
    token_node->left = NULL;
    token_node->right = NULL;
	token_node->parent = NULL;
    return(token_node);
}

// Adding to the beginning is important for correct override behavior later.

void add_redirection_to_list(t_redir **list, int type, const char *filename_or_delimiter, t_token *heredoc_node) 
{
    t_redir *new_redir = malloc(sizeof(t_redir));
    if (!new_redir) {
        perror("malloc failed in add_redirection_to_list");
        return; // Or handle error
    }
    new_redir->type = type;
    new_redir->filename = filename_or_delimiter ? strdup(filename_or_delimiter) : NULL;
    if (filename_or_delimiter && !new_redir->filename) {
         perror("strdup failed in add_redirection_to_list");
         free(new_redir);
         return; // Or handle error
    }
    new_redir->heredoc_node = heredoc_node; // Store pointer to HEREDOC node
    new_redir->next = *list;
    *list = new_redir;
}

static int apply_redir_open(t_redir *redir_item)
{
    int fd_opened; // 변수 이름 충돌 방지를 위해 fd -> fd_opened 로 변경
    int target_fd = STDIN_FILENO;
    int dup2_result;

    // 입력 값 유효성 검사
    if (!redir_item || !redir_item->filename) {
        //fprintf(stderr, "[MINISHELL_DEBUG apply_redir_open] Error: Redirection item or filename is NULL.\n");
        //fflush(stderr);
        return (-1);
    }

    //fprintf(stderr, "[MINISHELL_DEBUG apply_redir_open] Attempting to open file for input: [%s]\n", redir_item->filename);
    //fflush(stderr); // 버퍼를 비워 바로 출력되도록 함

    fd_opened = open(redir_item->filename, O_RDONLY);

    //fprintf(stderr, "[MINISHELL_DEBUG apply_redir_open] open() for file [%s] returned fd: %d\n", redir_item->filename, fd_opened);
    //fflush(stderr);

    if (fd_opened == -1) {
        // open 실패 시, perror 대신 strerror_r이나 직접 에러 메시지 구성이 더 안전할 수 있지만,
        // 디버깅 목적이므로 perror 또는 strerror 사용
        //fprintf(stderr, "[MINISHELL_DEBUG apply_redir_open] Error opening file [%s]: %s (errno: %d)\n", redir_item->filename, strerror(errno), errno);
        //fflush(stderr);
        // perror(redir_item->filename); // 이 라인도 유용합니다.
        return (-1);
    }

    //fprintf(stderr, "[MINISHELL_DEBUG apply_redir_open] Attempting dup2(opened_fd=%d, target_fd=%d)\n", fd_opened, target_fd);
    //fflush(stderr);

    dup2_result = dup2(fd_opened, target_fd);

    //fprintf(stderr, "[MINISHELL_DEBUG apply_redir_open] dup2() returned: %d\n", dup2_result);
    //fflush(stderr);

    if (dup2_result == -1) {
        //fprintf(stderr, "[MINISHELL_DEBUG apply_redir_open] Error in dup2 for fd %d to STDIN_FILENO: %s (errno: %d)\n", fd_opened, strerror(errno), errno);
        //fflush(stderr);
        // perror("dup2 < failed");
        close(fd_opened); // 에러 시 열었던 fd는 닫아줍니다.
        return (-1);
    }

    // dup2 성공 후 원래 fd 닫기
    if (close(fd_opened) == -1) {
        //fprintf(stderr, "[MINISHELL_DEBUG apply_redir_open] Error closing original fd %d after dup2: %s (errno: %d)\n", fd_opened, strerror(errno), errno);
        //fflush(stderr);
        // perror("close fd after dup2 <");
        // dup2는 성공했으므로 계속 진행할 수 있지만, 에러는 기록합니다.
    }

    //fprintf(stderr, "[MINISHELL_DEBUG apply_redir_open] Successfully applied input redirection for [%s]\n", redir_item->filename);
    //fflush(stderr);
    return (0); // Success
}

// Helper for Output Redirection (>)
// Helper for Output Redirection (>)
static int apply_redir_write(t_redir *redir_item)
{
	int fd_opened; // 변수 이름 변경 (가독성 및 충돌 방지)
    int target_fd = STDOUT_FILENO;
    mode_t mode = 0644;
    int dup2_result;

    if (!redir_item || !redir_item->filename) {
        //fprintf(stderr, "[PID %d DEBUG apply_redir_write] Error: Redirection item or filename is NULL.\n", getpid());
        //fflush(stderr);
        return (-1);
    }

    // ▼▼▼ open() 호출 전 파일 이름 출력 추가 ▼▼▼
    //fprintf(stderr, "[PID %d DEBUG apply_redir_write] Attempting to open/create file for output: [%s]\n", getpid(), redir_item->filename);
    //fflush(stderr);
    // ▲▲▲ ▲▲▲ ▲▲▲

    fd_opened = open(redir_item->filename, O_WRONLY | O_CREAT | O_TRUNC, mode);
    // 사용자의 기존 디버그 프린트 유지 (PID 추가)
    //fprintf(stderr, "[PID %d DEBUG apply_redir_write] open returned fd=%d for filename [%s]\n", getpid(), fd_opened, redir_item->filename);
    //fflush(stderr); 

    if (fd_opened == -1) {
        // perror 대신 strerror_r 또는 직접 메시지 구성이 스레드 안전성 측면에서 더 나을 수 있으나,
        // 현재 디버깅 목적이므로 perror 또는 strerror 사용.
        //fprintf(stderr, "[PID %d DEBUG apply_redir_write] Error opening/creating file [%s]: %s (errno: %d)\n", 
        //        getpid(), redir_item->filename, strerror(errno), errno);
        //fflush(stderr);
        // perror(redir_item->filename); // 이 라인도 유용합니다. 만약 위의 fprintf가 충분하지 않다면 사용하세요.
        return (-1);
    }

    //fprintf(stderr, "[PID %d DEBUG apply_redir_write] Calling dup2(opened_fd=%d, target_fd=%d)\n", getpid(), fd_opened, target_fd); 
    //fflush(stderr); 

    dup2_result = dup2(fd_opened, target_fd);
    // dup2 결과도 fprintf로 출력하면 좋습니다.
    //fprintf(stderr, "[PID %d DEBUG apply_redir_write] dup2 returned: %d\n", getpid(), dup2_result);
    //fflush(stderr);

    if (dup2_result == -1) {
        //fprintf(stderr, "[PID %d DEBUG apply_redir_write] dup2 > failed in helper for fd %d: %s (errno: %d)\n", 
                //getpid(), fd_opened, strerror(errno), errno);
        //fflush(stderr);
        // perror("dup2 > failed in helper"); 
        close(fd_opened);
        return (-1);
    }
    //fprintf(stderr, "[PID %d DEBUG apply_redir_write] dup2 STDOUT successful.\n", getpid()); 
    //fflush(stderr);

    //fprintf(stderr, "[PID %d DEBUG apply_redir_write] Closing original fd %d\n", getpid(), fd_opened); 
    //fflush(stderr);
    if (close(fd_opened) == -1) {
        //fprintf(stderr, "[PID %d DEBUG apply_redir_write] Error closing original fd %d after dup2: %s (errno: %d)\n", 
          //      getpid(), fd_opened, strerror(errno), errno);
        //fflush(stderr);
        // perror("close fd after dup2 > in helper");
    }
    //fprintf(stderr, "[PID %d DEBUG apply_redir_write] Exiting apply_redir_write successfully.\n", getpid()); 
    //fflush(stderr);
    return (0); // Success
}

// Helper for Append Output Redirection (>>)
static int apply_redir_append(t_redir *redir_item)
{
    int fd;
    int target_fd = STDOUT_FILENO;
    mode_t mode = 0644;

    fd = open(redir_item->filename, O_WRONLY | O_CREAT | O_APPEND, mode);
    if (fd == -1) {
        perror(redir_item->filename);
        return (-1);
    }
    if (dup2(fd, target_fd) == -1) {
        perror("dup2 >> failed");
        close(fd);
        return (-1);
    }
    if (close(fd) == -1) {
        perror("close fd after dup2 >>");
        // Continue even if close fails
    }
    return (0); // Success
}

// Helper for Heredoc Redirection (<<)
static int apply_redir_heredoc(t_redir *redir_item)
{
    int heredoc_fd_to_dup;
    int target_fd = STDIN_FILENO;

    // Check state and FD from preprocessing
    if (!redir_item->heredoc_node ||
        redir_item->heredoc_node->heredoc_state != HD_PROCESSED_OK ||
        redir_item->heredoc_node->heredoc_pipe_fd < 0)
    {
        // Error message should be printed before calling if possible,
        // but print a generic one here if needed.
        //fprintf(stderr, "minishell: internal error - heredoc not ready in apply_redirections\n");
        return (-1);
    }
    heredoc_fd_to_dup = redir_item->heredoc_node->heredoc_pipe_fd;
    if (dup2(heredoc_fd_to_dup, target_fd) == -1) {
        perror("dup2 stdin heredoc failed");
        return (-1);
    }
    // DO NOT close heredoc_fd_to_dup here
    return (0); // Success
}

/**
 * @brief Applies all redirections from a list sequentially.
 * Uses helper functions for each redirection type.
 * IMPORTANT: Assumes the list is in the correct order for application
 * (i.e., does NOT reverse the list internally).
 *
 * @param list Pointer to the first node of the t_redir list.
 * @return int 0 on success, -1 on failure.
 */
int apply_redirections(t_redir *list)
{
    t_redir *current = list;
    int      status = 0; // Track status from helpers

    while (current && status == 0) // Stop loop on first error
    {
        switch (current->type)
        {
            case REDIR_OPEN: // <
                status = apply_redir_open(current);
                break;
            case REDIR_WRITE: // >
                status = apply_redir_write(current);
                break;
            case REDIR_WRITE_A: // >>
                status = apply_redir_append(current);
                break;
            case HEREDOC: // <<
                status = apply_redir_heredoc(current);
                break;
            default:
                // Optional: Handle unknown type? Or ignore?
                //fprintf(stderr, "minishell: unknown redirection type %d\n", current->type);
                // status = -1; // Treat unknown as error?
                break;
        }
        current = current->next;
    }
    // Return -1 if any helper function failed, otherwise 0
    return (status);
}

// --- Helper Function: Free the redirection list ---
void free_redir_list(t_redir *list) {
    t_redir *current = list;
    t_redir *next;
    while (current) {
        next = current->next;
        // free(current->filename); // Only if you used strdup earlier
        free(current);
        current = next;
    }
} 

void ft_add_back_node(t_token **lst, t_token *node)
{
    t_token *first_node;
    
    if(!lst)
        return ;
    if(!(*lst))
        *lst = node;
    else
    {
        first_node = *lst;
        while (first_node->right)
            first_node = first_node->right;
        first_node->right = node;
    }
}


// --- Helper Implementations (Basic Examples) ---

static bool is_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

static bool is_operator_start(char c) {
    return (c == '|' || c == '<' || c == '>');
}

// Helper to create token, add to list, and advance index
// NOTE: Assumes ft_new_token_node duplicates the substring
/*
static t_token *add_and_advance(t_token **list_head, char *str_segment, size_t len, int token_type, size_t *index)
{
    t_token *new_node = ft_new_token_node(str_segment, token_type);
    if (!new_node) {
        //fprintf(stderr, "minishell: memory allocation error in add_and_advance\n");
        return NULL; // Signal error
    }
    ft_add_back_node(list_head, new_node);
    *index += len;
    return new_node; // Return last added node (might not be needed)
}
*/
// Placeholder: You need a robust way to get token type from the string value
// This is simplified and likely insufficient for differentiating CMD/BUILTIN/WORD reliably here.
// Inside tokenize.c

// (Keep the rest of your new ft_tokenize function)

static int ft_get_token_type_from_str(const char *str) {
    if (!str) return WORD; // Should not happen with valid tokens

    // --- Check Operators First ---
    if (strcmp(str, "|") == 0) return PIPE;         // Type 4
    if (strcmp(str, "<") == 0) return REDIR_OPEN;    // Type 5
    if (strcmp(str, ">") == 0) return REDIR_WRITE;   // Type 6
    if (strcmp(str, ">>") == 0) return REDIR_WRITE_A;// Type 7
    if (strcmp(str, "<<") == 0) return HEREDOC;     // Type 10

    // --- Check Builtins ---
    // Add all your built-in commands here
    if (strcmp(str, "echo") == 0 ||
        strcmp(str, "cd") == 0 ||
        strcmp(str, "pwd") == 0 ||
        strcmp(str, "export") == 0 ||
        strcmp(str, "unset") == 0 ||
        strcmp(str, "env") == 0 ||
        strcmp(str, "exit") == 0)
    {
        return BUILTIN; // Type 12
    }

    // --- Default ---
    // If it's not an operator and not a known builtin,
    // it's either an external command or an argument.
    // Classify as WORD for now. The AST builder can potentially
    // reclassify the first WORD after a pipe as CMD if needed,
    // or you can attempt path/external command checks here if desired.
    return WORD; // Type 0 (or STRING)
}

// (The rest of your new ft_tokenize function...)

// Simplified error cleanup
static void free_token_list_on_error(t_token *list) {
    t_token *current = list;
    t_token *next;
    while (current) {
        next = current->right; // Assuming right is the 'next' pointer in the list
        if (current->string) free(current->string);
        free(current);
        current = next;
    }
}


// --- The New ft_tokenize ---
t_token *ft_tokenize(char *str) {
    t_token *token_list = NULL;
    size_t i = 0;
    size_t len;
    // size_t token_start; // 루프 내에서 필요시 정의

    if (!str) {
         return NULL;
    }
    len = strlen(str);

    while (i < len) {
        size_t token_start;
        t_token *new_node = NULL; // ★★★ 이번 반복에서 생성될 토큰을 가리킬 포인터, 루프 시작 시 NULL로 초기화

        // 1. 공백 건너뛰기
        while (i < len && is_whitespace(str[i])) {
            i++;
        }
        if (i >= len) {
             break;
        }

        token_start = i; // 실제 토큰 시작 위치

        // 2. 토큰 유형에 따른 처리 (연산자, 따옴표, 일반 단어)
        if (is_operator_start(str[i])) {
            // ... 연산자 토큰 처리 ...
            // 이 블록 내에서 new_node = ft_new_token_node(...); 등으로 설정
            // i 값도 연산자 길이만큼 업데이트
            // 예시:
            // char op_str[3] = {0}; // 실제 연산자 문자열
            // int type; // 실제 연산자 타입
            // size_t op_len; // 실제 연산자 길이
            // (op_str, type, op_len 설정 로직)
            // new_node = ft_new_token_node(op_str, type);
            // if (!new_node) { /* 에러 처리 및 반환 */ }
            // ft_add_back_node(&token_list, new_node);
            // i += op_len;
            // (기존 코드에서 이 부분을 new_node를 설정하도록 수정)

            // 현재 코드의 add_and_advance를 사용하거나, 아래처럼 직접 처리
            int type;
            size_t op_len = 1; // 기본 연산자 길이
            char op_str[3] = {0}; // 최대 연산자 길이 + 널 문자 고려 (예: ">>")
            op_str[0] = str[i];

            if (i + 1 < len) {
                if (str[i] == '>' && str[i+1] == '>') { type = REDIR_WRITE_A; op_len = 2; op_str[1] = '>'; }
                else if (str[i] == '<' && str[i+1] == '<') { type = HEREDOC; op_len = 2; op_str[1] = '<'; }
                else if (str[i] == '>') { type = REDIR_WRITE; }
                else if (str[i] == '<') { type = REDIR_OPEN; }
                else if (str[i] == '|') { type = PIPE; } // 파이프 연산자도 여기서 처리
                else { /* 예상치 못한 연산자 시작 문자 처리 (오류 또는 단일 문자 연산자로 간주) */ type = WORD; /* 또는 다른 기본값 */ } // 이 부분은 기존 로직에 맞게 조정
            } else { // 문자열의 마지막 문자일 경우
                if (str[i] == '>') type = REDIR_WRITE;
                else if (str[i] == '<') type = REDIR_OPEN;
                else if (str[i] == '|') type = PIPE;
                else { type = WORD; } // 이 부분은 기존 로직에 맞게 조정
            }
            // op_str이 실제로 채워졌는지 확인 (예: op_str[1]이 설정된 경우)
            // 여기서는 op_len 기준으로 new_node 생성

            new_node = ft_new_token_node(op_str, type); // op_str은 실제 연산자 문자열 ("<", ">>" 등)
            if (!new_node) { free_token_list_on_error(token_list); return NULL; }
            ft_add_back_node(&token_list, new_node);
            i += op_len; // 연산자 길이만큼 i 증가


        } else if (str[i] == '"' || str[i] == '\'') {
            // ... 따옴표로 묶인 토큰 처리 ...
            // 이 블록 내에서 new_node = ft_new_token_node(...); 등으로 설정
            // i 값도 닫는 따옴표 다음으로 업데이트
            // (기존 코드에서 이 부분을 new_node를 설정하도록 수정)
            char quote_char = str[i];
            size_t j = i + 1;
            while (j < len && str[j] != quote_char) {
                j++;
            }
            if (j >= len) { /* 미종료 따옴표 오류 처리 */ free_token_list_on_error(token_list); return NULL; }

            size_t content_start = token_start + 1;
            size_t content_len = j - content_start;
            char *content_str = ft_substr(str, content_start, content_len);
            if (!content_str) { /* 메모리 할당 오류 처리 */ free_token_list_on_error(token_list); return NULL; }

            new_node = ft_new_token_node(content_str, WORD); // 따옴표 안의 내용은 WORD
            free(content_str);
            if (!new_node) { /* 메모리 할당 오류 처리 */ free_token_list_on_error(token_list); return NULL; }
            ft_add_back_node(&token_list, new_node);
            i = j + 1; // 닫는 따옴표 다음으로 i 이동

        } else { // 일반 단어
            // ... 일반 단어 토큰 처리 ...
            // 이 블록 내에서 new_node = ft_new_token_node(...); 등으로 설정
            // i 값도 단어의 끝 다음으로 업데이트
            // (기존 코드에서 이 부분을 new_node를 설정하도록 수정)
            size_t j = i;
            while (j < len && !is_whitespace(str[j]) && !is_operator_start(str[j]) && str[j] != '"' && str[j] != '\'') {
                j++;
            }
            size_t token_len = j - token_start;
            // if (token_len == 0) { i = j; continue; } // 0 길이 토큰은 스킵 (이론상 발생 안 함)

            char *token_str = ft_substr(str, token_start, token_len);
            if (!token_str) { /* 메모리 할당 오류 */ free_token_list_on_error(token_list); return NULL; }

            int type = ft_get_token_type_from_str(token_str);
            new_node = ft_new_token_node(token_str, type);
            free(token_str);
            if (!new_node) { /* 메모리 할당 오류 */ free_token_list_on_error(token_list); return NULL; }
            ft_add_back_node(&token_list, new_node);
            i = j; // 단어의 끝 다음으로 i 이동
        }

        // ★★★ 이 부분이 핵심 ★★★
        // 위 if/else if/else 블록 중 하나에서 new_node가 성공적으로 생성되었다면,
        // (그리고 i가 다음 처리 시작 위치로 업데이트 되었다면)
        // 그 new_node에 대해 followed_by_whitespace 플래그를 설정합니다.
        if (new_node) { // new_node가 NULL이 아닌 경우 (즉, 토큰이 성공적으로 생성된 경우)
            if (i < len && is_whitespace(str[i])) { // 현재 i 위치 (토큰 바로 다음 문자)가 공백인지 확인
                new_node->followed_by_whitespace = true;
            } else {
                new_node->followed_by_whitespace = false;
            }
        }
        // (디버깅 프린트는 필요시 여기에 둘 수 있습니다)

    } // end while (i < len)

    return token_list;
}

t_token *ft_create_ast(t_token *token_list_head) {
    t_token *ast_root = NULL; // 전체 AST의 최종 루트
    t_token *current_command_segment = NULL; // 현재 구축 중인 간단한 명령어 또는 리다이렉션 묶음
    t_token *last_processed_command_for_args = NULL; // 인자가 붙어야 할 실제 명령어 노드

    t_token *flat_iterator = token_list_head;

    while (flat_iterator) {
        t_token *current_node = flat_iterator;
        flat_iterator = flat_iterator->right;

        // 현재 토큰을 플랫 리스트에서 분리 (AST에 편입 준비)
        current_node->left = NULL;
        current_node->right = NULL;
        current_node->parent = NULL;

        int token_type = current_node->token;

        // 1. 명령어 또는 첫 단어 (인자 시작 가능성)
        if (token_type == CMD || token_type == BUILTIN || token_type == WORD || token_type == VAR) {
            if (!current_command_segment) { // 새로운 명령어 세그먼트 시작
                current_command_segment = current_node;
                last_processed_command_for_args = current_node;
            } else if (last_processed_command_for_args) { // 기존 명령어에 인자로 추가
                t_token *attach_point = find_last_node_in_simple_cmd(last_processed_command_for_args);
                attach_point->right = current_node;
                current_node->parent = attach_point;
            } else { // current_command_segment는 있지만 last_processed_command_for_args가 NULL인 경우 (예: 리다이렉션만 있었던 경우)
                fprintf(stderr, "minishell: syntax error near token `%s` (misplaced argument without command)\n", current_node->string);
                free_ast_recursive(ast_root); free_single_token_node_content(current_node); return NULL;
            }
        }
        // 2. 리다이렉션 연산자
        else if (token_type == REDIR_OPEN || token_type == REDIR_WRITE ||
                 token_type == REDIR_WRITE_A || token_type == HEREDOC) {
            t_token *operator_node = current_node;

            if (!flat_iterator) { // 파일 이름 없음
                fprintf(stderr, "minishell: syntax error near `%s' (missing filename)\n", operator_node->string);
                free_ast_recursive(ast_root); free_single_token_node_content(operator_node); return NULL;
            }



			
            t_token *filename_node = flat_iterator;
            flat_iterator = flat_iterator->right; // 파일 이름 토큰 소비

            filename_node->left = NULL; filename_node->right = NULL; filename_node->parent = operator_node;
            if (filename_node->token != WORD) { // 파일명은 WORD여야 함
                fprintf(stderr, "minishell: syntax error near `%s' (expected filename for `%s')\n", filename_node->string, operator_node->string);
                free_ast_recursive(ast_root); free_single_token_node_content(operator_node); free_single_token_node_content(filename_node); return NULL;
            }
            operator_node->right = filename_node;

            // 리다이렉션은 현재까지의 명령어 세그먼트(current_command_segment)에 적용됨
            operator_node->left = current_command_segment;
            if (current_command_segment) {
                current_command_segment->parent = operator_node;
            }
            current_command_segment = operator_node; // 리다이렉션이 현재 세그먼트의 루트가 됨
            // 인자는 여전히 원래 명령어(operator_node->left)에 붙어야 하므로 last_processed_command_for_args는 유지 (또는 operator_node->left로 설정)
            if (operator_node->left && (operator_node->left->token == CMD || operator_node->left->token == BUILTIN || operator_node->left->token == WORD || operator_node->left->token == VAR)) {
                last_processed_command_for_args = operator_node->left;
            } // else last_processed_command_for_args는 NULL이거나 이전 값 유지 (이 부분은 좀 더 정교화 필요)
        }
        // 3. 파이프 연산자
        else if (token_type == PIPE) {
            t_token *pipe_node = current_node;
            if (!current_command_segment) { // 파이프 왼쪽에 아무것도 없음
                fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
                free_ast_recursive(ast_root); free_single_token_node_content(pipe_node); return NULL;
            }
            pipe_node->left = current_command_segment; // 현재까지의 명령어 세그먼트를 파이프의 왼쪽 자식으로
            if (current_command_segment) {
                current_command_segment->parent = pipe_node;
            }
            ast_root = pipe_node; // 파이프가 새로운 전체 AST의 루트가 됨 (이전 ast_root는 이제 pipe_node->left)
            current_command_segment = NULL; // 파이프 오른쪽은 새로운 명령어 세그먼트가 됨
            last_processed_command_for_args = NULL;
        } else {
            fprintf(stderr, "minishell: syntax error: unexpected token type %d [%s]\n", token_type, current_node->string);
            free_ast_recursive(ast_root); free_single_token_node_content(current_node);
            // flat_iterator를 통해 남은 토큰들도 해제해야 함
            free_token_list_on_error(flat_iterator); 
            return NULL;
        }
    }

    // 루프가 끝나면, 마지막으로 구성된 current_command_segment가 전체 AST의 루트이거나,
    // PIPE가 루트였다면 PIPE의 오른쪽 자식이 되어야 함.
    // 현재 로직은 마지막 파이프가 전체 루트가 되도록 함.
    if (ast_root && ast_root->token == PIPE && ast_root->right == NULL) {
        if (current_command_segment) { // 파이프 오른쪽에 명령어가 있었던 경우
            ast_root->right = current_command_segment;
            current_command_segment->parent = ast_root;
        } else { // 파이프 오른쪽에 아무것도 없는 경우 (예: cmd | )
            fprintf(stderr, "minishell: syntax error: expected command after `|'\n");
            free_ast_recursive(ast_root); return NULL;
        }
    } else if (!ast_root && current_command_segment) { // 파이프 없이 단일 명령어 세그먼트만 있었던 경우
        ast_root = current_command_segment;
    } else if (ast_root && ast_root->token != PIPE && current_command_segment && ast_root != current_command_segment) {
        // 이 경우는 보통 발생하지 않아야 함. ast_root가 이미 설정되었는데 current_command_segment도 있는 경우.
        fprintf(stderr, "minishell: AST construction: Unhandled final segment.\n");
        free_ast_recursive(ast_root); free_ast_recursive(current_command_segment); return NULL;
    }
    
    return ast_root;
}


t_token *ft_parse(char *str)
{
    // fprintf(stderr, "[ft_parse DEBUG] Calling ft_tokenize for: [%s]\n", str); fflush(stderr); 
    t_token *token_list_head = ft_tokenize(str); 
    
    if (!token_list_head) {
        return NULL;
    }
    // ft_parse에서는 _tokens 대신 merge_word_tokens 호출
    merge_word_tokens(&token_list_head); 
    
    t_token *ast_root = ft_create_ast(token_list_head); 
    
    if (!ast_root && token_list_head) { 
        // ft_create_ast가 실패하고 NULL을 반환했을 때, ft_tokenize로 생성된 원본 token_list_head가 아직 해제되지 않았을 수 있음
        // ft_create_ast 내부에서 에러 발생 시 소비한 토큰과 소비하지 않은 토큰 모두를 책임지고 해제하는 것이 이상적.
        // 여기서는 ft_create_ast가 NULL을 반환하면, 남은 token_list_head라도 해제 시도.
        free_token_list_on_error(token_list_head); 
    }
    return ast_root;
}