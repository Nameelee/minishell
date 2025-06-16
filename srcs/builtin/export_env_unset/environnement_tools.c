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
	char	*env_var_name;
	char	*tmp_env_entry_dup;
	int		i;
	size_t	var_len;
	size_t	env_var_name_len;

	i = 0;
	if (!envp || !var)
		return (-1);
	var_len = ft_strlen(var);
	while (envp[i])
	{
		tmp_env_entry_dup = ft_strdup(envp[i]);
		if (!tmp_env_entry_dup)
		{
			i++;
			continue ;
		}
		env_var_name = ft_parse_env_variable(tmp_env_entry_dup);
		if (env_var_name)
		{
			env_var_name_len = ft_strlen(env_var_name);
			if (env_var_name_len == var_len
					&& ft_strncmp(env_var_name, var, var_len) == 0)
			{
				free(tmp_env_entry_dup);
				free(env_var_name);
				return (i);
			}
			free(env_var_name);
		}
		free(tmp_env_entry_dup);
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
	int		count;
	int		i;

	if (!env)
		return ;
	dup_env = ft_duplicate_env(env);
	if (dup_env)
	{
		count = 0;
		i = 0;
		while (dup_env[i])
		{
			write(STDOUT_FILENO, dup_env[i], ft_strlen(dup_env[i]));
			write(STDOUT_FILENO, "\n", 1);
			count++;
			i++;
		}
	}
	ft_split_clean(&dup_env);
}