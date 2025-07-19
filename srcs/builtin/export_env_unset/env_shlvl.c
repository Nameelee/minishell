/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_shlvl.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jelee <marvin@42lausanne.ch>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/19 15:23:48 by jelee             #+#    #+#             */
/*   Updated: 2025/07/19 15:23:54 by jelee            ###   ####lausanne.ch   */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin.h"

/**
 * @brief  Finds and returns the current shell level from environment variables.
 * @param  envp The environment variable array.
 * @return The integer value of SHLVL, or 0 if not found.
 */
int	ft_get_shlvl(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "SHLVL=", 6) == 0)
		{
			return (ft_atoi(&envp[i][6]));
		}
		i++;
	}
	return (0);
}

/**
 * @brief  Sets the SHLVL environment variable to a new value.
 * @param  new_level The new integer value for the shell level.
 * @param  envp_ptr  A pointer to the environment variable array.
 */
void	ft_set_shlvl(int new_level, char ***envp_ptr)
{
	char	*shlvl_str;
	char	*new_shlvl_export_str;
	char	*export_args[3];

	shlvl_str = ft_itoa(new_level);
	if (!shlvl_str)
		return ;
	new_shlvl_export_str = ft_strjoin("SHLVL=", shlvl_str);
	free(shlvl_str);
	if (!new_shlvl_export_str)
		return ;
	export_args[0] = "export";
	export_args[1] = new_shlvl_export_str;
	export_args[2] = NULL;
	ft_export(envp_ptr, export_args);
	free(new_shlvl_export_str);
}
