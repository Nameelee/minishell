/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environnement_add_variable.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:17:51 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/16 13:08:42 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin.h"

static char	*process_new_variable(char **old_env, char *new_var)
{
	char	*var;

	if (ft_index_of_c(new_var, '=') == -1
		|| new_var[ft_index_of_c(new_var, '=') + 1] == '\0')
	{
		if (new_var[0] == '$')
		{
			var = ft_get_env_variable(old_env, new_var);
			if (!var || var[0] == '\0')
				return (NULL);
			var = ft_strdup(ft_strchr(var, '=') + 1);
			if (ft_index_of_c(var, '=') == -1)
			{
				if (ft_isalpha(var[0]))
					return (var);
				return (print_add_variable_msg(var));
			}
			else
				return (var);
		}
		else
			return (ft_strdup(new_var));
	}
	return (ft_strdup(new_var));
}

int	ft_is_var_on(char **env, char *var)
{
	int	i;
	int	idx;

	i = 0;
	while (env[i])
	{
		idx = ft_index_of_c(env[i], '=');
		if (ft_str_env_cmp(env[i], var, idx) == 0)
			return (i);
		i++;
	}
	return (-1);
}

static char	**process_old_variable(char **old_env, char **new_var, size_t *j)
{
	size_t	i;
	int		env_len;
	char	**new_env;

	env_len = ft_get_split_len(old_env) + ft_get_split_len(new_var);
	new_env = malloc(sizeof(char *) * (env_len + 1));
	if (!new_env)
		return (NULL);
	ft_memset(new_env, 0, sizeof(char **) * env_len);
	i = 0;
	while (i < ft_get_split_len(old_env))
		new_env[(*j)++] = ft_strdup(old_env[i++]);
	new_env[*j] = NULL;
	return (new_env);
}

int	add_new_variable(char **old_ev, char **new_ev, char *new_var, size_t *idx)
{
	char	*tmp;

	tmp = process_new_variable(old_ev, new_var);
	if (tmp)
	{
		if (ft_check_variable_export(tmp, old_ev))
		{
			if (ft_is_var_on(new_ev, tmp) != -1
				&& ft_index_of_c(tmp, '=') != -1)
				clean_swap(&tmp, &new_ev[ft_is_var_on(new_ev, tmp)]);
			else
				new_ev[(*idx)++] = tmp;
			return (0);
		}
	}
	return (1);
}

char	**ft_add_variable_to_env(char **old_env, char **new_var)
{
	size_t	i;
	size_t	j;
	char	**new_env;

	j = 0;
	i = 0;
	new_env = process_old_variable(old_env, new_var, &j);
	if (!new_env)
		return (NULL);
	while (i < ft_get_split_len(new_var))
	{
		add_new_variable(old_env, new_env, new_var[i], &j);
		i++;
	}
	new_env[j] = NULL;
	return (new_env);
}
