#include "tokenize.h"

/**
 * @brief 최종 단어 버퍼로부터 토큰 노드를 생성하고 따옴표 플래그를 설정합니다.
 *
 * @param buffer 최종 단어 문자열 버퍼
 * @param all_single 모든 세그먼트가 단일 따옴표였는지 여부
 * @param all_double 모든 세그먼트가 이중 따옴표였는지 여부
 * @param has_unquoted 따옴표 없는 세그먼트가 있었는지 여부
 * @param seg_count 총 세그먼트 수
 * @return 생성된 토큰 노드, 내용이 없거나 실패 시 NULL
 */
t_token *ft_finalize_word_node(char *buffer, bool all_single,
                                       bool all_double, bool has_unquoted,
                                       int seg_count) {
    t_token *new_node = NULL;

    if (ft_strlen(buffer) > 0) {
        new_node = ft_new_token_node(buffer, ft_get_token(buffer));
        if (new_node && seg_count > 0) {
            if (all_single && !has_unquoted && !all_double) {
                new_node->single_quote = 1;
            } else if (all_double && !has_unquoted && !all_single) {
                new_node->double_quote = 1;
            }
        }
    }
    return new_node;
}

/**
 * @brief 단어 또는 따옴표로 묶인 문자열을 처리하여 토큰을 생성합니다.
 *
 * @param str 입력 문자열
 * @param i 현재 인덱스에 대한 포인터 (함수 내에서 업데이트됨)
 * @param input_len 입력 문자열의 길이
 * @param list_head 오류 발생 시 해제할 토큰 리스트 헤드에 대한 포인터 (함수 내에서 업데이트될 수 있음)
 * @return 생성된 토큰 노드, 실패 또는 빈 토큰 시 NULL
 */
t_token *ft_handle_word(const char *str, size_t *i, size_t input_len,
                               t_token **list_head) {
    char *current_arg_buffer;
    bool all_s_q = true, all_d_q = true, has_unq = false;
    int seg_count = 0;
    t_token *new_node = NULL;

    current_arg_buffer = ft_strdup("");
    if (!current_arg_buffer) {
        if (list_head && *list_head) free_token_list(*list_head);
        return NULL;
    }
    while (*i < input_len && !is_whitespace(str[*i]) && !is_operator_char(str[*i])) {
        if (!ft_append_next_segment(str, i, input_len, list_head, &current_arg_buffer,
                                   &all_s_q, &all_d_q, &has_unq, &seg_count)) {
            // current_arg_buffer와 list_head는 ft_append_next_segment 내부에서 오류 시 해제됨
            return NULL;
        }
    }
    new_node = ft_finalize_word_node(current_arg_buffer, all_s_q, all_d_q, has_unq, seg_count);
    if (current_arg_buffer) { // ft_finalize_word_node는 버퍼 내용을 복사하므로 원본은 해제
        free(current_arg_buffer);
    }
    return new_node; // new_node가 NULL일 수 있음 (예: 빈 문자열 토큰화 결과)
}

// --- Main Tokenizer Function ---

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
        if (is_operator_char(str[i])) {
            new_node = ft_handle_operator(str, &i, input_len);
        } else {
            new_node = ft_handle_word(str, &i, input_len, &token_list_head);
            // ft_handle_word가 NULL을 반환하고 list_head가 해제된 경우 (내부 오류),
            // 아래 !new_node 검사에서 처리되거나, 여기서 바로 NULL 반환도 가능.
            // 현재 구조는 아래의 !new_node에서 일괄 처리.
        }

        if (!new_node) {
            // new_node가 NULL인 경우:
            // 1. 연산자/단어 처리 중 ft_new_token_node 실패
            // 2. ft_handle_word가 빈 토큰을 생성 (예: "" 처리)
            // 3. ft_handle_word 내부에서 오류 발생 (이 경우 list_head는 이미 해제되었어야 함)
            if (i < input_len && !is_whitespace(str[i])) {
                fprintf(stderr, "minishell: tokenizer error near '%.*s'\n", 10, &str[i]);
                free_token_list(token_list_head); // 부분적으로 생성된 리스트 해제
                return NULL;
            }
            break; // 오류는 아니지만 더 이상 토큰화할 내용이 없음
        }

        if (i < input_len && is_whitespace(str[i])) new_node->followed_by_whitespace = true;
        else new_node->followed_by_whitespace = false;

        ft_add_back_node(&token_list_head, new_node);
    }
    return token_list_head;
}


/* Before refactoring

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
*/

t_token *ft_parse(char *str) {
    t_token *token_list_head = ft_tokenize(str); 
    if (!token_list_head) { return NULL; }
    
    t_token *ast_root = ft_create_ast(token_list_head); 
    if (!ast_root && token_list_head) 
	{ 
    }
    return ast_root;
}