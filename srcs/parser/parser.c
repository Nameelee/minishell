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
        {
            // --- SET EXIT STATUS FOR PARSE ERROR ---
			// Make sure NO output goes to stdout here. Errors to stderr if desired.
			// fprintf(stderr, "minishell: syntax error\n"); // Example error to stderr
			//g_exit_status = 258; // Common exit code for syntax errors in bash
			// --- END SET EXIT STATUS ---
            free(line);
            continue;
        }
        
        //ft_binary_tree_traversal(ast_root);
		// print_ast_start(ast_root);
        //ft_binary_tree_traversal(ast_root);

		execute_ast(ast_root, &envp, true); // Execute AST!
		//fprintf(stderr, "DEBUG_FT_READ_LINE: After execute_ast, g_exit_status = %d\n", g_exit_status); // ★★★ 추가 확인
        ft_export(&envp, ft_split(ft_strjoin("export ?=", ft_itoa(return_value)), 32));
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