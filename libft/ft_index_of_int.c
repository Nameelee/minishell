/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_index_of_int.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cw3l <cw3l@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 12:14:07 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/04/12 09:10:08 by cw3l             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <assert.h>

int	ft_index_of_int(int *arr, int len, int n)
{
	int	i;

	i = 0;
	while (i < len)
	{
		if (arr[i] == n)
			return (i);
		i++;
	}
	return (-1);
}
