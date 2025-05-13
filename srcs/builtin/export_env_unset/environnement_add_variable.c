/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environnement_add_variable.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ast <ast@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:17:51 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/05 12:43:22 by ast              ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../builtin.h"


void	clean_swap(char **new, char **old)
{
	char *tmp;

	tmp = *old;
	*old = *new;
	free(tmp);
}

/*
	check if the format of the new vaariable is $var and if var is on env. 
	If the focntion find the variable on env, she check the format of the variable.
*/
static char	*process_new_variable(char **old_env, char *new_var)
{
	char	*var;

	if (ft_index_of_char(new_var, '=') == -1
		|| new_var[ft_index_of_char(new_var, '=') + 1] == '\0')
	{
		if (new_var[0] == '$')
		{
			
			var = ft_get_env_variable(old_env, new_var);
			if (!var || var[0] == '\0')
				return (NULL);
			var = ft_strdup(ft_strchr(var, '=') + 1);
			if (ft_index_of_char(var, '=') == -1)
			{
				if (ft_isalpha(var[0]))
					return (var);
				else
					printf("bash: export: `%s': not a valid identifier\n", var);
				return (NULL);
			}
			else
				return (var);
		}
		else
			return(ft_strdup(new_var));
	}
	return (ft_strdup(new_var));
}

/*
	check if the variable is on env;
*/
int	ft_is_variable_on_env(char **env, char *var)
{
	int	i;
	int	idx;

	i = 0;
	while (env[i])
	{
		//printf("voici %s et %d\n", env[i],i);
		idx = ft_index_of_char(env[i], '=');
		if (ft_str_env_cmp(env[i], var, idx) == 0)
			return (i);
		i++;
	}
	return (-1);
}

/*
	push the old variable in the new env;
*/
static char	**process_old_variable(char **old_env, char **new_var, size_t *j)
{
	size_t	i;
	int		env_len;
	char	**new_env;

	env_len = ft_get_split_len(old_env) + ft_get_split_len(new_var);
	new_env = malloc(sizeof(char *) * (env_len + 1));
	if (!new_env)
		return (NULL);
	ft_memset(new_env,0, sizeof(char **) * env_len);
	i = 0;
	while (i < ft_get_split_len(old_env))
		new_env[(*j)++] = ft_strdup(old_env[i++]);
	new_env[*j] = NULL;
	return (new_env);
}

char	**ft_add_variable_to_env(char **old_env, char **new_var)
{
	size_t	i;
	size_t	j;
	char	**new_env;
	char	*tmp;

	j = 0;
	i = 0;
	new_env = process_old_variable(old_env, new_var, &j);
	if (!new_env)
		return (NULL);
	while (i < ft_get_split_len(new_var))
	{
		tmp = process_new_variable(old_env, new_var[i]);
		if (tmp)
		{
			if(!ft_check_variable_export(tmp, old_env))
			{
				i++;
				continue;
			}
			if (ft_is_variable_on_env(new_env, tmp) != -1
				&& ft_index_of_char(tmp, '=') != -1)
			{
				clean_swap(&tmp, &new_env[ft_is_variable_on_env(new_env, tmp)]);
			}
			else
			{
				new_env[j++] = tmp;
			}
		}
		i++;
	}
	new_env[j] = NULL;
	return (new_env);
}
