/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   tokenize.c                                          :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/05 14:35:25 by jelee          #+#    #+#                */
/*   Updated: 2025/06/05 14:38:09 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

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
t_token	*ft_finalize_word_node(char *buffer, t_finalize_quote_type quote_status,
	int seg_count)
{
	t_token	*new_node;

	new_node = NULL;
	if (ft_strlen(buffer) > 0)
	{
		new_node = ft_new_token_node(buffer, ft_get_token(buffer));
		if (new_node && seg_count > 0)
		{
			if (quote_status == FNL_QUOTE_ALL_SINGLE)
				new_node->single_quote = 1;
			else if (quote_status == FNL_QUOTE_ALL_DOUBLE)
				new_node->double_quote = 1;
		}
	}
	return (new_node);
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
	t_token **list_head)
{
	char *current_arg_buffer;
	bool all_s_q = true;
	bool all_d_q = true;
	bool has_unq = false;
	int seg_count = 0;

	t_token *new_node = NULL;
	t_word_aggregator aggregator;
	t_finalize_quote_type determined_quote_status;
	current_arg_buffer = ft_strdup("");
	if (!current_arg_buffer) {
		if (list_head && *list_head) free_token_list(*list_head);
		return NULL;
	}
	aggregator.buffer_ptr = &current_arg_buffer;
	aggregator.all_s_ptr = &all_s_q;
	aggregator.all_d_ptr = &all_d_q;
	aggregator.has_unq_ptr = &has_unq;
	aggregator.count_ptr = &seg_count;
	aggregator.list_head_ptr = list_head;
	while (*i < input_len && !is_whitespace(str[*i]) && !is_operator_char(str[*i]))
	{
		if (!ft_append_next_segment(str, i, input_len, &aggregator))
			return NULL; 
	}
	determined_quote_status = FNL_QUOTE_NONE;
	if (all_s_q && !has_unq && !all_d_q)
		determined_quote_status = FNL_QUOTE_ALL_SINGLE;
	else if (all_d_q && !has_unq && !all_s_q)
		determined_quote_status = FNL_QUOTE_ALL_DOUBLE;
	new_node = ft_finalize_word_node(current_arg_buffer, determined_quote_status, seg_count);
	if (current_arg_buffer)
		free(current_arg_buffer);
	return new_node;
}

t_token	*ft_tokenize(char *str) 
{
    t_token *token_list_head;
    size_t i;
    size_t input_len;

	token_list_head = NULL;
	i = 0;
    if (!str) return NULL;
    input_len = strlen(str);
    while (i < input_len) 
	{
        while (i < input_len && is_whitespace(str[i])) { i++; }
        if (i >= input_len) break;

        t_token *new_node = NULL;
        if (is_operator_char(str[i]))
            new_node = ft_handle_operator(str, &i, input_len);
        else
            new_node = ft_handle_word(str, &i, input_len, &token_list_head);
        if (!new_node) 
		{
            if (i < input_len && !is_whitespace(str[i]))
			{
                fprintf(stderr, "minishell: tokenizer error near '%.*s'\n", 10, &str[i]);
                free_token_list(token_list_head);
                return NULL;
            }
            break;
        }
        if (i < input_len && is_whitespace(str[i]))
			new_node->followed_by_whitespace = true;
        else
			new_node->followed_by_whitespace = false;
        ft_add_back_node(&token_list_head, new_node);
    }
    return (token_list_head);
}

t_token *ft_parse(char *str)
{
	t_token	*token_list_head;
	t_token	*ast_root;

	token_list_head = ft_tokenize(str);
	if (!token_list_head)
		return (NULL);
	ast_root = ft_create_ast(token_list_head);
	if (!ast_root && token_list_head)
	{
	}
	return (ast_root);
}
