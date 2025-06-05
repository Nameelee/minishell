/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   tokenize3.c                                         :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/05 20:11:03 by jelee          #+#    #+#                */
/*   Updated: 2025/06/05 20:11:06 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"
/**
 * @brief 따옴표로 시작하는 세그먼트를 처리하고 추출 결과를 반환합니다.
 */
static t_segment_extraction_result	ft_process_quoted_segment_extraction(
	const char *str, size_t *idx, size_t input_len, t_word_aggregator *agg)
{
	t_segment_extraction_result	result;
	t_quoted_piece_data			q_data;

	result.piece_str = NULL;
	result.is_single = false;
	result.is_double = false;
	result.success = false;
	q_data = ft_handle_quoted_piece(str, idx, input_len, agg);
	if (!q_data.op_success)
		return (result);
	result.piece_str = q_data.extracted_str;
	result.is_single = q_data.is_single_quoted_segment;
	result.is_double = q_data.is_double_quoted_segment;
	result.success = true;
	return (result);
}

/**
 * @brief 따옴표 없이 시작하는 세그먼트를 처리하고 추출 결과를 반환합니다.
 */
static t_segment_extraction_result	ft_process_unquoted_segment_extraction(
	const char *str, size_t *idx, size_t input_len, t_word_aggregator *agg)
{
	t_segment_extraction_result	result;
	t_unquoted_piece_data		u_data;

	result.piece_str = NULL;
	result.is_single = false;
	result.is_double = false;
	result.success = false;
	*(agg->has_unq_ptr) = true;
	u_data = ft_handle_unquoted_piece(str, idx, input_len, agg);
	if (!u_data.op_success)
		return (result);
	result.piece_str = u_data.extracted_str;
	result.success = true;
	return (result);
}

t_segment_extraction_result	ft_extract_current_segment_info(
	const char *str, size_t *idx, size_t input_len, t_word_aggregator *agg)
{
	if (str[*idx] == '"' || str[*idx] == '\'')
		return (
			ft_process_quoted_segment_extraction(str, idx, input_len, agg));
	else
		return (
			ft_process_unquoted_segment_extraction(str, idx, input_len, agg));
}
