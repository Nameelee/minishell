/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   tokenize_utils6.c                                   :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/09 16:37:17 by jelee          #+#    #+#                */
/*   Updated: 2025/06/09 16:38:17 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"

const char	*get_token_type_string(int token_type)
{
	if (token_type == WORD)
		return ("WORD");
	if (token_type == CMD)
		return ("CMD");
	if (token_type == ARG)
		return ("ARG");
	if (token_type == PIPE)
		return ("PIPE");
	if (token_type == REDIR_OPEN)
		return ("INPUT_REDIRECT (<)");
	if (token_type == REDIR_WRITE)
		return ("OUTPUT_REDIRECT (>)");
	if (token_type == REDIR_WRITE_A)
		return ("APPEND_REDIRECT (>>)");
	if (token_type == DOUBLE_REDIR)
		return ("DOUBLE_REDIR");
	if (token_type == HEREDOC)
		return ("HEREDOC (<<)");
	if (token_type == VAR)
		return ("VAR");
	if (token_type == BUILTIN)
		return ("BUILTIN");
	return ("UNKNOWN");
}

int	ft_delete_token_lst(t_token **token_lst)
{
	if (!token_lst)
		return (0);
	free_token_list(*token_lst);
	*token_lst = NULL;
	return (1);
}

/**
 * @brief Initializes the members of a token node to their default values.
 * * @param token_node The token node to initialize.
 * @param token The token type to assign.
 */
/**/
static	void	ft_init_token_defaults(t_token *token_node, int token)
{
	token_node->token = token;
	token_node->followed_by_whitespace = false;
	token_node->asso = ft_get_associativity(token);
	token_node->precedence = ft_get_precedence(token);
	token_node->heredoc_pipe_fd = -1;
	token_node->heredoc_state = HD_NOT_PROCESSED;
	token_node->left = NULL;
	token_node->right = NULL;
	token_node->parent = NULL;
	token_node->single_quote = 0;
	token_node->double_quote = 0;
}

t_token	*ft_new_token_node(char *str, int token)
{
	t_token	*token_node;

	token_node = (t_token *)malloc(sizeof(t_token));
	if (!token_node)
	{
		perror("malloc in ft_new_token_node");
		return (NULL);
	}
	token_node->string = NULL;
	if (str)
	{
		token_node->string = ft_strdup(str);
		if (!token_node->string && str[0] != '\0')
		{
			perror("ft_strdup in ft_new_token_node");
			free(token_node);
			return (NULL);
		}
	}
	ft_init_token_defaults(token_node, token);
	return (token_node);
}

void	ft_add_back_node(t_token **lst, t_token *node)
{
	t_token	*curr;

	if (!lst || !node)
		return ;
	if (!*lst)
		*lst = node;
	else
	{
		curr = *lst;
		{
			while (curr->right)
			{
				curr = curr->right;
			}
			curr->right = node;
		}
	}
}
