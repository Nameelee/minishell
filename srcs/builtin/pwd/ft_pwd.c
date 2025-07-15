/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cw3l <cw3l@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 09:30:50 by cw3l              #+#    #+#             */
/*   Updated: 2025/05/10 09:38:45 by cw3l             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin.h"

int	ft_pwd(char **processed_argv)
{
	int		i;
	char	path[10240];

	if (!processed_argv)
		return (1);
	if (!getcwd(path, sizeof(path)))
	{
		printf("error get path %p\n", path);
		return (1);
	}
	i = 0;
	while (path[i])
	{
		write(1, &path[i], 1);
		i++;
	}
	write(1, "\n", 1);
	return (0);
}
