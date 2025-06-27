/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   execute_ast3.c                                      :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/27 17:59:49 by jelee          #+#    #+#                */
/*   Updated: 2025/06/27 17:59:57 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

static void	execute_simple_command(t_token *cmd_node, char ***envp, int l_exit)
{
	char	**argv;
	char	*full_path;

	argv = build_argv_from_ast(cmd_node, envp, l_exit);
	if (!argv)
		exit(0);
	check_for_directory(argv);
	full_path = get_full_path(argv[0], *envp);
	if (!full_path)
	{
		fprintf(stderr, "minishell: %s: command not found\n", argv[0]);
		free_argv(argv);
		exit(127);
	}
	execve(full_path, argv, *envp);
	handle_execve_error(argv, full_path);
}

/**
 * @brief Handles unrecognized token types that cannot be executed.
 */
static void	handle_unknown_command(t_token *node, char ***envp)
{
	if (is_expendable_variable(node->string, *envp) == 2
		|| !ft_strncmp(node->string, "$?",
			ft_strlen_longest(node->string, "$?")))
	{
		write(STDERR_FILENO, " command not found\n", 20);
		exit(127);
	}
	if (ft_is_variable(node->string)
		&& !is_expendable_variable(node->string, *envp))
		exit(0);
	if (ft_is_variable(node->string)
		&& is_expendable_variable(node->string, *envp) == 1)
	{
		write(STDERR_FILENO, " Is a directory\n", 16);
		exit(126);
	}
	fprintf(stderr, "minishell: %s: command not found (token type %d)\n",
		node->string, node->token);
	exit(127);
}

/**
 * @brief Dispatches a command node to the correct execution function.
 */
void	dispatch_command_execution(t_token *cmd_node, char ***envp, int l_exit)
{
	if (!cmd_node)
		exit(0);
	if (cmd_node->token == PIPE)
		exit(execute_pipe(cmd_node, envp));
	else if (cmd_node->token == BUILTIN)
		exit(ft_execute_builtin(cmd_node, envp, l_exit));
	else if (cmd_node->token == CMD || cmd_node->token == WORD
		|| cmd_node->token == VAR)
		execute_simple_command(cmd_node, envp, l_exit);
	else
		handle_unknown_command(cmd_node, envp);
}

static pid_t	fork_and_execute_child(t_token *node, char ***envp,
	int exit_status, struct sigaction *sa_int_old)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("minishell: fork failed");
		sigaction(SIGINT, sa_int_old, NULL);
		return (-1);
	}
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execute_child_command(node, envp, exit_status);
		exit(1);
	}
	return (pid);
}

/**
 * @brief Manages signals and executes a top-level command in a child process.
 * @return The final exit status of the command.
 */
int	execute_toplevel_command(t_token *node, char ***envp, int exit_status)
{
	pid_t				pid;
	struct sigaction	sa_int_old;
	struct sigaction	sa_quit_old;

	if (preprocess_heredocs(node) == -1)
		return (1);
	sigaction(SIGINT, NULL, &sa_int_old);
	sigaction(SIGQUIT, NULL, &sa_quit_old);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	pid = fork_and_execute_child(node, envp, exit_status, &sa_int_old);
	sigaction(SIGINT, &sa_int_old, NULL);
	sigaction(SIGQUIT, &sa_quit_old, NULL);
	if (pid == -1)
		return (1);
	exit_status = wait_and_get_status(pid);
	close_all_heredoc_fds_in_tree(node);
	return (exit_status);
}
