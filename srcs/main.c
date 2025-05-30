/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:09:22 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/16 13:38:38 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/main.h"

void	free_duplicated_env(char **dup_envp);

int	g_exit_status = 0;

void	sigint_handler(int signal)
{
	if (signal == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
		
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

int	ft_signal(void)
{
	struct sigaction	sa;

	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		return (1);
	signal(SIGQUIT, SIG_IGN);
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	char	**dupplicate_env;

	(void)argc;
	(void)argv;
	if (ft_signal() == -1)
		return (1);
	dupplicate_env = ft_duplicate_env(envp);
	if (!dupplicate_env)
	{
		return (EXIT_FAILURE);
	}
	ft_start_minishell("minishell: ", dupplicate_env);
	free_duplicated_env(dupplicate_env);
	printf("exit status is %d", g_exit_status);
	return (g_exit_status);
}
