/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:29:53 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/10 12:47:18 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../builtin.h"

static int	controle_variable(char **envp, char *var)
{
	char	*find_var;
	char	*tmp;
	int		idx;

	find_var = ft_get_env_variable(envp, var);
	if (find_var)
	{
		tmp = ft_strchr(find_var, '=') + 1;
		if (tmp && tmp[0] != '\0')
		{
			idx = ft_index_of_c(tmp, '=');
			if (idx == -1 && find_var[0] != '\0')
			{
				printf("env: «%s»: No such file or directory\n", tmp);
				return (127);
			}
		}
		else
			return (0);
	}
	return (0);
}

static int	ft_check_variable_env(char **split_args, char **envp)
{
	int		i;
	int		controle_return;

	i = 0;
	while (split_args[i])
	{
		if (ft_index_of_c(split_args[i], '=') == -1)
		{
			if (split_args[i][0] != '$')
			{
				printf("env: '%s': No such file or directory\n", split_args[i]);
				return (1);
			}
			else
			{
				controle_return = controle_variable(envp, split_args[i]);
				if (controle_return)
					return (controle_return);
			}
		}
		i++;
	}
	return (0);
}

static int	display_env_env(char ***env)
{
	ft_print_env(*env);
	return (0);
}

static int	add_variable_ev(char **env, char **split_args)
{
	char	**tmp_env;
	char	exit_return;

	exit_return = ft_check_variable_env(&split_args[1], env);
	if (!exit_return)
	{
		tmp_env = ft_add_variable_to_env(env, &split_args[1]);
		if (!tmp_env)
			return (1);
		ft_split_print(tmp_env);
		ft_split_clean(&tmp_env);
	}
	return (exit_return);
}

int	ft_env(char **split_args, char ***env)
{
	int	return_value;

	if (!(*env || !split_args))
		return (1);
	if (!split_args[1] || ft_strncmp(split_args[1], "", 1) == 0)
		return (display_env_env(env));
	return_value = add_variable_ev(*env, split_args);
	return (return_value);
}
