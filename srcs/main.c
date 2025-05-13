/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ast <ast@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/05/05 00:06:59 by ast              ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

 #include "../include/main.h"
 
 void free_duplicated_env(char **dup_envp);

 int g_exit_status = 0;

 int main(int argc, char **argv, char **envp)
 {
	(void)argc;
	(void)argv;
	char **dupplicate_env;

	dupplicate_env = ft_duplicate_env(envp);
	if (!dupplicate_env) 
	{
        return (EXIT_FAILURE);
    }
 	ft_start_minishell("minishell: ", dupplicate_env);
    free_duplicated_env(dupplicate_env);
 	return(g_exit_status);
 }
 


