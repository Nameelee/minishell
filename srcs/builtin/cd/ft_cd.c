/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cw3l <cw3l@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 09:29:37 by cw3l              #+#    #+#             */
/*   Updated: 2025/05/10 09:29:39 by cw3l             ###   ########.fr       */
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
		total_path = env;
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

int	ft_cd(char **args)
{
	int		chdir_return;
	char	*path;

	if (ft_check_path(&args[1]) == 2)
		return (0);
	else if (ft_check_path(&args[1]) == 0)
		return (print_cd_message(0));
	if (ft_get_split_len(args) == 1)
		return (ft_return_to_home(args[0]));
	else
	{
		path = ft_trim_path(args[1]);
		if (path[0] == '~' )
			path = ft_get_home_path(path, 0);
	}
	chdir_return = chdir(path);
	if (chdir_return == -1)
		return (print_cd_message(1));
	return (0);
}
