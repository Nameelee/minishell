/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   create_ast_helper2.c                                :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/11 11:33:03 by jelee          #+#    #+#                */
/*   Updated: 2025/06/11 11:33:05 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"
#include "inline_functions1.h"
#include "inline_functions2.h"

/**
 * @brief 현재 간단한 명령어의 마지막 인자를 찾습니다.
 * 명령어와 인자들은 'right' 포인터로 연결되어 있습니다.
 */
t_token	*find_last_argument(t_token *cmd_head)
{
	t_token	*current;

	current = cmd_head;
	if (!current)
		return (NULL);
	while (current->right && is_argument_type(current->right->token))
	{
		current = current->right;
	}
	return (current);
}

t_token	*find_redir_attach_point(t_token *redir_chain)
{
	t_token	*current;

	current = redir_chain;
	while (current->left && is_redir_operator(current->left->token))
		current = current->left;
	return (current);
}

int	handle_command(t_ast_state *s)
{
	t_token	*attach_point;

	if (*(s->csc_head) == NULL)
	{
		*(s->csc_head) = s->new_node;
		if (!attach_new_command(s))
			return (0);
	}
	else
	{
		attach_point = find_last_argument(*(s->csc_head));
		if (!attach_point)
			return (0);
		attach_point->right = s->new_node;
		s->new_node->parent = attach_point;
	}
	return (1);
}

/**
 * @brief 완성된 AST가 문법적으로 유효한지 최종 검사합니다.
 */
int	is_valid_ast(t_token *root)
{
	if (root && root->token == PIPE && (!root->left || !root->right))
	{
		fprintf(stderr, "minishell: syntax error: incomplete pipe\n");
		return (0);
	}
	return (1);
}
