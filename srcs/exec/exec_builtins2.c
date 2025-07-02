/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   exec_builtins2.c                                    :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/27 17:18:17 by jelee          #+#    #+#                */
/*   Updated: 2025/06/27 17:18:24 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

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
	ft_fprintf("minishell: ", argv[0], ": builtin not recognized\n");
	return (127);
}

/**
 * @brief Handles the logic for the 'exit' builtin command, determining the
 * final exit code or handling errors.
 * @return A status code. >= 256 signals the shell to exit, < 256 is an error.
 */
static int	handle_exit_command(char **argv, int exit_status)
{
	int	final_exit_code;

	final_exit_code = exit_status;
	if (argv[1])
	{
		if (!is_numeric(argv[1]))
		{
			ft_fprintf("minishell: exit: ",
				argv[1], ": numeric argument required\n");
			final_exit_code = 2;
		}
		else if (argv[2])
		{
			ft_fprintf("minishell: exit: too many arguments\n", "", "");
			return (1);
		}
		else
			final_exit_code = ft_atoi(argv[1]);
	}
	return (256 + (final_exit_code & 0xFF));
}

/**
 * @brief Builds arguments and executes a builtin command, dispatching to the
 * correct implementation.
 * @return The exit status of the executed builtin.
 */
int	ft_execute_builtin(t_token *node, char ***envp_ptr, int exit_status)
{
	char	**argv;
	int		status;

	argv = build_argv_from_ast(node, envp_ptr, exit_status);
	if (!argv || !argv[0])
	{
		free_argv(argv);
		return (1);
	}
	if (ft_strncmp(argv[0], "exit", 5) == 0)
	{
		status = handle_exit_command(argv, exit_status);
	}
	else
	{
		status = dispatch_builtin(argv, envp_ptr);
	}
	free_argv(argv);
	return (status);
}
