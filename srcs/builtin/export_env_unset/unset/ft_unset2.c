/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   ft_unset2.c                                         :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/26 14:30:04 by jelee          #+#    #+#                */
/*   Updated: 2025/06/26 14:30:07 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

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
		if (env && var[i])
		{
			if (ft_strncmp(env, var[i], ft_strlen(var[i])) == 0)
				return (i);
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
