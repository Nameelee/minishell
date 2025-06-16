/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   exec_pipe.c                                         :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/16 16:17:05 by jelee          #+#    #+#                */
/*   Updated: 2025/06/16 16:17:21 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
/**
 * @brief Handles the execution for the left child of a pipe.
 * This function runs in a child process and does not return.
 */
void	execute_pipe_left_child(t_token *node, char ***envp, int pipefd[2])
{
	close(pipefd[0]);
	if (dup2(pipefd[1], STDOUT_FILENO) == -1)
	{
		perror("minishell: dup2 L_STDOUT");
		close(pipefd[1]);
		exit(1);
	}
	close(pipefd[1]);
	execute_ast(node->left, envp, false);
	exit(g_exit_status);
}

/**
 * @brief Handles the execution for the right child of a pipe.
 * This function runs in a child process and does not return.
 */
void	execute_pipe_right_child(t_token *node, char ***envp, int pipefd[2])
{
	close(pipefd[1]);
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
	{
		perror("minishell: dup2 R_STDIN");
		close(pipefd[0]);
		exit(1);
	}
	close(pipefd[0]);
	execute_ast(node->right, envp, false);
	exit(g_exit_status);
}

/**
 * @brief Forks the left child process for the pipe.
 * @return The PID of the child, or -1 on failure.
 */
pid_t	fork_left_child(t_token *node, char ***envp, int pipefd[2])
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("minishell: fork L failed");
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid == 0)
		execute_pipe_left_child(node, envp, pipefd);
	return (pid);
}

/**
 * @brief Forks the right child process for the pipe.
 * @return The PID of the child, or -1 on failure.
 */
pid_t	fork_right_child(t_token *node, char ***envp, int pipefd[2],
			pid_t left_pid)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("minishell: fork R failed");
		close(pipefd[0]);
		close(pipefd[1]);
		waitpid(left_pid, NULL, 0);
		return (-1);
	}
	if (pid == 0)
		execute_pipe_right_child(node, envp, pipefd);
	return (pid);
}

int	wait_for_children_and_get_status(pid_t left_pid, pid_t right_pid)
{
	int	left_status;
	int	right_status;

	waitpid(left_pid, &left_status, 0);
	waitpid(right_pid, &right_status, 0);
	if (WIFEXITED(right_status))
		return (WEXITSTATUS(right_status));
	if (WIFSIGNALED(right_status))
		return (128 + WTERMSIG(right_status));
	return (1);
}
