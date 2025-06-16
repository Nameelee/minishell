/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   execute_ast1.c                                      :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/16 17:27:06 by jelee          #+#    #+#                */
/*   Updated: 2025/06/16 17:27:14 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @brief Handles errors after execvp fails, setting the correct exit code.
 * This function does not return.
 */
static void	handle_execvp_error(char **argv)
{
	fprintf(stderr, "minishell: %s: ", argv[0]);
	if (errno == EACCES)
	{
		fprintf(stderr, "Permission denied\n");
		free_argv(argv);
		exit(126);
	}
	else if (errno == ENOENT)
	{
		if (ft_strchr(argv[0], '/'))
			fprintf(stderr, "No such file or directory\n");
		else
			fprintf(stderr, "command not found\n");
		free_argv(argv);
		exit(127);
	}
	else
	{
		perror(NULL);
		free_argv(argv);
		exit(127);
	}
}

/**
 * @brief Checks if a command path is a directory before trying to execute it.
 * This function exits with code 126 if the path is a directory.
 */
static void	check_for_directory(char **argv)
{
	struct stat	path_stat;

	if (ft_strchr(argv[0], '/'))
	{
		if (stat(argv[0], &path_stat) == 0)
		{
			if (S_ISDIR(path_stat.st_mode))
			{
				fprintf(stderr, "minishell: %s: Is a directory\n", argv[0]);
				free_argv(argv);
				exit(126);
			}
		}
	}
}

/**
 * @brief Executes a simple external command.
 */
static void	execute_simple_command(t_token *cmd_node, char ***envp)
{
	char	**argv;

	argv = build_argv_from_ast(cmd_node, envp);
	if (!argv)
		exit(0);
	check_for_directory(argv);
	execvp(argv[0], argv);
	handle_execvp_error(argv);
}

/**
 * @brief Handles unrecognized token types that cannot be executed.
 */
static void	handle_unknown_command(t_token *node, char ***envp)
{
	if (is_expendable_variable(node->string, *envp) == 2
		|| !ft_strncmp(node->string, "$?",
			ft_strlen_longest(node->string, "$?")))
	{
		write(STDERR_FILENO, " command not found\n", 20);
		exit(127);
	}
	if (ft_is_variable(node->string)
		&& !is_expendable_variable(node->string, *envp))
		exit(0);
	if (ft_is_variable(node->string)
		&& is_expendable_variable(node->string, *envp) == 1)
	{
		write(STDERR_FILENO, " Is a directory\n", 16);
		exit(126);
	}
	fprintf(stderr, "minishell: %s: command not found (token type %d)\n",
		node->string, node->token);
	exit(127);
}

/**
 * @brief Dispatches a command node to the correct execution function.
 */
void	dispatch_command_execution(t_token *cmd_node, char ***envp)
{
	if (!cmd_node)
		exit(0);
	if (cmd_node->token == PIPE)
		exit(execute_pipe(cmd_node, envp));
	else if (cmd_node->token == BUILTIN)
		exit(ft_execute_builtin(cmd_node, envp));
	else if (cmd_node->token == CMD || cmd_node->token == WORD
		|| cmd_node->token == VAR)
		execute_simple_command(cmd_node, envp);
	else
		handle_unknown_command(cmd_node, envp);
}
