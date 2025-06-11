/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   redirection_helper.c                                :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/11 11:58:25 by jelee          #+#    #+#                */
/*   Updated: 2025/06/11 11:58:28 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"

int	apply_redir_open(t_redir *redir_item)
{
	int	fd_opened;
	int	target_fd;
	int	dup2_result;

	target_fd = STDIN_FILENO;
	if (!redir_item || !redir_item->filename)
		return (-1);
	fd_opened = open(redir_item->filename, O_RDONLY);
	if (fd_opened == -1)
	{
		perror(redir_item->filename);
		return (-1);
	}
	dup2_result = dup2(fd_opened, target_fd);
	if (dup2_result == -1)
	{
		perror("dup2 STDIN_FILENO");
		close(fd_opened);
		return (-1);
	}
	if (close(fd_opened) == -1)
		perror(redir_item->filename);
	return (0);
}

int	apply_redir_write(t_redir *redir_item)
{
	int		fd_opened;
	int		target_fd;
	mode_t	mode;
	int		dup2_result;

	target_fd = STDOUT_FILENO;
	mode = 0644;
	if (!redir_item || !redir_item->filename)
		return (-1);
	fd_opened = open(redir_item->filename, O_WRONLY | O_CREAT | O_TRUNC, mode);
	if (fd_opened == -1)
	{
		perror(redir_item->filename);
		return (-1);
	}
	dup2_result = dup2(fd_opened, target_fd);
	if (dup2_result == -1)
	{
		perror("dup2 STDOUT_FILENO");
		close(fd_opened);
		return (-1);
	}
	if (close(fd_opened) == -1)
		perror(redir_item->filename);
	return (0);
}

int	apply_redir_append(t_redir *redir_item)
{
	int		fd;
	int		target_fd;
	mode_t	mode;

	target_fd = STDOUT_FILENO;
	mode = 0644;
	if (!redir_item || !redir_item->filename)
		return (-1);
	fd = open(redir_item->filename, O_WRONLY | O_CREAT | O_APPEND, mode);
	if (fd == -1)
	{
		perror(redir_item->filename);
		return (-1);
	}
	if (dup2(fd, target_fd) == -1)
	{
		perror("dup2 STDOUT_FILENO (append)");
		close(fd);
		return (-1);
	}
	if (close(fd) == -1)
		perror(redir_item->filename);
	return (0);
}

int	apply_redir_heredoc(t_redir *redir_item)
{
	int	heredoc_fd_to_dup;
	int	target_fd;

	target_fd = STDIN_FILENO;
	if (!redir_item
		|| !redir_item->heredoc_node
		|| redir_item->heredoc_node->heredoc_state != HD_PROCESSED_OK
		|| redir_item->heredoc_node->heredoc_pipe_fd < 0)
	{
		fprintf(stderr, "internal error or heredoc not ready\n");
		return (-1);
	}
	heredoc_fd_to_dup = redir_item->heredoc_node->heredoc_pipe_fd;
	if (dup2(heredoc_fd_to_dup, target_fd) == -1)
	{
		perror("dup2 STDIN_FILENO (heredoc)");
		return (-1);
	}
	return (0);
}
