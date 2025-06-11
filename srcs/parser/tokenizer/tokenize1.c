/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   tokenize1.c                                         :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/06 17:41:46 by jelee          #+#    #+#                */
/*   Updated: 2025/06/06 17:41:50 by jelee          ########   odam.nl        */
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
t_token	*ft_finalize_word_node(char *buffer, t_fin_quote quote_status,
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
 * @brief Initializes the word building state and aggregator structs.
 *
 * @param state The word building state struct to initialize.
 * @param agg The aggregator struct to initialize.
 * @param list_head Pointer to the token list head for error handling.
 * @return Returns true on success, false on memory allocation failure.
 */
static bool	ft_init_word_build(t_word_build_state *state,
	t_word_aggregator *agg, t_token **list_head)
{
	state->buffer = ft_strdup("");
	if (!state->buffer)
	{
		if (list_head && *list_head)
			free_token_list(*list_head);
		return (false);
	}
	state->all_s_q = true;
	state->all_d_q = true;
	state->has_unq = false;
	state->seg_count = 0;
	agg->buffer_ptr = &state->buffer;
	agg->all_s_ptr = &state->all_s_q;
	agg->all_d_ptr = &state->all_d_q;
	agg->has_unq_ptr = &state->has_unq;
	agg->count_ptr = &state->seg_count;
	agg->list_head_ptr = list_head;
	return (true);
}

/**
 * @brief Determines the final quote status enum based on the aggregation flags.
 *
 * @param all_s True if all segments were single-quoted.
 * @param has_u True if any unquoted segment was found.
 * @param all_d True if all segments were double-quoted.
 * @return The appropriate t_fin_quote enum value.
 */
static	t_fin_quote	ft_get_finalize_quote_status(
	bool all_s, bool has_u, bool all_d)
{
	if (all_s && !has_u && !all_d)
		return (FNL_QUOTE_ALL_SINGLE);
	if (all_d && !has_u && !all_s)
		return (FNL_QUOTE_ALL_DOUBLE);
	return (FNL_QUOTE_NONE);
}

t_token	*ft_handle_word(const char *str, size_t *i, size_t input_len,
	t_token **list_head)
{
	t_word_build_state		state;
	t_word_aggregator		aggregator;
	t_fin_quote				quote_status;
	t_token					*new_node;

	if (!ft_init_word_build(&state, &aggregator, list_head))
		return (NULL);
	while (*i < input_len && !is_whitespace(str[*i])
		&& !is_operator_char(str[*i]))
	{
		if (!ft_append_next_segment(str, i, input_len, &aggregator))
			return (NULL);
	}
	quote_status = ft_get_finalize_quote_status(
			state.all_s_q, state.has_unq, state.all_d_q);
	new_node = ft_finalize_word_node(
			state.buffer, quote_status, state.seg_count);
	if (state.buffer)
		free(state.buffer);
	return (new_node);
}
