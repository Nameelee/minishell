/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   tokenize3.c                                         :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/05 18:56:00 by jelee          #+#    #+#                */
/*   Updated: 2025/06/05 18:56:07 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"
/**
 * @brief Checks for two-character operators (e.g., ">>", "<<") 
 * and updates the state.
 * Assumes op_state->op_str[0] is already set.
 */
static void	ft_identify_two_char_operator(const char *str,
	size_t current_i, size_t input_len, t_op_build_state *state)
{
	if (current_i + 1 < input_len)
	{
		if (str[current_i] == '>' && str[current_i + 1] == '>')
		{
			state->type = REDIR_WRITE_A;
			state->op_len = 2;
			state->op_str[1] = '>';
		}
		else if (str[current_i] == '<' && str[current_i + 1] == '<')
		{
			state->type = HEREDOC;
			state->op_len = 2;
			state->op_str[1] = '<';
		}
	}
}

/**
 * @brief If the operator is single-character, 
 * this sets specific types like PIPE, etc.
 * Assumes op_state->op_len is 1 and op_state->op_str[0] holds the character.
 */
static void	ft_set_specific_single_char_type(t_op_build_state *state)
{
	if (state->op_str[0] == '>')
		state->type = REDIR_WRITE;
	else if (state->op_str[0] == '<')
		state->type = REDIR_OPEN;
	else if (state->op_str[0] == '|')
		state->type = PIPE;
}

t_token	*ft_handle_operator(const char *str, size_t *i, size_t input_len)
{
	t_op_build_state	op_state;

	op_state.op_str[0] = str[*i];
	op_state.op_str[1] = '\0';
	op_state.op_str[2] = '\0';
	op_state.type = WORD;
	op_state.op_len = 1;
	ft_identify_two_char_operator(str, *i, input_len, &op_state);
	if (op_state.op_len == 1)
	{
		ft_set_specific_single_char_type(&op_state);
	}
	op_state.op_str[op_state.op_len] = '\0';
	if (op_state.type == WORD && op_state.op_len == 1)
		op_state.type = ft_get_token(op_state.op_str);
	*i += op_state.op_len;
	return (ft_new_token_node(op_state.op_str, op_state.type));
}
