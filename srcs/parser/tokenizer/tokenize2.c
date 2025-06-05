#include "tokenize.h"

/**
 * @brief 연산자 문자를 처리하여 토큰을 생성합니다.
 *
 * @param str 입력 문자열
 * @param i 현재 인덱스에 대한 포인터 (함수 내에서 업데이트됨)
 * @param input_len 입력 문자열의 길이
 * @return 생성된 토큰 노드, 실패 시 NULL
 */
t_token *ft_handle_operator(const char *str, size_t *i, size_t input_len) 
{
    char op_str[3] = {0};
    int type = WORD;
    size_t op_len = 1;

    op_str[0] = str[*i];
    if (*i + 1 < input_len) {
        if (str[*i] == '>' && str[*i+1] == '>') {
            type = REDIR_WRITE_A; op_len = 2; op_str[1] = '>';
        } else if (str[*i] == '<' && str[*i+1] == '<') {
            type = HEREDOC; op_len = 2; op_str[1] = '<';
        }
    }
    if (op_len == 1) {
        if (str[*i] == '>') type = REDIR_WRITE;
        else if (str[*i] == '<') type = REDIR_OPEN;
        else if (str[*i] == '|') type = PIPE;
    }
    op_str[op_len] = '\0';
    if (type == WORD && op_len == 1) {
        type = ft_get_token(op_str);
    }
    *i += op_len;
    return ft_new_token_node(op_str, type);
}

/**
 * @brief 따옴표로 묶인 세그먼트를 추출합니다.
 *
 * @param str 입력 문자열
 * @param i 현재 인덱스에 대한 포인터 (함수 내에서 업데이트됨)
 * @param input_len 입력 문자열의 길이
 * @param quote_char 따옴표 문자 (' 또는 ")
 * @param list_head 오류 발생 시 해제할 토큰 리스트 헤드에 대한 포인터
 * @param buffer_to_free_on_error 오류 발생 시 해제할 버퍼
 * @return 추출된 세그먼트 문자열, 실패 시 NULL
 */
char *ft_extract_quoted_segment(const char *str, size_t *i,
                                       size_t input_len, char quote_char,
                                       t_token **list_head, char *buffer_to_free_on_error) 
{
    size_t content_start;
    char *piece_str;

    (*i)++; // 여는 따옴표 소모
    content_start = *i;
    while (*i < input_len && str[*i] != quote_char) {
        (*i)++;
    }
    if (*i >= input_len) {
        fprintf(stderr, "minishell: syntax error: unclosed quote %c\n", quote_char);
        if (buffer_to_free_on_error) free(buffer_to_free_on_error);
        if (list_head && *list_head) free_token_list(*list_head);
        return NULL;
    }
    piece_str = ft_substr(str, content_start, *i - content_start);
    if (!piece_str) { // ft_substr 실패 처리
        if (buffer_to_free_on_error) free(buffer_to_free_on_error);
        if (list_head && *list_head) free_token_list(*list_head);
        return NULL;
    }
    (*i)++; // 닫는 따옴표 소모
    return piece_str;
}

/**
 * @brief 따옴표로 묶이지 않은 세그먼트를 추출합니다.
 *
 * @param str 입력 문자열
 * @param i 현재 인덱스에 대한 포인터 (함수 내에서 업데이트됨)
 * @param input_len 입력 문자열의 길이
 * @return 추출된 세그먼트 문자열, 실패 시 NULL
 */
char *ft_extract_unquoted_segment(const char *str, size_t *i, size_t input_len)
{
    size_t unquoted_start;
    char *piece_str;

    unquoted_start = *i;
    while (*i < input_len && !is_whitespace(str[*i]) &&
           !is_operator_char(str[*i]) && str[*i] != '"' && str[*i] != '\'') {
        (*i)++;
    }
    piece_str = ft_substr(str, unquoted_start, *i - unquoted_start);
    return piece_str; // ft_substr 실패 시 NULL 반환 가능
}

/**
 * @brief 현재 버퍼에 새 문자열 조각(piece)을 결합합니다.
 *
 * @param current_buffer 현재까지 결합된 문자열 버퍼 (이 함수 내에서 해제됨)
 * @param piece_str 결합할 새 문자열 조각 (이 함수 내에서 해제됨)
 * @param list_head 오류 발생 시 해제할 토큰 리스트 헤드에 대한 포인터
 * @return 결합된 새 버퍼, 실패 시 NULL
 */
char *ft_process_segment_concatenation(char *current_buffer, char *piece_str,
                                             t_token **list_head)
{
    char *temp_buffer;

    temp_buffer = ft_strjoin(current_buffer, piece_str);
    free(current_buffer);
    free(piece_str);

    if (!temp_buffer) { // ft_strjoin 실패
        if (list_head && *list_head) free_token_list(*list_head);
        return NULL;
    }
    return temp_buffer;
}

/**
 * @brief 단일 세그먼트(따옴표 있거나 없음)를 처리하여 현재 인자에 추가합니다.
 *
 * @param str 입력 문자열
 * @param idx 현재 인덱스에 대한 포인터 (업데이트됨)
 * @param input_len 입력 문자열 길이
 * @param list_head 오류 시 전체 토큰 리스트 해제를 위한 포인터
 * @param buffer_ptr 현재 인자 버퍼에 대한 포인터 (업데이트됨)
 * @param all_s 단일 따옴표 플래그 포인터 (업데이트됨)
 * @param all_d 이중 따옴표 플래그 포인터 (업데이트됨)
 * @param has_unq 따옴표 없는 세그먼트 존재 플래그 포인터 (업데이트됨)
 * @param count 세그먼트 카운트 포인터 (업데이트됨)
 * @return 성공 시 true, 실패 시 false
 */
bool ft_append_next_segment(const char *str, size_t *idx, size_t input_len,
                                   t_token **list_head, char **buffer_ptr,
                                   bool *all_s, bool *all_d, bool *has_unq, int *count) {
    char *piece_str = NULL;
    bool current_piece_is_single = false;
    bool current_piece_is_double = false;

    if (str[*idx] == '"' || str[*idx] == '\'') {
        char quote_char = str[*idx];
        current_piece_is_single = (quote_char == '\'');
        current_piece_is_double = (quote_char == '"');
        piece_str = ft_extract_quoted_segment(str, idx, input_len, quote_char, list_head, *buffer_ptr);
        if (!piece_str) { *buffer_ptr = NULL; return false; } // 오류 시 buffer_ptr은 helper가 해제
    } else {
        *has_unq = true;
        piece_str = ft_extract_unquoted_segment(str, idx, input_len);
        if (!piece_str) { // ft_substr 실패
            free(*buffer_ptr); *buffer_ptr = NULL;
            if (list_head && *list_head) free_token_list(*list_head);
            return false;
        }
    }
    *buffer_ptr = ft_process_segment_concatenation(*buffer_ptr, piece_str, list_head);
    if (!*buffer_ptr) { return false; }

    if (!current_piece_is_single) *all_s = false;
    if (!current_piece_is_double) *all_d = false;
    (*count)++;
    return true;
}