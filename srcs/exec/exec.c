/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   exec.c                                              :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/12 14:09:23 by jelee          #+#    #+#                */
/*   Updated: 2025/06/12 14:09:26 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @brief Reads user input for a heredoc and writes it to the provided pipe.
 * @param write_fd  The file descriptor for the write-end of the pipe.
 * @param delimiter The string that signals the end of input.
 */
static void	heredoc_input_loop(int write_fd, const char *delimiter)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			fprintf(stderr,
				"minishell:here-doc delimited by end-of-file (wanted `%s')\n",
				delimiter);
			break ;
		}
		if (strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		write(write_fd, line, strlen(line));
		write(write_fd, "\n", 1);
		free(line);
	}
}

/**
 * @brief Creates a pipe and fills it with heredoc input.
 * @param delimiter The heredoc delimiter string.
 * @return The file descriptor for the read-end of the pipe, or -1 on error.
 */
int	read_heredoc_to_pipe(const char *delimiter)
{
	int	pipefd[2];

	if (pipe(pipefd) == -1)
	{
		perror("minishell: pipe for heredoc failed");
		return (-1);
	}
	heredoc_input_loop(pipefd[1], delimiter);
	close(pipefd[1]);
	return (pipefd[0]);
}

bool	process_single_token(t_token *curr, char **argv,
			int *actual_i_ptr, t_exec_context *ctx)
{
	char			*expanded_str;
	t_quote_status	quotes;

	if (!curr || !curr->string)
	{
		fprintf(stderr, "minishell: unexpected NULL node or string.\n");
		return (false);
	}
	quotes.is_single = curr->single_quote;
	quotes.is_double = curr->double_quote;
	expanded_str = expand_all_variables(curr->string, quotes, ctx);
	if (!expanded_str)
		return (perror("minishell: argument expansion failed"), false);
	if (expanded_str[0] == '\0' && curr->double_quote == 0)
		free(expanded_str);
	else
	{
		argv[*actual_i_ptr] = expanded_str;
		(*actual_i_ptr)++;
	}
	return (true);
}

/**
 * @brief Sets up a pipe and coordinates the creation of child processes.
 */
int	execute_pipe(t_token *node, char ***envp)
{
	int		pipefd[2];
	pid_t	left_pid;
	pid_t	right_pid;

	if (pipe(pipefd) == -1)
		return (perror("minishell: pipe failed"), 1);
	left_pid = fork_left_child(node, envp, pipefd);
	if (left_pid == -1)
		return (1);
	right_pid = fork_right_child(node, envp, pipefd, left_pid);
	if (right_pid == -1)
		return (1);
	close(pipefd[0]);
	close(pipefd[1]);
	return (wait_for_children_and_get_status(left_pid, right_pid));
}
