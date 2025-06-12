/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   expand4.c                                           :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/12 16:10:14 by jelee          #+#    #+#                */
/*   Updated: 2025/06/12 16:10:17 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

static bool	ft_process_expansion_chunk(t_exp_data *data)
{
	if (**data->pos == '$' && !(data->is_double_quoted
			&& *data->pos > data->input_str && *(*data->pos - 1) == '\\'))
	{
		if (!handle_variable_expansion(data->pos, data->buffer, data->envp))
			return (false);
	}
	else if (data->is_double_quoted && **data->pos == '\\'
		&& (*(*data->pos + 1) == '$' || *(*data->pos + 1) == '"'
			|| *(*data->pos + 1) == '\\'))
	{
		(*data->pos)++;
		*data->buffer = append_char(*data->buffer, **data->pos);
		(*data->pos)++;
	}
	else
	{
		*data->buffer = append_char(*data->buffer, **data->pos);
		(*data->pos)++;
	}
	if (!*data->buffer)
		return (false);
	return (true);
}

/**
 * @brief 입력 문자열을 반복하며 각 청크를 확장합니다.
 *
 * 이 함수는 메인 처리 루프를 포함하며, 문자열의 끝에 도달할 때까지
 * ft_process_expansion_chunk를 반복적으로 호출합니다.
 * @param data 확장 상태와 데이터가 포함된 구조체 포인터입니다.
 * @return 처리가 성공하면 true, 실패하면 false를 반환합니다.
 */
static bool	perform_expansion_loop(t_exp_data *data)
{
	while (**data->pos)
	{
		if (!ft_process_expansion_chunk(data))
			return (false);
	}
	return (true);
}

/**
 * @brief Handles single-quoted strings by duplicating them without expansion.
 * @param input_str The string to duplicate.
 * @return The newly allocated string, or a new empty string if input is NULL.
 */
static char	*handle_single_quoted(const char *input_str)
{
	if (input_str)
		return (ft_strdup(input_str));
	return (ft_strdup(""));
}

/**
 * @brief Expands all variables in a given string, coordinating the process.
 * (This function is now under the 25-line limit)
 */
char	*expand_all_variables(const char *input_str, char **envp,
			bool is_single_quoted, bool is_double_quoted)
{
	t_exp_data	data;
	char		*buffer;
	const char	*pos;

	if (is_single_quoted)
		return (handle_single_quoted(input_str));
	buffer = ft_strdup("");
	if (!buffer)
		return (NULL);
	pos = input_str;
	data.pos = &pos;
	data.buffer = &buffer;
	data.envp = envp;
	data.is_double_quoted = is_double_quoted;
	data.input_str = input_str;
	if (!perform_expansion_loop(&data))
	{
		free(buffer);
		return (NULL);
	}
	return (buffer);
}
