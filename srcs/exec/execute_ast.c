/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   execute_ast.c                                       :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/16 16:33:31 by jelee          #+#    #+#                */
/*   Updated: 2025/06/16 16:33:37 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

int	execute_ast(t_token *node, char ***envp,
	bool is_top_level, int exit_status)
{
	if (!node)
	{
		if (is_top_level)
			return (0);
		else
			exit(0);
	}
	if (is_top_level)
		return (execute_toplevel_command(node, envp, exit_status));
	else
	{
		execute_child_command(node, envp, exit_status);
		exit(EXIT_FAILURE);
	}
	return (1);
}
