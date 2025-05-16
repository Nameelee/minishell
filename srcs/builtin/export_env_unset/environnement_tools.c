/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environnement_tools.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 13:27:38 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/16 12:45:50 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin.h"

void	clean_swap(char **new, char **old)
{
	char	*tmp;

	tmp = *old;
	*old = *new;
	free(tmp);
}

int	ft_get_var_idx_in_env(char **envp, char *var)
{
	char	*env_var;
	char	*tmp;
	int		i;

	i = 0;
	while (envp[i])
	{
		tmp = ft_strdup(envp[i]);
		env_var = ft_parse_env_variable(tmp);
		if (!ft_strncmp(env_var, var, ft_strlen_longest(env_var, var)))
		{
			free(tmp);
			free(env_var);
			return (i);
		}
		free(tmp);
		free(env_var);
		i++;
	}
	return (-1);
}

char	*ft_get_env_variable(char **envp, char *var)
{
	int	idx;

	if (!ft_is_variable(var))
		return (NULL);
	idx = ft_get_var_idx_in_env(envp, &var[1]);
	if (idx > -1)
		return (envp[idx]);
	return (NULL);
}

void	ft_print_env(char **env)
{
	char	**dup_env;

	if (!env)
		return ;
	dup_env = ft_duplicate_env(env);
	ft_split_print(dup_env);
	ft_split_clean(&dup_env);
}
