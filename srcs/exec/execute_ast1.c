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

/**
 * @brief Searches for the command in the directories specified by the
 * PATH environment variable.
 * @return An allocated string of the full path if found, otherwise NULL.
 */
static char	*find_in_path_variable(const char *cmd, char **envp)
{
	char	**paths;
	char	*path_var;
	char	*full_path;
	int		i;

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

/**
 * @brief Finds the full, executable path for a given command by checking for
 * a direct path first, then searching the PATH variable.
 * @return An allocated string of the full path, or NULL if not found.
 */
static char	*get_full_path(const char *cmd, char **envp)
{
	if (!cmd || cmd[0] == '\0')
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, F_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	return (find_in_path_variable(cmd, envp));
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
