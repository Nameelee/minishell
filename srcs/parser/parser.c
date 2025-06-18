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
			fprintf(stderr, "minishell: syntax error\n");
		return (NULL);
	}
	return (ast_root);
}

int	ft_read_line(char *prompt, char **envp)
{
	char	*line;
	t_token	*ast_root;
	int		exit_status; // 지역 변수 선언

	exit_status = 0; // 초기화
	while (1)
	{
		line = readline(prompt);
		if (!line)
		{
			printf("exit\n"); // Ctrl+D 처리
			return (exit_status);
		}
		ast_root = process_line(line);
		if (ast_root)
		{
			exit_status = execute_ast(ast_root, &envp, true, exit_status);
		}
		else
		{
			if (line_is_empty_or_whitespace(line))
				exit_status = 0;
			else
				exit_status = 2;
		}
		free(line);
	}
	return (exit_status);
}

int	ft_start_minishell(char *str, char **envp)
{
	int	read;

	read = ft_read_line(str, envp);
	if (read == 1)
		return (1);
	return (0);
}
