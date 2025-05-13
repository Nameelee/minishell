/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ast <ast@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 11:06:30 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/03 02:51:24 by ast              ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../builtin.h"

int	ft_pwd(char **processed_argv)
{
	int		i;
	char	path[10240];

	if(!processed_argv)
		return(1);
	// if (ft_get_split_len(processed_argv) > 1)
	// {
	// 	printf("pwd: too many arguments\n");
	// 	return (1);
	// }
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
	return(0);
}
