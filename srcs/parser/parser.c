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
		if (line_is_empty_or_whitespace(line))
			g_exit_status = 0;
		else
		{
			fprintf(stderr, "minishell: syntax error\n");
			g_exit_status = 2;
		}
		return (NULL);
	}
	return (ast_root);
}

int	ft_read_line(char *prompt, char **envp)
{
	char	*line;
	t_token	*ast_root;
	int		return_value;

	return_value = 0;
	while (1)
	{
		line = readline(prompt);
		if (!line)
			return (1);
		assert(line);
		ast_root = process_line(line);
		if (ast_root)
		{
			execute_ast(ast_root, &envp, true);
		}
		free(line);
	}
	return (return_value);
}

int	ft_start_minishell(char *str, char **envp)
{
	int	read;

	read = ft_read_line(str, envp);
	if (read == 1)
		return (1);
	return (0);
}

/* 
This works well. I commented this codes only for refactoring 
int ft_read_line(char *prompt, char **envp)
{
    char *line;
    int return_value;
    t_token *ast_root;  // Now ft_tokenize returns a single AST root

    return_value = 0;
    while (1)
    {
        line = readline(prompt);
        if (!line)
            return 1;

        assert(line);
		if (line[0] != '\0') // Only add non-empty lines to history
        {
            add_history(line); // Permitted lib
        }
        //ast_root = ft_tokenize(line);  // Returns AST root now
		ast_root= ft_parse(line);
		if (!ast_root) 
		{ 
			if (line_is_empty_or_whitespace(line)) 
			{
				g_exit_status = 0;
				free(line);
				continue;
			}

			else
			{
				fprintf(stderr, "minishell: syntax error\n"); // 또는 더 구체적인 오류 메시지
				g_exit_status = 2; // 구문 오류에 대한 일반적인 종료 상태
				free(line);
				continue;
			}
		}        
		//print_ast_start(ast_root);
		execute_ast(ast_root, &envp, true); // Execute AST!
        //ft_export(&envp, ft_split(ft_strjoin("export ?=", ft_itoa(return_value)), 32));
        free(line);  // Free input line after processing
    }
    return(return_value);
}
*/
