/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   exec2.c                                             :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/16 17:36:56 by jelee          #+#    #+#                */
/*   Updated: 2025/06/16 17:36:58 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

static int is_numeric(const char *str)
{
    if (!str || *str == '\0')
        return (0);
    if (*str == '+' || *str == '-')
        str++;
    while (*str)
    {
        if (!ft_isdigit((unsigned char)*str))
            return (0);
        str++;
    }
    return (1);
}

/**
 * @brief Counts the number of tokens that could become arguments.
 */
static int	count_potential_args(t_token *node)
{
	int		count;
	t_token	*curr;

	count = 0;
	curr = node;
	while (curr && (curr->token == CMD || curr->token == BUILTIN \
		|| curr->token == WORD || curr->token == VAR))
	{
		count++;
		curr = curr->right;
	}
	return (count);
}

/**
 * @brief Runs the loop to populate argv, processing each token.
 * @param argv The pre-allocated array to fill.
 * @param max_count The maximum number of tokens to process.
 * @param start_node The first token in the command chain.
 * @param envp_ptr A pointer to the environment variables.
 * @return Returns the populated argv array, or NULL on error.
 */
static char	**run_population_loop(char **argv, int max_count, \
				t_token *start_node, char ***envp_ptr, int l_exit)
{
	int		actual_i;
	t_token	*curr;
	int		i;

	actual_i = 0;
	curr = start_node;
	i = -1;
	while (++i < max_count)
	{
		if (!process_single_token(curr, argv, &actual_i, envp_ptr, l_exit))
			return (free_and_nullify_argv(argv, actual_i));
		curr = curr->right;
	}
	argv[actual_i] = NULL;
	if (actual_i == 0)
		return (free_and_nullify_argv(argv, 0));
	return (argv);
}

/**
 * @brief Sets up and builds an argv array from a token list.
 * This function handles the initial counting and allocation.
 */
char	**build_argv_from_ast(t_token *cmd_node, char ***envp_ptr, int l_exit)
{
	int		max_count;
	char	**argv;

	if (!cmd_node)
		return (NULL);
	max_count = count_potential_args(cmd_node);
	if (max_count == 0)
		return (NULL);
	argv = (char **)malloc(sizeof(char *) * (max_count + 1));
	if (!argv)
		return (perror("minishell: malloc failed"), NULL);
	return (run_population_loop(argv, max_count, cmd_node, envp_ptr, l_exit));
}

/**
 * @brief Dispatches the command to the correct builtin implementation.
 * @param argv The argument array for the command.
 * @param envp_ptr A pointer to the environment variables.
 * @return The exit status of the executed builtin command.
 */
static int	dispatch_builtin(char **argv, char ***envp_ptr)
{
	if (ft_strncmp(argv[0], "echo", 5) == 0)
		return (ft_echo(argv));
	else if (ft_strncmp(argv[0], "cd", 3) == 0)
		return (ft_cd(argv, envp_ptr));
	else if (ft_strncmp(argv[0], "pwd", 4) == 0)
		return (ft_pwd(argv));
	else if (ft_strncmp(argv[0], "export", 7) == 0)
		return (ft_export(envp_ptr, argv));
	else if (ft_strncmp(argv[0], "unset", 6) == 0)
		return (ft_unset(argv, envp_ptr));
	else if (ft_strncmp(argv[0], "env", 4) == 0)
		return (ft_env(argv, envp_ptr));
	fprintf(stderr, "minishell: %s: builtin not recognized\n", argv[0]);
	return (127);
}

int	ft_execute_builtin(t_token *node, char ***envp_ptr, int exit_status)
{
	char	**argv;
	int		current_cmd_status; // 현재 명령어의 상태를 저장할 변수

	// build_argv_from_ast도 $? 확장을 위해 last_exit_status가 필요할 수 있습니다.
	argv = build_argv_from_ast(node, envp_ptr, exit_status);
	if (!argv || !argv[0])
	{
		free_argv(argv);
		return (1);
	}

		if (ft_strncmp(argv[0], "exit", 5) == 0)
	{
		int final_exit_code = exit_status;
		if (argv[1])
		{
			if (!is_numeric(argv[1]))
			{
				fprintf(stderr, "minishell: exit: %s: numeric argument required\n", argv[1]);
				final_exit_code = 2;
			}
			else if (argv[2])
			{
				fprintf(stderr, "minishell: exit: too many arguments\n");
				free_argv(argv);
				return (1); // 셸을 종료하지 않고 상태 코드 1만 반환
			}
			else
				final_exit_code = ft_atoi(argv[1]);
		}
		free_argv(argv);
		// 주 반복문에 종료 신호를 보냅니다. 256을 더해 일반 종료 코드와 구별합니다.
		return (256 + (final_exit_code & 0xFF));
	}

	current_cmd_status = dispatch_builtin(argv, envp_ptr);
	free_argv(argv);
	return (current_cmd_status);
}
