/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environnement_tools.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ast <ast@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 13:27:38 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/04/24 23:28:36 by ast              ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../builtin.h"

int	ft_get_var_idx_in_env(char **envp, char *var)
{
	char	*env_var;
	char *tmp;
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
	return (NULL); // change from "" to NULL;
}