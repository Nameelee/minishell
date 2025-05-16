/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environnement_variable_tools.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 09:39:42 by cw3l              #+#    #+#             */
/*   Updated: 2025/05/16 12:52:42 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin.h"

char	*ft_parse_env_variable(char *env_var)
{
	char	*var;
	int		equal_idx;

	if (!env_var)
		return (NULL);
	equal_idx = ft_index_of_c(env_var, '=');
	if (equal_idx == -1)
		return (ft_strdup(env_var));
	var = ft_substr(env_var, 0, equal_idx);
	return (var);
}

char	*ft_extract_var(char *env_var)
{
	char	*var;
	int		equal_idx;

	if (!env_var)
		return (NULL);
	equal_idx = ft_index_of_c(env_var, '=');
	if (equal_idx == -1)
		return (ft_strdup(env_var));
	var = ft_substr(env_var, 0, equal_idx);
	if (!var)
		return (NULL);
	return (var);
}

int	ft_is_variable(char *str)
{
	if (!str)
		return (0);
	if (str[0] == '$')
		return (1);
	return (0);
}

int	ft_print_variable_int_str(char *str)
{
	char	*code;

	if (str[1] == '?')
	{
		code = ft_itoa(g_exit_status);
		write(STDOUT_FILENO, code, ft_strlen(code));
		free(code);
		return (0);
	}
	return (1);
}

int	ft_print_variable(char **envp, char *str, int *idx)
{
	int		j;
	char	*var;
	char	**split;

	split = ft_split(str, 32);
	var = ft_get_env_variable(envp, split[0]);
	if (!var)
		return (-1);
	j = ft_index_of_c(var, '=') + 1;
	while (var[j])
	{
		write(STDOUT_FILENO, &var[j], 1);
		j++;
	}
	(*idx) += (size_t)ft_strlen(split[0]) - 2;
	return (0);
}
