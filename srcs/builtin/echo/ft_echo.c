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

int ft_echo(char **split_args) // No squote, dquote, envp parameters
{
    int i = 1; // Index for arguments, split_args[0] is "echo"
    bool n_option = false;

    // Check for -n option(s)
    // Bash allows multiple -n options like "echo -n -n -n hello"
    // And "-nnnn" is also valid.
    while (split_args[i] && ft_strncmp(split_args[i], "-n", 2) == 0) {
        bool only_n_chars = true;
        for (size_t j = 1; j < ft_strlen(split_args[i]); ++j) {
            if (split_args[i][j] != 'n') {
                only_n_chars = false;
                break;
            }
        }
        if (only_n_chars && ft_strlen(split_args[i]) >=2) { // Must be at least "-n"
            n_option = true;
            i++;
        } else {
            break; // Not a valid -n sequence, stop processing -n options
        }
    }

    int arg_idx = i;
    while (split_args[arg_idx]) {
        ft_putstr_fd(split_args[arg_idx], STDOUT_FILENO);
        if (split_args[arg_idx + 1]) {
            ft_putstr_fd(" ", STDOUT_FILENO);
        }
        arg_idx++;
    }

    if (!n_option) {
        ft_putstr_fd("\n", STDOUT_FILENO);
    }
    return (0);
}