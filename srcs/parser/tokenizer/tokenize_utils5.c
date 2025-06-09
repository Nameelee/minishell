/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   tokenize_utils5.c                                   :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/09 16:39:45 by jelee          #+#    #+#                */
/*   Updated: 2025/06/09 16:39:50 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"

void	print_ast(t_token *node, int level)
{
	int	i;

	if (node == NULL)
		return ;
	i = 0;
	while (i < level - 1)
	{
		printf("|   ");
		i++;
	}
	if (level > 0)
		printf("|-- ");
	printf("%s", get_token_type_string(node->token));
	if (node->string)
		printf(" (%s)", node->string);
	printf("\n");
	print_ast(node->left, level + 1);
	print_ast(node->right, level + 1);
}

void	print_ast_start(t_token *root)
{
	print_ast(root, 0);
}

int	ft_count_occurence_of_token(t_token *token_lst, int token)
{
	int	i;

	i = 0;
	while (token_lst)
	{
		if (token_lst->token == token)
			i++;
		token_lst = token_lst->right;
	}
	return (i);
}
