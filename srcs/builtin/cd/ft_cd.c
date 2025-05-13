/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ast <ast@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:30:49 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/03 22:38:41 by ast              ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../builtin.h"



char *ft_trim_path(char *path)
{
	int i;
	int len;

	len = ft_strlen(path);
	i = 0;
	while (path[i] == 32)
		i++;
	while (path[i] == 32)
		i++;
	ft_memmove(path,&path[i],len - i);
	path[len - i] = '\0';
	return(path);
}

char	*ft_get_home_path(char *path, int cd)
{
	char	*env;
	char	*total_path;

	env = getenv("HOME");
	if (!env)
		return (NULL);
	if(cd == 0)
		total_path = ft_strjoin(env, &path[1]);
	else
		total_path = env;
	if (!total_path)
		return (NULL);
	return (total_path);
}

int ft_check_path(char **split)
{	
	if(ft_get_split_len(split) == 2)
		return(0);
	else if(ft_get_split_len(split) > 2)
		return(0);
	else
	{
		if(ft_get_split_len(split) == 1 && ft_strncmp(split[0], "$PWD", 4) == 0)
			return(2);
		if(ft_get_split_len(split) <=2 && ft_get_split_len(split) > 0)
			return(1);
	}
	return(0);
}

int ft_return_to_home(char *path)
{
	char	*home_path;
	int		chdir_return;
	
	chdir_return = 1;
	home_path = ft_get_home_path(path, 1);
	if(home_path)
		chdir_return = chdir(home_path);
	if (chdir_return == -1)
	{
		write(STDERR_FILENO, " Aucun fichier ou dossier de ce nom", ft_strlen(" Aucun fichier ou dossier de ce nom"));
		return (1);
	}
	return(0);
	
}

int	ft_cd(char **args)
{
	int		chdir_return;
	char 	*path;
	/* 
		first i check if the path is valide and display the specific message if not.
		valide arg: cd + 1 argument.
	*/

	if(ft_check_path(&args[1])  == 2)
		return(0);
	else if (ft_check_path(&args[1]) == 0)
	{
		write(STDERR_FILENO, " trop d'arguments\n", ft_strlen(" trop d'arguments\n"));
		return(1);
	}
	/* 
		after, i check if cd had argument. If not, we go home.
	*/
	if(ft_get_split_len(args) == 1)
		return(ft_return_to_home(args[0]));
	else
	{
		/* 
			We need to separate the cd command from the path and use only the path.
		*/
		path = ft_trim_path(args[1]);
		if (path[0] == '~' )
		{
			/* 
				if the commande had argument, if the argument start by ~ , the home path is the start point.
			*/
			path = ft_get_home_path(path, 0);
		}
	}
	/* 
		follow de path. see man chdir, check the return value. Display error message if necessary.
	*/
	chdir_return = chdir(path);
	if (chdir_return == -1)
	{
		write(STDERR_FILENO, " Aucun fichier ou dossier de ce nom", ft_strlen(" Aucun fichier ou dossier de ce nom"));
		return (1);
	}
	return (0);
}
