// 파일: srcs/parser/tokenizer/tokenize.c
#include "tokenize.h"
#include <assert.h>
#include <errno.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>  

// --- Helper Function Declarations ---
static bool is_whitespace(char c);
static bool is_operator_char(char c); 
static void free_token_list(t_token *list); 
//static void free_ast_recursive(t_token *node);
static void free_single_token_node_content_and_node(t_token *node); 
//static t_token *find_last_node_in_simple_cmd(t_token *command_node); // 이제 사용됩니다.
static int apply_redir_open(t_redir *redir_item);
static int apply_redir_write(t_redir *redir_item);
static int apply_redir_append(t_redir *redir_item);
static int apply_redir_heredoc(t_redir *redir_item);

// --- Helper Function Definitions ---
static bool is_whitespace(char c) { return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r'); }
static bool is_operator_char(char c) { return (c == '|' || c == '<' || c == '>'); }




/*
static char *append_to_buffer(char *buffer, const char *piece) {
    char *new_buffer = ft_strjoin(buffer, piece);
    free(buffer);
    return new_buffer;
}
*/

// static void free_ast_recursive(t_token *node) { 
//     if (!node) return;
//     if (node->left) free_ast_recursive(node->left);
//     node->left = NULL;
//     if (node->right) free_ast_recursive(node->right);
//     node->right = NULL;
//     if (node->string) free(node->string);
//     node->string = NULL;
//     free(node);
// }

static void free_single_token_node_content_and_node(t_token *node) { 
    if (!node) return;
    if (node->string) free(node->string);
    node->string = NULL;
    free(node);
}

static void free_token_list(t_token *list_head) { 
    t_token *current = list_head; t_token *next;
    while (current) {
        next = current->right; 
        free_single_token_node_content_and_node(current);
        current = next;
    }
}

// find_last_node_in_simple_cmd: 이 함수는 ft_create_ast에서 사용됩니다.
// static t_token *find_last_node_in_simple_cmd(t_token *command_node) { 
//     if (!command_node) return NULL;
//     t_token *current = command_node;
//     // CMD, BUILTIN, WORD 토큰의 ->right 체인을 따라가며 마지막 인수를 찾습니다.
//     // VAR 토큰도 인수로 간주될 수 있다면 조건에 추가합니다.
//     while (current->right && 
//            (current->right->token == WORD || 
//             current->right->token == ARG ||  // ARG 토큰 타입이 있다면
//             current->right->token == VAR)) { // VAR 토큰 타입이 인수로 올 수 있다면
//         current = current->right;
//     }
//     return current;
// }

// --- Redirection Helper Functions ---
static int apply_redir_open(t_redir *redir_item) {
    int fd_opened; int target_fd = STDIN_FILENO; int dup2_result;
    if (!redir_item || !redir_item->filename) return (-1);
    fd_opened = open(redir_item->filename, O_RDONLY);
    if (fd_opened == -1) { perror(redir_item->filename); return (-1); }
    dup2_result = dup2(fd_opened, target_fd);
    if (dup2_result == -1) { perror("dup2 STDIN_FILENO"); close(fd_opened); return (-1); }
    if (close(fd_opened) == -1) { perror(redir_item->filename); }
    return (0);
}
static int apply_redir_write(t_redir *redir_item) {
    int fd_opened; int target_fd = STDOUT_FILENO; mode_t mode = 0644; int dup2_result;
    if (!redir_item || !redir_item->filename) return (-1);
    fd_opened = open(redir_item->filename, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (fd_opened == -1) { perror(redir_item->filename); return (-1); }
    dup2_result = dup2(fd_opened, target_fd);
    if (dup2_result == -1) { perror("dup2 STDOUT_FILENO"); close(fd_opened); return (-1); }
    if (close(fd_opened) == -1) { perror(redir_item->filename); }
    return (0);
}
static int apply_redir_append(t_redir *redir_item) {
    int fd; int target_fd = STDOUT_FILENO; mode_t mode = 0644;
    if (!redir_item || !redir_item->filename) return (-1);
    fd = open(redir_item->filename, O_WRONLY | O_CREAT | O_APPEND, mode);
    if (fd == -1) { perror(redir_item->filename); return (-1); }
    if (dup2(fd, target_fd) == -1) { perror("dup2 STDOUT_FILENO (append)"); close(fd); return (-1); }
    if (close(fd) == -1) { perror(redir_item->filename); }
    return (0); 
}
static int apply_redir_heredoc(t_redir *redir_item) {
    int heredoc_fd_to_dup; int target_fd = STDIN_FILENO;
    if (!redir_item || !redir_item->heredoc_node || redir_item->heredoc_node->heredoc_state != HD_PROCESSED_OK || redir_item->heredoc_node->heredoc_pipe_fd < 0) {
        fprintf(stderr, "minishell: internal error or heredoc not ready for apply_redir_heredoc\n"); return (-1);
    }
    heredoc_fd_to_dup = redir_item->heredoc_node->heredoc_pipe_fd;
    if (dup2(heredoc_fd_to_dup, target_fd) == -1) { perror("dup2 STDIN_FILENO (heredoc)"); return (-1); }
    return (0); 
}

// --- Redirection List Handling Functions ---
void add_redirection_to_list(t_redir **list, int type, const char *filename_or_delimiter, t_token *heredoc_node) {
    t_redir *new_redir = (t_redir *)malloc(sizeof(t_redir));
    if (!new_redir) { perror("minishell: malloc in add_redirection_to_list"); return; }
    new_redir->type = type; new_redir->filename = NULL;
    if (filename_or_delimiter) {
        new_redir->filename = ft_strdup(filename_or_delimiter);
        if (!new_redir->filename && filename_or_delimiter[0] != '\0') { perror("minishell: ft_strdup in add_redirection_to_list"); free(new_redir); return; }
    }
    new_redir->heredoc_node = heredoc_node; new_redir->next = *list; *list = new_redir;
}
int apply_redirections(t_redir *list) {
    t_redir *current = list; int status = 0;
    while (current != NULL) {
        status = 0; 
        switch (current->type) {
            case REDIR_OPEN: status = apply_redir_open(current); break;
            case REDIR_WRITE: status = apply_redir_write(current); break;
            case REDIR_WRITE_A: status = apply_redir_append(current); break;
            case HEREDOC: status = apply_redir_heredoc(current); break;
            default: fprintf(stderr, "minishell: unknown redirection type %d\n", current->type); status = -1; break;
        }
        if (status == -1) return (-1); 
        current = current->next;
    }
    return (0);
}
void free_redir_list(t_redir *list) {
    t_redir *current = list; t_redir *next;
    while (current) {
        next = current->next;
        if(current->filename) free(current->filename);
        free(current); current = next;
    }
} 

// --- Tokenizer and Parser Utility Functions ---
const char *get_token_type_string(int token_type) { 
    if (token_type == WORD) return "WORD"; 
    if (token_type == CMD) return "CMD";
    if (token_type == ARG) return "ARG";
    if (token_type == PIPE) return "PIPE";
    if (token_type == REDIR_OPEN) return "INPUT_REDIRECT (<)"; 
    if (token_type == REDIR_WRITE) return "OUTPUT_REDIRECT (>)"; 
    if (token_type == REDIR_WRITE_A) return "APPEND_REDIRECT (>>)"; 
    if (token_type == DOUBLE_REDIR) return "DOUBLE_REDIR";
    if (token_type == HEREDOC) return "HEREDOC (<<)";
    if (token_type == VAR) return "VAR";
    if (token_type == BUILTIN) return "BUILTIN"; 
    return "UNKNOWN";
}

void print_ast(t_token *node, int level) {
    if (node == NULL) { return; }
    for (int i = 0; i < level - 1; i++) { printf("|   "); }
    if (level > 0) { printf("|-- "); }
    printf("%s", get_token_type_string(node->token));
    if (node->string) {
         printf(" (%s)", node->string);
    }
    printf("\n");
    print_ast(node->left, level + 1);     
    print_ast(node->right, level + 1);    
}

void print_ast_start(t_token *root) { print_ast(root, 0); }

int ft_delete_token_lst(t_token **token_lst) { 
    if (!token_lst) return 0;
    free_token_list(*token_lst);
    *token_lst = NULL; 
    return(1);
}

t_token *ft_new_token_node(char *str, int token) { 
    t_token *token_node = (t_token *)malloc(sizeof(t_token));
    if(!token_node) { perror("malloc in ft_new_token_node"); return(NULL); }
	token_node->string = NULL; 
    if (str) { 
        token_node->string = ft_strdup(str); 
        if (!token_node->string && str[0] != '\0') { 
             perror("ft_strdup in ft_new_token_node"); free(token_node); return (NULL);    
         }
    }
    token_node->token = token;
	token_node->followed_by_whitespace = false;
    token_node->asso = ft_get_associativity(token);
    token_node->precedence = ft_get_precedence(token);
	token_node->heredoc_pipe_fd = -1; 
	token_node->heredoc_state = HD_NOT_PROCESSED; 
    token_node->left = NULL; token_node->right = NULL; token_node->parent = NULL;
    token_node->single_quote = 0; token_node->double_quote = 0;
    return(token_node);
}

void ft_add_back_node(t_token **lst, t_token *node) { 
    if(!lst || !node) return;
    if(!*lst) *lst = node;
    else { t_token *curr = *lst; while(curr->right) curr = curr->right; curr->right = node; }
}

// --- Tokenizer ---

t_token *ft_tokenize(char *str) {
    t_token *token_list_head = NULL;
    size_t i = 0;
    size_t input_len;

    if (!str) return NULL;
    input_len = strlen(str);

    while (i < input_len) {
        while (i < input_len && is_whitespace(str[i])) { i++; }
        if (i >= input_len) break;

        t_token *new_node = NULL;
        // size_t token_start_idx = i; // Removed as it was unused in the new logic

        if (is_operator_char(str[i])) {
            char op_str[3] = {0}; // Increased size for ">>" or "<<" plus null terminator
            int type = WORD;
            size_t op_len = 1;
            op_str[0] = str[i];

            if (i + 1 < input_len) {
                if (str[i] == '>' && str[i+1] == '>') { type = REDIR_WRITE_A; op_len = 2; op_str[1] = '>'; }
                else if (str[i] == '<' && str[i+1] == '<') { type = HEREDOC; op_len = 2; op_str[1] = '<'; }
                // The original tokenizer also checked for '&>' for DOUBLE_REDIR. If you need that:
                // else if (str[i] == '&' && str[i+1] == '>') { type = DOUBLE_REDIR; op_len = 2; op_str[1] = '>'; }
            }
            
            if (op_len == 1) { // Single character operators
                 if (str[i] == '>') type = REDIR_WRITE;
                 else if (str[i] == '<') type = REDIR_OPEN;
                 else if (str[i] == '|') type = PIPE;
                 // else: it might be an operator char not defined (e.g. ';', '&' alone if not DOUBLE_REDIR)
                 // ft_get_token might handle this if op_str is passed to it.
                 // For clarity, explicit operator types are better here.
            }
            op_str[op_len] = '\0'; // Ensure null termination

            // If type is still WORD, it means it's an unrecognized operator here,
            // or we rely on ft_get_token. The original ft_get_token might be better for operators.
            // Let's keep it simple: if it's one of the known ops, use its type.
            if (type == WORD && op_len == 1) { // Fallback to ft_get_token if not explicitly typed above
                 type = ft_get_token(op_str);
            }


            new_node = ft_new_token_node(op_str, type);
            i += op_len;
        } else { // Handle words and concatenated quoted/unquoted segments
            char *current_arg_buffer = ft_strdup("");
            if (!current_arg_buffer) { free_token_list(token_list_head); return NULL; }

            // Flags to determine the nature of the concatenated token
            bool all_segments_single_quoted = true; // Assume true until a non-single-quoted segment is found
            bool all_segments_double_quoted = true; // Assume true until a non-double-quoted segment is found
            bool has_unquoted_segments = false;
            int segment_count = 0;

            while (i < input_len && !is_whitespace(str[i]) && !is_operator_char(str[i])) {
                char *piece_str = NULL;
                bool current_piece_is_single = false;
                bool current_piece_is_double = false;
                // size_t segment_start = i; // This was unused

                if (str[i] == '"' || str[i] == '\'') {
                    char quote_char = str[i];
                    current_piece_is_single = (quote_char == '\'');
                    current_piece_is_double = (quote_char == '"');
                    
                    i++; // Consume opening quote
                    size_t content_start = i;
                    while (i < input_len && str[i] != quote_char) { i++; }
                    if (i >= input_len) {
                        fprintf(stderr, "minishell: syntax error: unclosed quote %c\n", quote_char);
                        free(current_arg_buffer); free_token_list(token_list_head); return NULL;
                    }
                    piece_str = ft_substr(str, content_start, i - content_start);
                    i++; // Consume closing quote
                } else { // Unquoted segment
                    has_unquoted_segments = true;
                    size_t unquoted_start = i;
                    while (i < input_len && !is_whitespace(str[i]) && !is_operator_char(str[i]) && str[i] != '"' && str[i] != '\'') {
                        i++;
                    }
                    piece_str = ft_substr(str, unquoted_start, i - unquoted_start);
                }

                if (!piece_str) { free(current_arg_buffer); free_token_list(token_list_head); return NULL; }
                
                char *temp_buffer = ft_strjoin(current_arg_buffer, piece_str);
                free(current_arg_buffer);
                free(piece_str);
                current_arg_buffer = temp_buffer;
                if (!current_arg_buffer) { free_token_list(token_list_head); return NULL; }
                
                // Update overall quote flags for the concatenated token
                if (!current_piece_is_single) all_segments_single_quoted = false;
                if (!current_piece_is_double) all_segments_double_quoted = false;
                segment_count++;
            }
            
            if (ft_strlen(current_arg_buffer) > 0) {
                // Determine token type (CMD, BUILTIN, WORD, VAR) using the utils function
                new_node = ft_new_token_node(current_arg_buffer, ft_get_token(current_arg_buffer));
                if (new_node) {
                    // Set quote flags for the final token:
                    // If all concatenated segments were purely single-quoted (and no unquoted parts), it's a single_quote token.
                    // Else if all were purely double-quoted (and no unquoted parts), it's a double_quote token.
                    // Otherwise (mixed, or unquoted parts involved), it's neither.
                    if (segment_count > 0) { // Only if we actually processed segments
                        if (all_segments_single_quoted && !has_unquoted_segments && !all_segments_double_quoted) {
                            new_node->single_quote = 1;
                        } else if (all_segments_double_quoted && !has_unquoted_segments && !all_segments_single_quoted) {
                            new_node->double_quote = 1;
                        }
                        // If has_unquoted_segments is true, or if it's a mix of single and double,
                        // the quote flags remain 0 (default). This means it will be expanded
                        // as an unquoted string by `expand_all_variables`.
                    }
                }
            }
            free(current_arg_buffer); // Buffer is now part of new_node->string or was empty
        }

        if (!new_node) {
             if (i < input_len && !is_whitespace(str[i])) { // Error if something unhandled remains
                fprintf(stderr, "minishell: tokenizer error near '%.10s'\n", &str[i]);
                free_token_list(token_list_head); return NULL;
             }
             break; 
        }

        if (i < input_len && is_whitespace(str[i])) new_node->followed_by_whitespace = true;
        else new_node->followed_by_whitespace = false;

        ft_add_back_node(&token_list_head, new_node);
    }
    return token_list_head;
}

// --- AST Creation Function (수정된 버전) ---
// t_token *ft_create_ast(t_token *token_list_head) {
//     t_token *ast_root = NULL;
//     t_token *current_cmd_and_args_head = NULL; 
//     t_token *current_cmd_token = NULL;      // 실제 CMD/BUILTIN 토큰 (인수가 붙을 대상)
//     // t_token *current_cmd_last_arg = NULL; // find_last_node_in_simple_cmd로 대체 가능

//     t_token *current_segment_root = NULL;   // 현재 파이프 세그먼트의 루트 (명령어 또는 리다이렉션)

//     t_token *flat_iterator = token_list_head;
//     t_token *next_flat_token = NULL;

//     while (flat_iterator) {
// 		t_token *current_node = flat_iterator;
//         next_flat_token = flat_iterator->right;
// 	//	printf("%s\n", flat_iterator->string);

//         current_node->left = NULL;
//         current_node->right = NULL;
//         current_node->parent = NULL; 

//         int token_type = current_node->token;

// 		if (ast_root)
// 				printf("Ast: '%s'\n", ast_root->string);
// 		// if (ast_root->left)
// 		// 		printf("Ast left: '%d'\n", ast_root->left->token);

//         if (token_type == CMD || token_type == BUILTIN || token_type == WORD || token_type == VAR) {
//             if (!current_cmd_token) { // 새 명령어 또는 세그먼트의 첫 단어
//                 current_cmd_token = current_node; // 이 노드가 현재 명령어
//                 // current_cmd_last_arg = current_node; // 이제 find_last_node_in_simple_cmd 사용
//                 current_cmd_and_args_head = current_node; 
//                 if (!current_segment_root) { 
//                     current_segment_root = current_cmd_and_args_head;
//                 }
//             } else { // 현재 명령어(current_cmd_token)에 대한 인수
//                 // ****** find_last_node_in_simple_cmd 호출 사용 ******
//                 t_token *attach_point = find_last_node_in_simple_cmd(current_cmd_token);
//                 if (attach_point) { 
//                     attach_point->right = current_node; 
//                     current_node->parent = attach_point;
//                     // current_cmd_last_arg = current_node; // current_cmd_last_arg는 더 이상 직접 추적 안 함
//                 } else { 
//                      // current_cmd_token이 NULL이거나, 알 수 없는 이유로 attach_point를 찾지 못한 경우
//                      fprintf(stderr, "minishell: AST construction error - cannot find attach point for argument '%s'.\n", current_node->string ? current_node->string : "NULL_STR");
//                     free_ast_recursive(ast_root); free_ast_recursive(current_segment_root);
//                     free_single_token_node_content_and_node(current_node); // 오류 유발 노드
//                     if (current_cmd_token != current_node) free_single_token_node_content_and_node(current_cmd_token); // 만약 다르다면
//                     free_token_list(next_flat_token); // 나머지 토큰 리스트
//                     return NULL;
//                 }
//                 // ****** 수정 끝 ******
//             }
//         }
//         else if (token_type == REDIR_OPEN || token_type == REDIR_WRITE ||
//                  token_type == REDIR_WRITE_A || token_type == HEREDOC) {
//             t_token *operator_node = current_node;

//             if (!next_flat_token) { 
//                 fprintf(stderr, "minishell: syntax error near EOL after redirection '%s'\n", operator_node->string ? operator_node->string : "OP");
//                 free_ast_recursive(ast_root); free_ast_recursive(current_segment_root);
//                 free_single_token_node_content_and_node(operator_node);
//                 return NULL;
//             }
//             t_token *filename_node = next_flat_token;
//             next_flat_token = filename_node->right;
//             filename_node->left = NULL; filename_node->right = NULL; 
            
//             if (filename_node->token != WORD && filename_node->token != VAR) {
//                  fprintf(stderr, "minishell: syntax error: expected filename after redirection '%s', got '%s'\n", operator_node->string ? operator_node->string : "OP", filename_node->string ? filename_node->string : get_token_type_string(filename_node->token));
//                  free_ast_recursive(ast_root); free_ast_recursive(current_segment_root);
//                  free_single_token_node_content_and_node(operator_node); free_single_token_node_content_and_node(filename_node);
//                  free_token_list(next_flat_token);
//                  return NULL;
//             }
//             operator_node->right = filename_node;
//             filename_node->parent = operator_node;
            
//             operator_node->left = current_segment_root;
//             if (current_segment_root) {
//                 current_segment_root->parent = operator_node;
//             }
            
//             current_segment_root = operator_node; // 리다이렉션 노드가 현재 세그먼트의 루트가 됨.
            
//             // 리다이렉션 뒤에 오는 인수는 원래 명령어(current_cmd_token)에 붙어야 함.
//             // current_cmd_token은 operator_node->left 내부의 명령어 노드를 가리켜야 함.
//             // 만약 current_cmd_and_args_head가 NULL이었다면 (예: `> out cmd`), 
//             // current_cmd_token은 다음 CMD/WORD 토큰에서 설정될 것임.
//             if (!operator_node->left) { // 리다이렉션이 명령어 없이 시작된 경우 (> outfile cmd ...)
//                 current_cmd_token = NULL; // 다음 토큰이 명령어가 될 준비
//                 current_cmd_and_args_head = NULL; 
//             }
//             // current_cmd_token은 이미 operator_node->left (또는 그 내부의 명령어)를 가리키고 있어야 함.
//             // 이 부분은 current_cmd_token을 명시적으로 업데이트하지 않아도,
//             // 다음 WORD/CMD 토큰이 올 때 current_cmd_token (NULL이 아니라면)에 인수가 붙도록 로직이 되어 있음.
//         }
//         else if (token_type == PIPE) {
//             t_token *pipe_node = current_node;
//             if (!current_segment_root && !ast_root) { 
//                 fprintf(stderr, "minishell: syntax error: unexpected pipe\n");
//                 free_single_token_node_content_and_node(pipe_node); free_token_list(next_flat_token);
//                 return NULL;
//             }

//             // 현재 세그먼트 (current_segment_root)가 존재하면 파이프의 왼쪽 자식이 됨.
//             // 그렇지 않고 ast_root만 존재하면 (이전 파이프 체인), ast_root가 왼쪽 자식이 됨.
//             pipe_node->left = current_segment_root ? current_segment_root : ast_root;
//             if (pipe_node->left) pipe_node->left->parent = pipe_node;
            
// 			if (pipe_node)
// 				printf("New Pipe: '%s'\n", pipe_node->string);
// 			if (pipe_node->left)
// 				printf("New Pipe left: '%s'\n", pipe_node->left->string);

//             ast_root = pipe_node; // 이 파이프가 새로운 루트가 됨 (오른쪽은 다음 토큰들로 채워짐)


//             current_cmd_and_args_head = NULL; 
//             current_cmd_token = NULL;
//             // current_cmd_last_arg = NULL; // 더 이상 직접 사용 안 함
//             current_segment_root = NULL;
//         } else { 
//             fprintf(stderr, "minishell: syntax error: unknown token type %d (%s)\n", token_type, current_node->string ? current_node->string : "NULL_STR");
//             free_ast_recursive(ast_root); free_ast_recursive(current_segment_root);
//             free_single_token_node_content_and_node(current_node);
//             free_token_list(next_flat_token);
//             return NULL;
//         }
//         flat_iterator = next_flat_token;
//     } 

//     if (ast_root && ast_root->token == PIPE && ast_root->right == NULL) {
//         if (current_segment_root) { 
//             ast_root->right = current_segment_root;
//             if(current_segment_root) current_segment_root->parent = ast_root;
//         } else { 
//             fprintf(stderr, "minishell: syntax error: command expected after pipe\n");
//             free_ast_recursive(ast_root); return NULL;
//         }
//     } else if (!ast_root && current_segment_root) { 
//         ast_root = current_segment_root;
//     } else if (ast_root && current_segment_root && ast_root != current_segment_root && ast_root->right != current_segment_root) {
//         fprintf(stderr, "minishell: AST Error: dangling segment at end of parsing. AST Root: %s, Dangling: %s\n",
//                 ast_root ? get_token_type_string(ast_root->token) : "NULL",
//                 current_segment_root ? get_token_type_string(current_segment_root->token) : "NULL");
//         free_ast_recursive(ast_root); free_ast_recursive(current_segment_root);
//         return NULL;
//     }
    
//     if (!ast_root && !current_segment_root && token_list_head && token_list_head->token != 0 && token_list_head->string != NULL) { 
//         // If token_list_head was valid but ast_root is null, an error should have been caught and returned NULL.
//         // This might indicate an empty command after processing, or an unhandled error state.
//     }
    
//     return ast_root;
// }
/*
static t_token *_argument(t_token *command_node) {
    if (!command_node) return NULL;
    t_token *current = command_node;
    // Traverse down the right links as long as they are WORDs (arguments)
    while (current->right && current->right->token == WORD) {
        current = current->right;
    }
    return current; // Returns command node or the last WORD node in the chain
}
*/




// static t_token *handle_operator_node(t_token *op_node, t_token **root_ptr) {
//     t_token *current_root = *root_ptr;

//     // Operator becomes the new root, previous root becomes left child
//     op_node->left = current_root;
//     if (current_root) {
//         current_root->parent = op_node;
//     }
//     *root_ptr = op_node; // Update the root pointer in the caller

//     // Pipe's right child is linked later when the next command/operand arrives
//     return *root_ptr; // Return the new root
// }



/* works well in pipes
t_token *ft_create_ast(t_token *token_list)
{
    t_token *root = NULL;
    t_token *new_node = NULL;
    t_token *last_command_node = NULL; // Track the last CMD/BUILTIN added
	//int loop_count = 0; // for debug 카운터

    while (token_list)
    {
		//loop_count++; // for debug
        new_node = token_list;
        token_list = token_list->right; // Consume node from list
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->parent = NULL;

        int token_type = new_node->token;

        if (!root) // First token in the (sub)tree
        {
            // Must be CMD, BUILTIN, REDIR, or HEREDOC
            if (token_type == CMD || token_type == BUILTIN || token_type == WORD) {
                last_command_node = new_node; // This is the first command
                root = new_node;
            } else if (token_type == HEREDOC || token_type == REDIR_OPEN ||
                       token_type == REDIR_WRITE || token_type == REDIR_WRITE_A) {
                root = handle_operator_node(new_node, &root); // Pass NULL as current root
                if (!root) return NULL; // Error occurred
                last_command_node = NULL; // Operator resets command context
            } else 
			{
              	fprintf(stderr, "Minishell AST Error: Unexpected token '%s' (type %d) at start of command.\n", new_node->string ? new_node->string : "N/A", token_type);
				return NULL;
			}
        }
        else // Tree already exists
        {
            // Handle Operators (PIPE, REDIR_*, HEREDOC)
            if (token_type == HEREDOC || token_type == REDIR_OPEN ||
                token_type == REDIR_WRITE || token_type == REDIR_WRITE_A ||
                token_type == PIPE)
            {
                root = handle_operator_node(new_node, &root);
                if (!root) return NULL;
                last_command_node = NULL; // Operator resets command context
            }
            // Handle Command/Builtin (must follow PIPE)
			else if (root && root->token == PIPE && root->right == NULL &&
				(token_type == CMD || token_type == BUILTIN || token_type == WORD))
			{
					// This token is the command following a pipe
					root->right = new_node; // Attach as right child of PIPE
					new_node->parent = root;
					last_command_node = new_node; // This is the new command context
					// IMPORTANT: Successfully handled, do not fall through to argument check!
			}
			// ELSE IF: Handle Arguments (only if not handled above)
			//else if (token_type == WORD)
			else if (token_type == WORD || token_type == VAR)
			{
				if (!last_command_node) {
                     fprintf(stderr, "Minishell AST Error: Argument '%s' (type %d) found but no command to attach it to (last_command_node is NULL).\n", new_node->string ? new_node->string : "N/A", token_type);
                     // Cleanup and error
                     // free_token_list(new_node); // Part of token_list, careful freeing
                     // free_ast_recursive(root); // Assuming you have a way to free the partially built AST
                     return NULL;
                }
				t_token *attach_point = find_last_argument(last_command_node);
				if (attach_point) {
					attach_point->right = new_node;
					new_node->parent = attach_point;
				}
				else 
				{
                    // This case should ideally not be reached if last_command_node is valid.
                    // If find_last_argument returns NULL from a non-NULL last_command_node, it's an issue.
                     fprintf(stderr, "Minishell AST Error: Could not find attach point for argument '%s' to command '%s'.\n", new_node->string ? new_node->string : "N/A", last_command_node->string ? last_command_node->string : "N/A");
                     // Cleanup and error
                     return NULL;
                }
			}
			// ELSE: Handle Unknown Token Types
			else
			{
                // Handle unknown or misplaced token types
                fprintf(stderr, "Minishell AST Error: Unexpected token '%s' (type %d) in command structure.\n", new_node->string ? new_node->string : "N/A", token_type);
                // Cleanup and error
                return NULL;
			}
        }
		fflush(stderr); // 디버그 출력이 즉시 보이도록 함
    } // end while
    return root; // Return the final root of the constructed AST
}
*/



// AST 노드와 그 자식들을 재귀적으로 해제하는 함수 (구현 필요)
// static void free_ast_recursive(t_token *node) {
//     if (!node) return;
//     free_ast_recursive(node->left);
//     free_ast_recursive(node->right);
//     if (node->string) free(node->string);
//     free(node);
// }

// 단일 토큰 노드의 내용과 노드 자체를 해제하는 함수 (구현 필요)
// static void free_single_token_node_content_and_node(t_token *node) {
//     if (!node) return;
//     if (node->string) free(node->string);
//     free(node);
// }

// 토큰 리스트 전체를 해제하는 함수 (구현 필요)
// static void free_token_list(t_token *list_head) {
//     t_token *current = list_head;
//     t_token *next;
//     while (current) {
//         next = current->right; // 토큰 리스트는 ->right로 연결되어 있다고 가정
//         free_single_token_node_content_and_node(current);
//         current = next;
//     }
// }

// mini_latest/srcs/parser/tokenizer/tokenize.c 내의 함수

// find_last_argument 함수 (이전 제공 코드 또는 사용자 정의)
static t_token *find_last_argument(t_token *command_node) {
    if (!command_node) return NULL;
    t_token *current = command_node;
    while (current->right &&
           (current->right->token == WORD || current->right->token == VAR || current->right->token == ARG)) {
        current = current->right;
    }
    return current;
}
// ... (필요한 메모리 해제 함수들)

t_token *ft_create_ast(t_token *token_list)
{
    t_token *root = NULL;
    t_token *new_node = NULL;
    t_token *current_simple_cmd_head = NULL;

    while (token_list)
    {
        new_node = token_list;
        token_list = new_node->right;
        new_node->left = NULL; new_node->right = NULL; new_node->parent = NULL;
        int token_type = new_node->token;

        if (IS_PIPE_OPERATOR(token_type)) {
            if (!root || !current_simple_cmd_head) {
                fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
                return NULL;
            }
            new_node->left = root;
            if (root) root->parent = new_node;
            root = new_node;
            current_simple_cmd_head = NULL;
        }
        else if (IS_REDIR_OPERATOR(token_type)) {
            if (!token_list || !IS_FILENAME_TYPE(token_list->token)) {
                fprintf(stderr, "minishell: syntax error: missing or invalid filename after '%s'\n", new_node->string);
                return NULL;
            }
            t_token *filename_node = token_list; token_list = filename_node->right;
            filename_node->left = NULL; filename_node->right = NULL;
            new_node->right = filename_node;
            filename_node->parent = new_node;

            if (current_simple_cmd_head == NULL) {
                if (root == NULL) {
                    root = new_node;
                } else if (root->token == PIPE && root->right == NULL) {
                    root->right = new_node;
                    new_node->parent = root;
                } else if (IS_REDIR_OPERATOR(root->token) && root->left == NULL) {
                    new_node->left = root;
                    if(root) root->parent = new_node;
                    root = new_node;
                } else {
                    fprintf(stderr, "Minishell AST Error: Prefix redirection '%s' in an unexpected AST state. Root: %d (%s)\n",
                            new_node->string, root ? root->token : -1, root ? root->string: "NULL");
                    return NULL;
                }
            } else {
                t_token **target_subtree_root_ptr;
                if (root->token == PIPE) {
                    if(root->right == NULL) {
                        fprintf(stderr, "Minishell AST Error: Inconsistent state for redirection on pipe RHS (RHS is NULL but CSC_Head is not).\n");
                        return NULL;
                    }
                    target_subtree_root_ptr = &root->right;
                } else {
                    target_subtree_root_ptr = &root;
                }
                new_node->left = *target_subtree_root_ptr;
                if (*target_subtree_root_ptr) (*target_subtree_root_ptr)->parent = new_node;
                *target_subtree_root_ptr = new_node;
            }
        }
        else if (IS_COMMAND_COMPONENT(token_type)) {
            if (current_simple_cmd_head == NULL) {
                current_simple_cmd_head = new_node; // 이 명령어가 새로운 csc_head가 됨

                if (root == NULL) { // AST의 첫 노드
                    root = new_node;
                } else if (root->token == PIPE && root->right == NULL) { // 파이프 오른쪽의 첫 명령어
                    root->right = new_node;
                    new_node->parent = root;
                }
                // *** 핵심 수정: 파이프 RHS의 리다이렉션 다음에 오는 명령어 처리 ***
                else if (root->token == PIPE && root->right &&
                           IS_REDIR_OPERATOR(root->right->token) && root->right->left == NULL) {
                    // 예: A | > out CMD. root=PIPE, root->right=REDIR. REDIR->left에 CMD 연결.
                    root->right->left = new_node;
                    new_node->parent = root->right;
                }
                // *** 일반 접두사 리다이렉션 체인 다음에 오는 명령어 처리 ***
                else if (IS_REDIR_OPERATOR(root->token) && root->left == NULL) {
                    t_token *attach_redir = root;
                    // 가장 안쪽(왼쪽)으로 파고들어가서 명령어를 붙일 리다이렉션을 찾음
                    while(attach_redir->left != NULL && IS_REDIR_OPERATOR(attach_redir->left->token) ) {
                        attach_redir = attach_redir->left;
                    }
                    if(IS_REDIR_OPERATOR(attach_redir->token) && attach_redir->left == NULL) {
                        attach_redir->left = new_node;
                        new_node->parent = attach_redir;
                    } else {
                        fprintf(stderr, "Minishell AST Error: Command '%s' cannot find place in prefix redirection chain. Attach_Redir: %s (Token: %d)\n",
                                new_node->string, attach_redir ? attach_redir->string : "NULL", attach_redir ? attach_redir->token : -1);
                        return NULL;
                    }
                } else {
                    // 위의 조건들에서 처리되지 못하고 csc_head가 NULL인 상태로 명령어가 오면 오류
                    fprintf(stderr, "Minishell AST Error: Command component '%s' (type %d) in unexpected state when current_simple_cmd_head is NULL. Root token: %d (%s)\n",
                            new_node->string, new_node->token, root ? root->token : -1, root ? root->string : "N/A");
                    return NULL;
                }
            } else { // 기존 명령어(csc_head)의 인자
                t_token *attach_point = find_last_argument(current_simple_cmd_head);
                 if (!attach_point) {
                    fprintf(stderr, "Minishell AST Error: Cannot find attach point for argument '%s' to command '%s'.\n",
                            new_node->string, current_simple_cmd_head->string);
                    return NULL;
                }
                attach_point->right = new_node;
                new_node->parent = attach_point;
            }
        }
        else {
            fprintf(stderr, "Minishell AST Error: Unknown or unhandled token type %d for string '%s'.\n",
                    token_type, new_node->string ? new_node->string : "N/A");
            return NULL;
        }
    }

    // 최종 유효성 검사
    if (root && root->token == PIPE && (!root->left || !root->right)) {
        fprintf(stderr, "minishell: syntax error: incomplete pipe\n");
        return NULL;
    }
    // 리다이렉션 노드가 명령어를 가지고 있는지 검사 (더 정교하게)
    // 이 검사는 AST를 한 번 순회하여 모든 REDIR 노드의 left가 NULL이 아닌지 확인해야 함
    // (단, 파이프의 마지막 RHS가 REDIR이고 그 REDIR의 left가 NULL인 경우는 실행 전까지는 유효할 수 없음)

    return root;
}


// --- ft_parse (Main parsing function) ---
t_token *ft_parse(char *str) {
    t_token *token_list_head = ft_tokenize(str); 
    if (!token_list_head) { return NULL; }
    
    t_token *ast_root = ft_create_ast(token_list_head); 
    if (!ast_root && token_list_head) { // If AST creation failed BUT token_list was not empty
        // ft_create_ast is responsible for freeing the tokens it consumed or encountered during error.
        // If token_list_head still has nodes (e.g. ft_create_ast returned early due to empty list after processing),
        // and those nodes were not processed and freed by ft_create_ast, they might leak or cause issues.
        // However, a robust ft_create_ast should handle all nodes from token_list_head.
        // If ft_tokenize produced tokens, but ft_create_ast produced NULL (e.g. for an empty command or syntax error it fully cleaned),
        // then no further freeing of token_list_head itself (as a flat list) is needed here.
    }
    return ast_root;
}