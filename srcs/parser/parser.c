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
			if (exit_status >= 256)
			{
				int final_code = exit_status - 256;
				free_ast(ast_root);
				free(line);
				// main으로 최종 종료 코드를 반환하여 정상 종료
				return (final_code);
			}
			free_ast(ast_root); // 각 명령어 실행 후 AST 메모리 해제
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
	int	exit_code;

	exit_code = ft_read_line(str, envp);
	return (exit_code);
}
