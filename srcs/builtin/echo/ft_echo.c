/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 12:15:59 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/16 12:52:31 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin.h"

void	print_echo(char *str, char **envp)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1] != '?'
			&& str[i + 1] != '\0' && str[i + 1] != 32)
		{
			if (ft_print_variable(envp, &str[i++], &i) == -1)
				break ;
		}
		else if (str[i] == '$' && str[i + 1] == '?')
			ft_print_variable_int_str(&str[i++]);
		else
			write(STDOUT_FILENO, &str[i], 1);
		i++;
	}
}

int	process_echo(char **split_args, char **envp, int n)
{
	int	j;

	j = 0;
	while (split_args[j])
	{
		print_echo(split_args[j], envp);
		if (split_args[j + 1])
			write(STDOUT_FILENO, " ", 1);
		j++;
	}
	if (!n)
		write(STDOUT_FILENO, "\n", 1);
	return (1);
}

void	process_single_echo(char **split_args, int n)
{
	int		i;
	int		j;
	char	*str;

	i = 0;
	while (split_args[i])
	{
		j = 0;
		str = split_args[i];
		while (str[j])
		{
			write(STDOUT_FILENO, &str[j], 1);
			j++;
		}
		if (split_args[i + 1])
			write(STDOUT_FILENO, " ", 1);
		i++;
	}
	if (!n)
		write(STDOUT_FILENO, "\n", 1);
}

int	ft_echo(char **split_args, int squote, int dquote, char **envp)
{
	int		i;
	int		arg;

	(void)dquote;
	(void)squote;
	i = 1;
	arg = 0;
	if (!split_args || !*split_args)
		return (1);
	if (!split_args[i])
		write(1, "\n", 1);
	else
	{
		if (!ft_strncmp(split_args[i], "-n",
				ft_strlen_longest(split_args[i], "-n")))
		{
			arg = 1;
			i++;
		}
		if (squote == 0)
			process_echo(&split_args[i], envp, arg);
		else
			process_single_echo(&split_args[i], arg);
	}
	return (0);
}
