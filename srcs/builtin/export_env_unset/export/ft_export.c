/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ast <ast@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 10:52:29 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/05 12:59:37 by ast              ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../../builtin.h"

int	ft_check_variable_export(char *var, char **envp)
{
	int		i;
	char	*tmp;

	if (!var)
		return (0);
	i = 0;
	if(ft_strncmp("?=", var, 2) == 0)
		return(1);
	while (ft_isalpha(var[i]))
		i++;
	if (i > 0 && var[i] == '\0')
		return (1);
	if (i == 0 && var[i] != '$')
		return (0);
	if((i > 0 && !ft_isalnum(var[i]) && var[i] != '=') || (i == 0 && var[i] == '\0'))
	{
		write(STDERR_FILENO, " identifiant non valable\n", ft_strlen(" identifiant non valable\n"));
		return(0);
	}
	else if (i == 0 && var[i] == '$')
	{
		tmp = ft_get_env_variable(envp, var);
		if (!tmp)
			return (0);
		var = &tmp[ft_index_of_char(tmp, '=') + 1];
	}
	return (1);
}

int	ft_count_valide_variable(char **var, char **envp, int *err)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	if (!var)
		return (0);
	while (var[i])
	{
		if (ft_check_variable_export(var[i], envp))
			count++;
		else
		{
			g_exit_status = 1;
			//printf("bash: export: « %s » : identifiant non valable\n", var[i]);
			(*err)++;
		}
		i++;
	}
	return (count);
}
/*
	add variable export to env
*/

static char	**add_variable_ex(char ***env, char **split_args)
{
	int		valide_variable_len;
	char	**new_env;
	int 	err;

	valide_variable_len = ft_count_valide_variable(split_args, *env, &err);
	if (valide_variable_len)
	{
		new_env = ft_add_variable_to_env(*env, split_args);
		if (!new_env)
			return (0);
		else
		{
			//ft_split_clean(env); Nami
			return (new_env);
		}
	}
	else
		return(NULL);
	return (*env);
}

int	ft_export(char ***env, char **split_args)
{
	char **tmp_env;
	
	if (!env || !*env || !split_args )
		return (1);
	if (!split_args[1] || ft_strncmp(split_args[1], "", 1) == 0)
	{
		//ft_split_clean(&split_args);
		return (display_export_env(env));
	}
	tmp_env = add_variable_ex(env, &split_args[1]);
	if(!tmp_env)
		return(1);
	*env = tmp_env;
	//ft_split_clean(&split_args);
	return (0);
}
