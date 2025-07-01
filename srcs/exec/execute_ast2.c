/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   execute_ast2.c                                      :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/16 17:24:42 by jelee          #+#    #+#                */
/*   Updated: 2025/06/16 17:24:46 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @brief Parses redirection tokens from the AST and builds a list.
 * @param cmd_node_ptr A pointer to the command node, which will be updated.
 * @return A linked list of redirections (t_redir *).
 */
t_redir	*parse_redirection_nodes(t_token **cmd_node_ptr)
{
	t_redir	*redir_list;

	redir_list = NULL;
	while (*cmd_node_ptr && ((*cmd_node_ptr)->token == REDIR_OPEN
			|| (*cmd_node_ptr)->token == REDIR_WRITE
			|| (*cmd_node_ptr)->token == REDIR_WRITE_A
			|| (*cmd_node_ptr)->token == HEREDOC))
	{
		if (!(*cmd_node_ptr)->right || !(*cmd_node_ptr)->right->string)
		{
			//fprintf(stderr, "minishell: syntax error near redirection\n");
			ft_fprintf("minishell: syntax error near redirection\n", "", "");
			free_redir_list(redir_list);
			exit(1);
		}
		if ((*cmd_node_ptr)->token == HEREDOC)
			add_redirection_to_list(&redir_list, (*cmd_node_ptr)->token,
				(*cmd_node_ptr)->right->string, *cmd_node_ptr);
		else
			add_redirection_to_list(&redir_list, (*cmd_node_ptr)->token,
				(*cmd_node_ptr)->right->string, NULL);
		*cmd_node_ptr = (*cmd_node_ptr)->left;
	}
	return (redir_list);
}

static t_token	*handle_redirections(t_token *node)
{
	t_redir	*redir_list;
	t_token	*cmd_node;

	cmd_node = node;
	redir_list = parse_redirection_nodes(&cmd_node);
	if (apply_redirections(redir_list) == -1)
	{
		free_redir_list(redir_list);
		exit(1);
	}
	free_redir_list(redir_list);
	return (cmd_node);
}

/**
 * @brief Contains all logic that runs inside a child process.
 */
void	execute_child_command(t_token *node, char ***envp, int l_exit)
{
	t_token	*cmd_node;

	cmd_node = handle_redirections(node);
	dispatch_command_execution(cmd_node, envp, l_exit);
}

int	wait_and_get_status(pid_t pid)
{
	int		status;
	int		exit_status;
	char	*status_str; 

	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		exit_status = 128 + WTERMSIG(status);
		if (WTERMSIG(status) == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
		else if (WTERMSIG(status) == SIGQUIT)
		{
			//fprintf(stderr, "Quit: %d\n", WTERMSIG(status));
			status_str = ft_itoa(WTERMSIG(status));
			if (status_str == NULL) 
			{
				ft_fprintf("Memory alloc fail error printing.\n", "", "");
				return (1);
			}
			ft_fprintf("Quit: ", status_str, "\n");
			free(status_str);
		}
	}
	else
		exit_status = 1;
	return (exit_status);
}
