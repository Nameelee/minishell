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

static char	*get_path_variable(char **envp)
{
	int	i;

	i = 0;
	if (!envp)
		return (NULL);
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);
		i++;
	}
	return (NULL);
}

static char	*get_full_path(const char *cmd, char **envp)
{
	char	**paths;
	char	*path_var;
	char	*full_path;
	int		i;

	if (!cmd || cmd[0] == '\0')
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, F_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	path_var = get_path_variable(envp);
	if (!path_var)
		return (NULL);
	paths = ft_split(path_var, ':');
	if (!paths)
		return (NULL);
	i = -1;
	while (paths[++i])
	{
		full_path = ft_get_total_path(paths[i], (char *)cmd);
		if (full_path && access(full_path, X_OK) == 0)
		{
			ft_split_clean(&paths);
			return (full_path);
		}
		free(full_path);
	}
	ft_split_clean(&paths);
	return (NULL);
}

static void	handle_execve_error(char **argv, char *full_path)
{
	fprintf(stderr, "minishell: %s: ", argv[0]);
	if (errno == EACCES)
		fprintf(stderr, "Permission denied\n");
	else
		perror(NULL);
	free(full_path);
	free_argv(argv);
	if (errno == EACCES)
		exit(126);
	exit(127);
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

static void	execute_simple_command(t_token *cmd_node, char ***envp, int l_exit)
{
	char	**argv;
	char	*full_path;

	argv = build_argv_from_ast(cmd_node, envp, l_exit);
	if (!argv)
		exit(0);
	check_for_directory(argv);
	full_path = get_full_path(argv[0], *envp);
	if (!full_path)
	{
		fprintf(stderr, "minishell: %s: command not found\n", argv[0]);
		free_argv(argv);
		exit(127);
	}
	execve(full_path, argv, *envp);
	handle_execve_error(argv, full_path);
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
void	dispatch_command_execution(t_token *cmd_node, char ***envp, int l_exit)
{
	if (!cmd_node)
		exit(0);
	if (cmd_node->token == PIPE)
		exit(execute_pipe(cmd_node, envp));
	else if (cmd_node->token == BUILTIN)
		exit(ft_execute_builtin(cmd_node, envp, l_exit));
	else if (cmd_node->token == CMD || cmd_node->token == WORD
		|| cmd_node->token == VAR)
		execute_simple_command(cmd_node, envp, l_exit);
	else
		handle_unknown_command(cmd_node, envp);
}
