/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 10:32:32 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/09 16:49:57 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "tokenizer/tokenize.h"

bool	line_is_empty_or_whitespace(const char *str)
{
	if (!str)
		return (true);
	while (*str)
	{
		if (*str != ' ' && *str != '\t' && *str != '\n'
			&& *str != '\v' && *str != '\f' && *str != '\r')
			return (false);
		str++;
	}
	return (true);
}

static t_token	*process_line(char *line)
{
	t_token	*ast_root;

	if (line[0] != '\0')
		add_history(line);
	ast_root = ft_parse(line);
	if (!ast_root)
	{
		if (!line_is_empty_or_whitespace(line))
			ft_fprintf("minishell: ", "", ": syntax error\n");
		return (NULL);
	}
	return (ast_root);
}

/**
 * @brief Parses and executes a single line of input.
 * @param line The line read from the prompt.
 * @param envp_ptr A pointer to the environment variables array.
 * @param exit_status_ptr A pointer to the shell's exit status.
 * @return The final exit code if the shell should terminate, or -1 to continue.
 */
static int	process_input_line(char *line, char ***envp_ptr,
				int *exit_status_ptr)
{
	t_token	*ast_root;
	int		new_status;

	ast_root = process_line(line);
	if (ast_root)
	{
		new_status = execute_ast(ast_root, envp_ptr, true, *exit_status_ptr);
		free_ast(ast_root);
		if (new_status >= 256)
			return (new_status - 256);
		*exit_status_ptr = new_status;
	}
	else
	{
		if (line_is_empty_or_whitespace(line))
			*exit_status_ptr = 0;
		else
			*exit_status_ptr = 2;
	}
	return (-1);
}

/**
 * @brief The main Read-Eval-Print-Loop (REPL) of the shell.
 * @return The final exit status of the shell.
 */
int	ft_read_line(char *prompt, char ***envp)
{
	char	*line;
	int		exit_status;
	int		final_code;

	exit_status = 0;
	while (1)
	{
		line = readline(prompt);
		if (!line)
		{
			printf("exit\n");
			return (exit_status);
		}
		final_code = process_input_line(line, envp, &exit_status);
		free(line);
		if (final_code != -1)
		{
			return (final_code);
		}
	}
}

int	ft_start_minishell(char *str, char ***envp)
{
	int	exit_code;

	exit_code = ft_read_line(str, envp);
	return (exit_code);
}
