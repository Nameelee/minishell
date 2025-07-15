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
#include "inline_functions1.h"
#include "inline_functions2.h"

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

/**
 * @brief Iterates through the input string, appending segments to build a
 * single word token until a whitespace or operator is found.
 * @return true on success, false on failure.
 */
static bool	build_word_from_segments(const char *str, size_t *i,
	size_t input_len, t_word_aggregator *aggregator)
{
	while (*i < input_len && !is_whitespace(str[*i])
		&& !is_operator_char(str[*i]))
	{
		if (!ft_append_next_segment(str, i, input_len, aggregator))
			return (false);
	}
	return (true);
}

/**
 * @brief Determines if the constructed word is a BUILTIN or a standard WORD
 * based on its content and the preceding token.
 * @return The determined token type (BUILTIN or WORD).
 */
static int	determine_word_token_type(char *buffer, t_token **list_head)
{
	t_token	*last_token;

	last_token = ft_get_last_token(*list_head);
	if (ft_is_builtin(buffer) && (!last_token
			|| !is_redir_operator(last_token->token)))
		return (BUILTIN);
	return (WORD);
}

/**
 * @brief Handles the creation of a word token by initializing state, building
 * the word from segments, and finalizing the new token node.
 * @return A pointer to the newly created token, or NULL on failure.
 */
t_token	*ft_handle_word(const char *str, size_t *i, size_t input_len,
	t_token **list_head)
{
	t_word_build_state	state;
	t_word_aggregator	aggregator;
	t_token				*new_node;
	t_fin_quote			quote_status;

	if (!ft_init_word_build(&state, &aggregator, list_head))
		return (NULL);
	if (!build_word_from_segments(str, i, input_len, &aggregator))
	{
		free(state.buffer);
		return (NULL);
	}
	quote_status = ft_get_finalize_quote_status(state.all_s_q, state.has_unq,
			state.all_d_q);
	new_node = ft_finalize_word_node(state.buffer,
			determine_word_token_type(state.buffer, list_head),
			quote_status, state.seg_count);
	free(state.buffer);
	return (new_node);
}
