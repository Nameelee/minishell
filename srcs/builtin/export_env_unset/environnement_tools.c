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

/**
 * @brief Checks if a environment entry matches a given variable name.
 * @param env_entry A string from the environment array (e.g., "PATH=...").
 * @param var The variable name to search for (e.g., "PATH").
 * @param var_len The pre-calculated length of the 'var' name.
 * @return Returns true if the variable names match, false otherwise.
 */
static bool	is_matching_env_var(const char *env_entry, const char *var,
				size_t var_len)
{
	char	*tmp_dup;
	char	*env_var_name;
	bool	is_match;

	is_match = false;
	tmp_dup = ft_strdup(env_entry);
	if (!tmp_dup)
		return (false);
	env_var_name = ft_parse_env_variable(tmp_dup);
	if (env_var_name)
	{
		if (ft_strlen(env_var_name) == var_len
			&& ft_strncmp(env_var_name, var, var_len) == 0)
		{
			is_match = true;
		}
		free(env_var_name);
	}
	free(tmp_dup);
	return (is_match);
}

/**
 * @brief Finds the index of a variable within the environment array.
 * @param envp The environment array (e.g., {"PATH=...", "USER=...", NULL}).
 * @param var The variable name to find (e.g., "USER").
 * @return The index of the variable if found, otherwise -1.
 */
int	ft_get_var_idx_in_env(char **envp, char *var)
{
	int		i;
	size_t	var_len;

	if (!envp || !var)
		return (-1);
	i = 0;
	var_len = ft_strlen(var);
	while (envp[i])
	{
		if (is_matching_env_var(envp[i], var, var_len))
			return (i);
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
