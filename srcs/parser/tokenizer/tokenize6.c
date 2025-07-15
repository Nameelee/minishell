/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   tokenize6.c                                         :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/27 17:32:43 by jelee          #+#    #+#                */
/*   Updated: 2025/06/27 17:33:53 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"
#include "inline_functions1.h"
#include "inline_functions2.h"

t_token	*ft_get_last_token(t_token *list)
{
	if (!list)
		return (NULL);
	while (list->right)
		list = list->right;
	return (list);
}

t_token	*ft_finalize_word_node(char *buffer, int token_type,
	t_fin_quote quote_status, int seg_count)
{
	t_token	*new_node;

	new_node = NULL;
	if (ft_strlen(buffer) > 0)
	{
		new_node = ft_new_token_node(buffer, token_type);
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
bool	ft_init_word_build(t_word_build_state *state,
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
