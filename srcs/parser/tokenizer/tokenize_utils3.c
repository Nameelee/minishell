/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   tokenize_utils3.c                                   :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/09 17:40:59 by jelee          #+#    #+#                */
/*   Updated: 2025/06/09 17:41:02 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"

int	ft_get_associativity(int token)
{
	if (token == REDIR_OPEN || token == HEREDOC)
		return (2);
	return (1);
}

void	ft_display_commande_lst(t_token *token_lst)
{
	while (token_lst)
	{
		if (token_lst->string)
			printf("%s ", token_lst->string);
		token_lst = token_lst->right;
	}
	printf("\n");
}

char	*ft_get_str_token(int token)
{
	if (token == WORD)
		return ("WORD ");
	if (token == CMD)
		return ("COMMANDE ");
	if (token == ARG)
		return ("ARG ");
	if (token == PIPE)
		return ("PIPE ");
	if (token == REDIR_OPEN)
		return ("REDIR_OPEN ");
	if (token == REDIR_WRITE)
		return ("REDIR_WRITE ");
	if (token == REDIR_WRITE_A)
		return ("REDIR_WRITE_A ");
	if (token == DOUBLE_REDIR)
		return ("DOUBLE_REDIR ");
	if (token == HEREDOC)
		return ("HEREDOC ");
	if (token == VAR)
		return ("VAR");
	if (token == BUILTIN)
		return ("BUILTIN");
	return (NULL);
}

void	ft_display_token_sequence_lst(t_token *token_lst)
{
	while (token_lst)
	{
		printf("%s\n", ft_get_str_token(token_lst->token));
		token_lst = token_lst->right;
	}
}

char	*ft_get_total_path(char *path, char *str)
{
	char	*intermediaire_path;
	char	*total_path;

	intermediaire_path = ft_strjoin(path, "/");
	if (!intermediaire_path)
		return (NULL);
	total_path = ft_strjoin(intermediaire_path, str);
	free(intermediaire_path);
	intermediaire_path = NULL;
	return (total_path);
}
