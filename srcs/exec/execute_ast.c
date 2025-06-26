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
#include "../parser/tokenizer/inline_functions1.h"

static int	execute_parent_builtin(t_token *node,
				char ***envp, int exit_status);

/**
 * @brief Checks if a token type is a redirection operator.
 * This is a local helper to avoid include path issues.
 */
static bool	is_redirection_operator(int type)
{
	return (type == REDIR_OPEN || type == REDIR_WRITE
		|| type == REDIR_WRITE_A || type == HEREDOC);
}

/**
 * @brief Skips redirection nodes to find the actual command node.
 */
static t_token	*get_command_node(t_token *node)
{
	t_token	*cmd_node;

	cmd_node = node;
	while (cmd_node && is_redirection_operator(cmd_node->token))
	{
		cmd_node = cmd_node->left;
	}
	return (cmd_node);
}

/**
 * @brief Checks if a command is a builtin that must run in the parent process.
 */
static bool	is_parent_builtin(t_token *cmd_node)
{
	const char	*cmd;

	if (!cmd_node || cmd_node->token != BUILTIN)
		return (false);
	cmd = cmd_node->string;
	if (ft_strncmp(cmd, "exit", 5) == 0 || ft_strncmp(cmd, "cd", 3) == 0
		|| ft_strncmp(cmd, "export", 7) == 0
		|| ft_strncmp(cmd, "unset", 6) == 0)
	{
		return (true);
	}
	return (false);
}

/**
 * @brief Executes a builtin command in the parent, IO redirection.
 */
static int	execute_parent_builtin(t_token *node,
				char ***envp, int exit_status)
{
	int		saved_stdin;
	int		saved_stdout;
	t_redir	*redir_list;
	t_token	*cmd_node;
	int		status;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	if (saved_stdin == -1 || saved_stdout == -1)
		return (perror("minishell: dup"), 1);
	cmd_node = node;
	redir_list = parse_redirection_nodes(&cmd_node);
	if (apply_redirections(redir_list) == -1)
		status = 1;
	else
		status = ft_execute_builtin(cmd_node, envp, exit_status);
	free_redir_list(redir_list);
	if (dup2(saved_stdin, STDIN_FILENO) == -1
		|| dup2(saved_stdout, STDOUT_FILENO) == -1)
		perror("minishell: dup2 restore failed");
	close(saved_stdin);
	close(saved_stdout);
	return (status);
}

int	execute_ast(t_token *node, char ***envp,
	bool is_top_level, int exit_status)
{
	t_token	*cmd_node;

	if (!node)
	{
		if (is_top_level)
			return (0);
		else
			exit(0);
	}
	if (is_top_level && node->token != PIPE)
	{
		cmd_node = get_command_node(node);
		if (is_parent_builtin(cmd_node))
		{
			return (execute_parent_builtin(node, envp, exit_status));
		}
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
