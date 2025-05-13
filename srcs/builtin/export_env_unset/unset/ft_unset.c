/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ast <ast@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 10:52:29 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/03 21:50:33 by ast              ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../../builtin.h"

static int	is_unset_valide_variable(char *var)
{
	int	i;

	i = 0;
	while (var[i])
	{
		if (!ft_isalnum(var[i]))
		{
			printf("unset: %s: invalid parameter name\n", var);
			return (0);
		}
		i++;
	}
	return (1);
}

int	ft_count_unset_valide_variable(char **var)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (var[i])
	{
		if (is_unset_valide_variable(var[i]))
			count++;
		i++;
	}
	return (count);
}

int	ft_delete_from_env(char *env, char **var)
{
	int	i;

	i = 0;
	if (!env)
		return (-1);
	while (var[i] != NULL)
	{
		if(env && var[i])
		{
			if (ft_strncmp(env, var[i], ft_strlen(var[i])) == 0)
			{
				return (i);
			}
		}
		i++;
	}
	return (-1);
}

char	**ft_delete_variable(char **old_env, char **del_var, int valide_var)
{
	size_t	i;
	size_t	j;
	size_t	env_len;
	char	**new_env;

	env_len = ft_get_split_len(old_env) - valide_var;
	new_env = malloc(sizeof(char *) * (env_len + 3));
	if (!new_env)
		return (NULL);
	i = 0;
	j = 0;
	while (i < ft_get_split_len(old_env))
	{
		if (ft_delete_from_env(old_env[i], del_var) != -1)
		{
			free(old_env[i]);
			i++;
			continue ;
		}
		else
			new_env[j++] = old_env[i++];
	}
	new_env[j] = NULL;
	return (new_env);
}

int	    ft_unset(char **var, char ***envp)
{
	char	**new_env;
	int		valide_variable;

	
	valide_variable = ft_count_unset_valide_variable(&var[1]);
	new_env = ft_delete_variable(*envp, &var[1], valide_variable);
	if (!new_env)
	{
		free(var);
		return(1);
	}
	env_quick_s(new_env, ft_get_split_len(new_env), ft_str_env_cmp);
	free(*envp);
	*envp = new_env;
	return(0);
}
