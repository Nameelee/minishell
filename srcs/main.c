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

void	ft_fprintf(const char *prefix, const char *arg, const char *suffix)
{
	char	*temp_str;
	char	*final_str;

	temp_str = NULL;
	final_str = NULL;
	temp_str = ft_strjoin(prefix, arg);
	if (temp_str == NULL)
	{
		ft_putendl_fd("Memory alloc failed error printing.", STDERR_FILENO);
		return ;
	}
	final_str = ft_strjoin(temp_str, suffix);
	free(temp_str);
	if (final_str == NULL)
	{
		ft_putendl_fd("Memory alloc failed error printing.", STDERR_FILENO);
		return ;
	}
	ft_putstr_fd(final_str, STDERR_FILENO);
	free(final_str);
}

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
	int		final_exit_status;

	(void)argc;
	(void)argv;
	if (ft_signal() == -1)
		return (1);
	dupplicate_env = ft_duplicate_env(envp);
	if (!dupplicate_env)
	{
		return (EXIT_FAILURE);
	}
	final_exit_status = ft_start_minishell("minishell: ", &dupplicate_env);
	free_duplicated_env(dupplicate_env);
	return (final_exit_status);
}
