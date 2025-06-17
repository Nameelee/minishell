/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   ft_cd.c                                             :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/17 14:46:39 by jelee          #+#    #+#                */
/*   Updated: 2025/06/17 14:46:42 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin.h"

/**
 * @brief Determines the target path for the 'cd' command.
 * @param args The arguments passed to 'cd'.
 * @return Allocated string representing the target path, or NULL on error.
 */
static char	*get_target_path(char **args)
{
	char	*path_arg;

	path_arg = args[1];
	if (path_arg == NULL || path_arg[0] == '\0'
		|| (path_arg[0] == '~' && (path_arg[1] == '\0' || path_arg[1] == '/')))
	{
		return (ft_get_home_path(path_arg,
				(path_arg == NULL || path_arg[0] == '\0')));
	}
	else
	{
		return (ft_strdup(path_arg));
	}
}

/**
 * @brief Prints a formatted error message for a failed 'cd' command.
 * @param original_arg The original path argument given by the user.
 * @param used_path The path that was actually passed to the chdir().
 */
static void	print_cd_error(char *original_arg, char *used_path)
{
	ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
	if (original_arg && (original_arg[0] == '~'
			|| ft_strchr(original_arg, '$')))
		ft_putstr_fd(original_arg, STDERR_FILENO);
	else
		ft_putstr_fd(used_path, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	perror(NULL);
}

/**
 * @brief The 'cd' (change directory) builtin command.
 * @param args The arguments array, where args[0] is "cd".
 * @return 0 on success, 1 on failure.
 */
int	ft_cd(char **args)
{
	char	*path_to_go;
	int		return_status;

	if (ft_get_split_len(args) > 2)
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	path_to_go = get_target_path(args);
	if (!path_to_go)
		return (1);
	if (chdir(path_to_go) == -1)
	{
		print_cd_error(args[1], path_to_go);
		return_status = 1;
	}
	else
	{
		return_status = 0;
	}
	free(path_to_go);
	return (return_status);
}
