/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environnement_variable_tools.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ast <ast@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 12:57:45 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/03 03:35:53 by ast              ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../builtin.h"

char	*ft_parse_env_variable(char *env_var)
{
	char	*var;
	int		equal_idx;

	if (!env_var)
		return (NULL);
	equal_idx = ft_index_of_char(env_var, '=');
	if (equal_idx == -1)
		return (ft_strdup(env_var));
	var = ft_substr(env_var, 0, equal_idx);
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
