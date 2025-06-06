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
 * @brief Skips any whitespace characters from the current position.
 */
static void	ft_skip_whitespace(const char *str, size_t *i, size_t input_len)
{
	while (*i < input_len && is_whitespace(str[*i]))
	{
		(*i)++;
	}
}

/**
 * @brief Dispatches to the correct handler (operator or word) to create a token.
 */
static t_token	*ft_create_next_token(const char *str, size_t *i,
	size_t input_len, t_token **list_head)
{
	if (is_operator_char(str[*i]))
	{
		return (ft_handle_operator(str, i, input_len));
	}
	else
	{
		return (ft_handle_word(str, i, input_len, list_head));
	}
}

/**
 * @brief Processes a single token (or block), 
 * appends it to the list, and returns the status.
 */
static t_tokenize_status	ft_process_single_token(const char *str, size_t *i,
	size_t input_len, t_token **list_head)
{
	t_token	*new_node;

	ft_skip_whitespace(str, i, input_len);
	if (*i >= input_len)
		return (TOKENIZE_SUCCESS_BREAK);
	new_node = ft_create_next_token(str, i, input_len, list_head);
	if (!new_node)
	{
		if (*i < input_len && !is_whitespace(str[*i]))
		{
			fprintf(stderr, "tokenizer error near '%.*s'\n", 10, &str[*i]);
			free_token_list(*list_head);
			return (TOKENIZE_ERROR);
		}
		return (TOKENIZE_SUCCESS_BREAK);
	}
	if (*i < input_len && is_whitespace(str[*i]))
		new_node->followed_by_whitespace = true;
	else
		new_node->followed_by_whitespace = false;
	ft_add_back_node(list_head, new_node);
	return (TOKENIZE_SUCCESS_CONTINUE);
}

t_token	*ft_tokenize(char *str)
{
	t_token				*token_list_head;
	size_t				i;
	size_t				input_len;
	t_tokenize_status	status;

	token_list_head = NULL;
	i = 0;
	if (!str)
	{
		return (NULL);
	}
	input_len = strlen(str);
	while (i < input_len)
	{
		status = ft_process_single_token(str, &i, input_len, &token_list_head);
		if (status == TOKENIZE_ERROR)
			return (NULL);
		if (status == TOKENIZE_SUCCESS_BREAK)
			break ;
	}
	return (token_list_head);
}

t_token	*ft_parse(char *str)
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
