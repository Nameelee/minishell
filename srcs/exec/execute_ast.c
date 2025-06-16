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

void	execute_ast(t_token *node, char ***envp, bool is_top_level)
{
	if (!node)
	{
		if (is_top_level)
			g_exit_status = 0;
		return ;
	}
	if (is_top_level)
		execute_toplevel_command(node, envp);
	else
		execute_child_command(node, envp);
}
