#include "tokenize.h"
#include <assert.h>
#include <errno.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>  

static bool is_whitespace(char c);
static bool is_operator_char(char c); 
static void free_token_list(t_token *list); 
static void free_single_token_node_content_and_node(t_token *node); 

static bool is_whitespace(char c) 
{ 
	return (c == ' ' || c == '\t' || c == '\n' 
		|| c == '\v' || c == '\f' || c == '\r'); 
}

static bool is_operator_char(char c) 
{ 
	return (c == '|' || c == '<' || c == '>'); 
}

static void free_single_token_node_content_and_node(t_token *node) 
{ 
    if (!node) 
		return;
    if (node->string) 
		free(node->string);
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
    return root;
}


t_token *ft_parse(char *str) {
    t_token *token_list_head = ft_tokenize(str); 
    if (!token_list_head) { return NULL; }
    
    t_token *ast_root = ft_create_ast(token_list_head); 
    if (!ast_root && token_list_head) 
	{ 
    }
    return ast_root;
}