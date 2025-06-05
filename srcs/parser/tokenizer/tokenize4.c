/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   tokenize3.c                                         :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/05 19:55:48 by jelee          #+#    #+#                */
/*   Updated: 2025/06/05 19:55:52 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */
#include "tokenize.h"

/**
 * @brief 현재 인덱스에서 시작하여 닫는 따옴표를 찾을 때까지 인덱스를 전진시킵니다.
 *
 * @param p_state 파싱 상태 (입력 문자열, 현재 인덱스 포인터, 총 길이 포함)
 * @param quote_char 찾아야 할 닫는 따옴표 문자
 */
static void	ft_advance_to_closing_quote(t_parse_state *p_state, char quote_char)
{
	while (*(p_state->current_idx_ptr) < p_state->total_len
		&& p_state->str_content[*(p_state->current_idx_ptr)] != quote_char)
	{
		(*(p_state->current_idx_ptr))++;
	}
}

/**
 * @brief 따옴표로 묶인 세그먼트 추출 중 오류 발생 시 메모리 정리 작업을 수행합니다.
 *
 * @param agg 단어 집계자 구조체 (정리해야 할 버퍼 및 토큰 리스트에 대한 포인터 포함)
 */
static void	ft_cleanup_on_extraction_error(t_word_aggregator *agg)
{
	if (*agg->buffer_ptr)
		free(*agg->buffer_ptr);
	if (agg->list_head_ptr && *(agg->list_head_ptr))
		free_token_list(*(agg->list_head_ptr));
}

char	*ft_extract_quoted_segment(t_parse_state *p_state, char quote_char,
		t_word_aggregator *agg)
{
	size_t	content_start;
	char	*piece_str;

	(*(p_state->current_idx_ptr))++;
	content_start = *(p_state->current_idx_ptr);
	ft_advance_to_closing_quote(p_state, quote_char);
	if (*(p_state->current_idx_ptr) >= p_state->total_len)
	{
		fprintf(stderr, "syntax error: unclosed quote %c\n", quote_char);
		ft_cleanup_on_extraction_error(agg);
		return (NULL);
	}
	piece_str = ft_substr(p_state->str_content, content_start,
			*(p_state->current_idx_ptr) - content_start);
	if (!piece_str)
	{
		ft_cleanup_on_extraction_error(agg);
		return (NULL);
	}
	(*(p_state->current_idx_ptr))++;
	return (piece_str);
}
//--end of ft_extrac_quoted_segment

/**
 * @brief 따옴표로 묶이지 않은 세그먼트를 추출합니다.
 *
 * @param str 입력 문자열
 * @param i 현재 인덱스에 대한 포인터 (함수 내에서 업데이트됨)
 * @param input_len 입력 문자열의 길이
 * @return 추출된 세그먼트 문자열, 실패 시 NULL
 */
char	*ft_extract_unquoted_segment(
	const char *str, size_t *i, size_t input_len)
{
	size_t	unquoted_start;
	char	*piece_str;

	unquoted_start = *i;
	while (*i < input_len && !is_whitespace(str[*i])
		&& !is_operator_char(str[*i]) && str[*i] != '"' && str[*i] != '\'')
	{
		(*i)++;
	}
	piece_str = ft_substr(str, unquoted_start, *i - unquoted_start);
	return (piece_str);
}
