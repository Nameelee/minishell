/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   create_ast.c                                        :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/11 11:04:20 by jelee          #+#    #+#                */
/*   Updated: 2025/06/11 11:04:24 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"

/**
 * @brief Initializes the state required for building the AST.
 *
 * @param root A pointer to the root of the AST
 * @param csc_head A pointer to the head of the current simple command
 * @param state The t_ast_state struct to initialize
 */
static void	ft_init_ast_state(t_token **root, t_token **csc_head,
				t_ast_state *state)
{
	*root = NULL;
	*csc_head = NULL;
	state->root = root;
	state->csc_head = csc_head;
	state->new_node = NULL;
}

/**
 * @brief Processes one token from the list and adds it to the AST.
 *
 * @param state The current state of the AST build.
 * @param token_list A pointer to the current position in the token list.
 * @return Returns true on success, false on failure.
 */
static bool	ft_process_next_token(t_ast_state *state, t_token **token_list)
{
	state->new_node = get_next_node(token_list);
	if (IS_PIPE_OPERATOR(state->new_node->token))
	{
		if (!handle_pipe(state))
			return (false);
	}
	else if (IS_REDIR_OPERATOR(state->new_node->token))
	{
		if (!handle_redir(state, token_list))
			return (false);
	}
	else if (IS_COMMAND_COMPONENT(state->new_node->token))
	{
		if (!handle_command(state))
			return (false);
	}
	else
	{
		fprintf(stderr, "AST Error: Unhandled token type %d\n",
			state->new_node->token);
		return (false);
	}
	return (true);
}

t_token	*ft_create_ast(t_token *token_list)
{
	t_token		*root;
	t_token		*csc_head;
	t_ast_state	state;

	ft_init_ast_state(&root, &csc_head, &state);
	while (token_list)
	{
		if (!ft_process_next_token(&state, &token_list))
		{
			free_token_list(root);
			return (NULL);
		}
	}
	if (!is_valid_ast(root))
	{
		free_token_list(root);
		return (NULL);
	}
	return (root);
}
