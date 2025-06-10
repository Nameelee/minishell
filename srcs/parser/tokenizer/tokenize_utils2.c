/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   tokenize_utils2.c                                   :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/10 11:38:37 by jelee          #+#    #+#                */
/*   Updated: 2025/06/10 11:38:41 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"

bool	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\v' || c == '\f' || c == '\r');
}

bool	is_operator_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

void	free_single_token_node_content_and_node(t_token *node)
{
	if (!node)
		return ;
	if (node->string)
		free(node->string);
	node->string = NULL;
	free(node);
}

void	free_token_list(t_token *list_head)
{
	t_token	*current;
	t_token	*next;

	current = list_head;
	while (current)
	{
		next = current->right;
		free_single_token_node_content_and_node(current);
		current = next;
	}
}
