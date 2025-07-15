/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   ft_cd.c                                             :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/17 14:32:44 by jelee          #+#    #+#                */
/*   Updated: 2025/06/17 14:32:49 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin.h"

char	*ft_trim_path(char *path)
{
	int	i;
	int	len;

	len = ft_strlen(path);
	i = 0;
	while (path[i] == 32)
		i++;
	while (path[i] == 32)
		i++;
	ft_memmove(path, &path[i], len - i);
	path[len - i] = '\0';
	return (path);
}

char	*ft_get_home_path(char *path, int cd)
{
	char	*env;
	char	*total_path;

	env = getenv("HOME");
	if (!env)
		return (NULL);
	if (cd == 0)
		total_path = ft_strjoin(env, &path[1]);
	else
		total_path = ft_strdup(env);
	if (!total_path)
		return (NULL);
	return (total_path);
}

int	ft_check_path(char **split)
{
	if (ft_get_split_len(split) == 2)
		return (0);
	else if (ft_get_split_len(split) > 2)
		return (0);
	else
	{
		if (ft_get_split_len(split) == 1
			&& ft_strncmp(split[0], "$PWD", 4) == 0)
			return (2);
		if (ft_get_split_len(split) <= 2 && ft_get_split_len(split) > 0)
			return (1);
	}
	return (0);
}

int	ft_return_to_home(char *path)
{
	char	*home_path;
	int		chdir_return;

	chdir_return = 1;
	home_path = ft_get_home_path(path, 1);
	if (home_path)
		chdir_return = chdir(home_path);
	if (chdir_return == -1)
	{
		write(STDERR_FILENO, " Aucun fichier ou dossier de ce nom",
			ft_strlen(" Aucun fichier ou dossier de ce nom"));
		return (1);
	}
	return (0);
}
