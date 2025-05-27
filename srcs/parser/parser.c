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


// line이 NULL, 비어있거나 공백만 있는지 확인하는 헬퍼 함수
bool line_is_empty_or_whitespace(const char *str) {
    if (!str) return true;
    while (*str) {
        if (*str != ' ' && *str != '\t' && *str != '\n' && *str != '\v' && *str != '\f' && *str != '\r') {
            return false;
        }
        str++;
    }
    return true;
}


/* Previous version of Cedric
int ft_read_line(char *str)
{
    char *line;
    t_token **token_lst;

    while (1)
    {
        line = readline(str);
        if(!line)
            return(1);
		assert(line);
        token_lst = ft_tokenize(line);
		ft_display_token_sequence_lst(*token_lst);
		
    }
    return(0);
}
*/

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
		add_history(line);//permitted lib
        //ast_root = ft_tokenize(line);  // Returns AST root now
		ast_root= ft_parse(line);
		if (!ast_root) 
		{ // 비어있지 않거나 공백만 있는 줄이 아닌 곳에서 발생한 실제 파싱/구문 오류
			fprintf(stderr, "minishell: syntax error\n"); // 또는 더 구체적인 오류 메시지
			g_exit_status = 2; // 구문 오류에 대한 일반적인 종료 상태
			free(line);
			continue;
		}

		if (line_is_empty_or_whitespace(line)) 
		{
			if (ast_root) {
				// ft_free_ast_recursive(ast_root); // 공백 파싱으로 AST가 생성되었다면 해제
			}
			free(line);
			if (!line && !*prompt) return 1; // 빈 프롬프트에서 EOF 발생 시 exit 시뮬레이션
			if (ft_strlen(line) == 0 && !ast_root) { // readline에서 반환된 진짜 빈 줄
			// g_exit_status 변경 없이 프롬프트만 다시 표시
			continue;
			}
		}
        
        //ft_binary_tree_traversal(ast_root);
		//print_ast_start(ast_root);
        //ft_binary_tree_traversal(ast_root);

		execute_ast(ast_root, &envp, true); // Execute AST!
		//fprintf(stderr, "DEBUG_FT_READ_LINE: After execute_ast, g_exit_status = %d\n", g_exit_status); // ★★★ 추가 확인
        //ft_export(&envp, ft_split(ft_strjoin("export ?=", ft_itoa(return_value)), 32));
        free(line);  // Free input line after processing
		//fprintf(stderr, "DEBUG_FT_READ_LINE: End of loop iteration, g_exit_status = %d\n", g_exit_status); // ★★★ 추가 확인
    }
    return(return_value);
}

int  ft_start_minishell(char *str, char **envp)
{
    int read;
    read = ft_read_line(str, envp);
    if(read == 1)
        return(1);
    return(0);
}