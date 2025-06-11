/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   create_ast_helper3.c                                :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/11 11:18:32 by jelee          #+#    #+#                */
/*   Updated: 2025/06/11 11:18:35 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"
#include "inline_functions1.h"
#include "inline_functions2.h"

/**
 * @brief Attaches a new command to the right side of a pipe node in the AST.
 */
static void	attach_cmd_to_pipe(t_ast_state *s)
{
	t_token	*pipe_node;
	t_token	*new_cmd;
	t_token	*attach_point;

	pipe_node = *(s->root);
	new_cmd = s->new_node;
	attach_point = pipe_node->right;
	if (!attach_point)
	{
		pipe_node->right = new_cmd;
		new_cmd->parent = pipe_node;
	}
	else
	{
		attach_point = find_redir_attach_point(attach_point);
		attach_point->left = new_cmd;
		new_cmd->parent = attach_point;
	}
}

/**
 * @brief Attaches a new command to a redirection chain at the root of the AST.
 */
static void	attach_cmd_to_redir(t_ast_state *s)
{
	t_token	*root_node;
	t_token	*new_cmd;
	t_token	*attach_point;

	root_node = *(s->root);
	new_cmd = s->new_node;
	attach_point = find_redir_attach_point(root_node);
	attach_point->left = new_cmd;
	new_cmd->parent = attach_point;
}

int	attach_new_command(t_ast_state *s)
{
	t_token	*root;
	t_token	*new_cmd;

	root = *(s->root);
	new_cmd = s->new_node;
	if (root == NULL)
		*(s->root) = new_cmd;
	else if (root->token == PIPE)
		attach_cmd_to_pipe(s);
	else if (is_redir_operator(root->token))
		attach_cmd_to_redir(s);
	else
	{
		fprintf(stderr, "AST Error: Command in unexpected state.\n");
		return (0);
	}
	return (1);
}
