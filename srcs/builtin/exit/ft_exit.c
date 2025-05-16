/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cw3l <cw3l@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 09:30:03 by cw3l              #+#    #+#             */
/*   Updated: 2025/05/10 09:30:14 by cw3l             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin.h"

int	is_numeric(const char *str)
{
	if (!str || *str == '\0')
		return (0);
	if (*str == '+' || *str == '-')
		str++;
	while (*str)
	{
		if (!isdigit((unsigned char)*str))
			return (0);
		str++;
	}
	return (1);
}

char	*ft_process_split(char *str)
{
	char	new[7];
	int		i;
	int		j;

	i = 0;
	j = 0;
	while (str[i])
	{
		if (ft_isdigit(str[i]))
			new[j++] = str[i];
		i++;
	}
	new[j] = '\0';
	return (ft_strdup(new));
}

void	ft_exit(char **split)
{
	if (!split)
		exit(0);
	if (split[1])
	{
		if (!is_numeric(split[1]))
			print_exit_message(0);
		if (split[2])
		{
			if (ft_strncmp(split[1], "+", 1) == 0
				|| ft_strncmp(split[1], "-", 1) == 0)
			{
				if (split[2])
				{
					if (ft_process_split(split[2]) && split[1][0] == '+')
						exit(ft_atoi(ft_process_split(split[2])));
					else
						exit(156);
				}
			}
			print_exit_message(1);
		}
		exit(atoi(split[1]));
	}
	exit(g_exit_status);
}
