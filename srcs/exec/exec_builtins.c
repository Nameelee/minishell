/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   exec2.c                                             :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/16 17:36:56 by jelee          #+#    #+#                */
/*   Updated: 2025/06/16 17:36:58 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

// int	is_numeric(const char *str)
// {
// 	if (!str || *str == '\0')
// 		return (0);
// 	if (*str == '+' || *str == '-')
// 		str++;
// 	while (*str)
// 	{
// 		if (!ft_isdigit((unsigned char)*str))
// 			return (0);
// 		str++;
// 	}
// 	return (1);
// }

/**
 * @brief Counts the number of tokens that could become arguments.
 */
static int	count_potential_args(t_token *node)
{
	int		count;
	t_token	*curr;

	count = 0;
	curr = node;
	while (curr && (curr->token == CMD || curr->token == BUILTIN \
		|| curr->token == WORD || curr->token == VAR))
	{
		count++;
		curr = curr->right;
	}
	return (count);
}

static char	**run_population_loop(char **argv, int max_count,
				t_token *start_node, t_exec_context *ctx)
{
	int		actual_i;
	t_token	*curr;
	int		i;

	actual_i = 0;
	curr = start_node;
	i = -1;
	while (++i < max_count)
	{
		if (!process_single_token(curr, argv, &actual_i, ctx))
			return (free_and_nullify_argv(argv, actual_i));
		curr = curr->right;
	}
	argv[actual_i] = NULL;
	if (actual_i == 0)
		return (free_and_nullify_argv(argv, 0));
	return (argv);
}

char	**build_argv_from_ast(t_token *cmd_node, char ***envp_ptr, int l_exit)
{
	int				max_count;
	char			**argv;
	t_exec_context	ctx;

	if (!cmd_node)
		return (NULL);
	max_count = count_potential_args(cmd_node);
	if (max_count == 0)
		return (NULL);
	argv = (char **)malloc(sizeof(char *) * (max_count + 1));
	if (!argv)
		return (perror("minishell: malloc failed"), NULL);
	ctx.envp_ptr = envp_ptr;
	ctx.l_exit = l_exit;
	return (run_population_loop(argv, max_count, cmd_node, &ctx));
}
