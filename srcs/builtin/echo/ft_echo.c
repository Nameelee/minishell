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

/**
 * @brief Checks if a single argument string is a valid "-n" option.
 * A valid option starts with '-' and is followed by one or more 'n' characters.
 * @param arg The argument string to check.
 * @return True if the argument is a valid "-n" option, false otherwise.
 */
static bool	is_valid_n_option(const char *arg)
{
	size_t	i;

	if (arg[0] != '-' || arg[1] != 'n')
		return (false);
	i = 1;
	while (arg[i])
	{
		if (arg[i] != 'n')
			return (false);
		i++;
	}
	return (true);
}

/**
 * @brief Parses arguments to handle all valid "-n" options at the start.
 * @param args The full argument array.
 * @param n_option_flag A pointer to a boolean that is set to true if any
 * valid -n option is found.
 * @return The index of the first argument that is not an "-n" option.
 */
static int	parse_echo_options(char **args, bool *n_option_flag)
{
	int	i;

	i = 1;
	*n_option_flag = false;
	while (args[i] && is_valid_n_option(args[i]))
	{
		*n_option_flag = true;
		i++;
	}
	return (i);
}

/**
 * @brief The echo command implementation. Handles the -n option.
 * @param split_args The command and its arguments.
 * @return Always returns 0.
 */
int	ft_echo(char **split_args)
{
	int		arg_idx;
	bool	n_option;

	arg_idx = parse_echo_options(split_args, &n_option);
	while (split_args[arg_idx])
	{
		ft_putstr_fd(split_args[arg_idx], STDOUT_FILENO);
		if (split_args[arg_idx + 1])
			ft_putstr_fd(" ", STDOUT_FILENO);
		arg_idx++;
	}
	if (!n_option)
		ft_putstr_fd("\n", STDOUT_FILENO);
	return (0);
}
