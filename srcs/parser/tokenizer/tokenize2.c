/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   tokenize2.c                                         :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/05 19:04:46 by jelee          #+#    #+#                */
/*   Updated: 2025/06/05 19:04:52 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */
#include "tokenize.h"

/**
 * @brief 현재 버퍼에 새 문자열 조각(piece)을 결합합니다.
 *
 * @param current_buffer 현재까지 결합된 문자열 버퍼 (이 함수 내에서 해제됨)
 * @param piece_str 결합할 새 문자열 조각 (이 함수 내에서 해제됨)
 * @param list_head 오류 발생 시 해제할 토큰 리스트 헤드에 대한 포인터
 * @return 결합된 새 버퍼, 실패 시 NULL
 */
char	*ft_seg_concaten(char *current_buffer, char *piece_str,
	t_token **list_head)
{
	char	*temp_buffer;

	temp_buffer = ft_strjoin(current_buffer, piece_str);
	free(current_buffer);
	free(piece_str);
	if (!temp_buffer)
	{
		if (list_head && *list_head)
			free_token_list(*list_head);
		return (NULL);
	}
	return (temp_buffer);
}

/**
 * @brief Helper to extract a quoted piece and determine its quote type.
 * (입력 인자 개수 및 반환 방식 수정됨)
 *
 * @param str_input 입력 문자열
 * @param idx_ptr 현재 인덱스에 대한 포인터 (업데이트됨)
 * @param len_input 입력 문자열 길이
 * @param agg 단어 구성을 위한 상태 집계자 구조체 포인터
 * @return t_quoted_pdata 구조체 (추출된 문자열, 따옴표 타입, 성공 여부 포함)
 */
t_quoted_pdata	ft_handle_quoted_piece(
	const char *str_input, size_t *idx_ptr,
	size_t len_input, t_word_aggregator *agg)
{
	t_quoted_pdata	data_out;
	t_parse_state	p_state;
	char			quote_char;

	data_out.extracted_str = NULL;
	data_out.is_single_quoted_segment = false;
	data_out.is_double_quoted_segment = false;
	data_out.op_success = false;
	quote_char = str_input[*idx_ptr];
	data_out.is_single_quoted_segment = (quote_char == '\'');
	data_out.is_double_quoted_segment = (quote_char == '"');
	p_state.str_content = str_input;
	p_state.current_idx_ptr = idx_ptr;
	p_state.total_len = len_input;
	data_out.extracted_str
		= ft_extract_quoted_segment(&p_state, quote_char, agg);
	if (!data_out.extracted_str)
		*agg->buffer_ptr = NULL;
	else
		data_out.op_success = true;
	return (data_out);
}

/**
 * @brief Helper to extract an unquoted piece and update related flags.
 */
t_unquot_pdata	ft_handle_unquoted_piece(
	const char *str_input, size_t *idx_ptr,
		size_t len_input, t_word_aggregator *agg)
{
	t_unquot_pdata	data_out;

	data_out.extracted_str = NULL;
	data_out.op_success = false;
	data_out.extracted_str
		= ft_extract_unquoted_segment(str_input, idx_ptr, len_input);
	if (!data_out.extracted_str)
	{
		if (*agg->buffer_ptr)
		{
			free(*agg->buffer_ptr);
			*agg->buffer_ptr = NULL;
		}
		if (agg->list_head_ptr && *(agg->list_head_ptr))
			free_token_list(*(agg->list_head_ptr));
	}
	else
		data_out.op_success = true;
	return (data_out);
}

bool	ft_append_next_segment(const char *str, size_t *idx, size_t input_len,
	t_word_aggregator *agg)
{
	t_seg_extract	extraction_info;

	extraction_info = ft_extract_current_segment_info(str, idx, input_len, agg);
	if (!extraction_info.success)
		return (false);
	*agg->buffer_ptr = ft_seg_concaten(*agg->buffer_ptr,
			extraction_info.piece_str, agg->list_head_ptr);
	if (!*agg->buffer_ptr)
		return (false);
	if (!extraction_info.is_single)
		*(agg->all_s_ptr) = false;
	if (!extraction_info.is_double)
		*(agg->all_d_ptr) = false;
	(*(agg->count_ptr))++;
	return (true);
}
