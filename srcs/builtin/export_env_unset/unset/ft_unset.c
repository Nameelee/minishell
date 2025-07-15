/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cw3l <cw3l@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 10:52:29 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/10 09:50:17 by cw3l             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../builtin.h"

static int	is_valid_identifier(char *var)
{
	int	i;

	i = 0;
	if (!var || (!ft_isalpha(var[0]) && var[0] != '_'))
	{
		ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
		ft_putstr_fd(var, STDERR_FILENO);
		ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
		return (0);
	}
	while (var[i])
	{
		if (!ft_isalnum(var[i]) && var[i] != '_')
		{
			ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
			ft_putstr_fd(var, STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			return (0);
		}
		i++;
	}
	return (1);
}

static int	find_var_index(char **envp, const char *var_to_find)
{
	int		i;
	char	*var_name;

	i = 0;
	while (envp[i])
	{
		var_name = ft_parse_env_variable(envp[i]);
		if (var_name && ft_strncmp(var_name, var_to_find,
				ft_strlen(var_to_find) + 1) == 0)
		{
			free(var_name);
			return (i);
		}
		free(var_name);
		i++;
	}
	return (-1);
}

static void	remove_var_at_index(char ***envp, int index_to_remove)
{
	int	i;

	free((*envp)[index_to_remove]);
	i = index_to_remove;
	while ((*envp)[i])
	{
		(*envp)[i] = (*envp)[i + 1];
		i++;
	}
}

int	ft_unset(char **args, char ***envp)
{
	int	i;
	int	var_index;
	int	ret_status;

	i = 1;
	ret_status = 0;
	while (args[i])
	{
		if (is_valid_identifier(args[i]))
		{
			var_index = find_var_index(*envp, args[i]);
			if (var_index != -1)
			{
				remove_var_at_index(envp, var_index);
			}
		}
		else
		{
			ret_status = 1;
		}
		i++;
	}
	return (ret_status);
}
