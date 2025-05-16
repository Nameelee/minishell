/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_error.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 09:28:07 by cw3l              #+#    #+#             */
/*   Updated: 2025/05/10 12:08:01 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin.h"

void	print_exit_message(int code)
{
	if (code == 0)
	{
		write(STDERR_FILENO, " numeric argument required\n",
			ft_strlen(" numeric argument required\n"));
		exit(2);
	}
	if (code == 1)
	{
		write(STDERR_FILENO, " too many arguments\n",
			ft_strlen(" too many arguments\n"));
		exit(1);
	}
}

int	print_cd_message(int code)
{
	if (code == 0)
	{
		write(STDERR_FILENO, " too many arguments\n",
			ft_strlen(" too many arguments\n"));
		return (1);
	}
	if (code == 1)
	{
		write(STDERR_FILENO, " No such file or directory\n",
			ft_strlen(" No such file or directory\n"));
		return (1);
	}
	return (0);
}

int	print_export_message(int code)
{
	if (code == 0)
	{
		write(STDERR_FILENO, " not a valid identifier\n",
			ft_strlen(" not a valid identifier\n"));
		return (0);
	}
	return (0);
}

void	*print_add_variable_msg(char *var)
{
	printf("bash: export: `%s': not a valid identifier\n", var);
	return (NULL);
}
