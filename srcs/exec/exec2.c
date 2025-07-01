/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   exec2.c                                             :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/16 17:48:17 by jelee          #+#    #+#                */
/*   Updated: 2025/06/16 17:48:19 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

int	preprocess_heredocs(t_token *node)
{
	if (!node)
		return (0);
	if (preprocess_heredocs(node->left) != 0)
		return (-1);
	if (preprocess_heredocs(node->right) != 0)
		return (-1);
	if (node->token == HEREDOC)
	{
		if (!node->right || !node->right->string)
		{
			// fprintf(stderr,
			// 	"minishell: syntax error: missing delimiter for heredoc\n");
			ft_fprintf("minishell: syntax error: missing delimiter for heredoc\n", "", "");
			node->heredoc_state = HD_PROCESSING_FAILED;
			return (-1);
		}
		node->heredoc_pipe_fd = read_heredoc_to_pipe(node->right->string);
		if (node->heredoc_pipe_fd < 0)
		{
			node->heredoc_state = HD_PROCESSING_FAILED;
			return (-1);
		}
		node->heredoc_state = HD_PROCESSED_OK;
	}
	return (0);
}

void	close_all_heredoc_fds_in_tree(t_token *node)
{
	if (!node)
		return ;
	close_all_heredoc_fds_in_tree(node->left);
	close_all_heredoc_fds_in_tree(node->right);
	if (node->token == HEREDOC && node->heredoc_pipe_fd >= 0)
	{
		close(node->heredoc_pipe_fd);
		node->heredoc_pipe_fd = -1;
	}
}

/**
 * @brief Frees a null-terminated array of strings and the array itself.
 * @param argv The array to free.
 */
void	free_argv(char **argv)
{
	int	i;

	if (!argv)
		return ;
	i = 0;
	while (argv[i])
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}

char	**free_and_nullify_argv(char **argv, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		free(argv[i]);
		i++;
	}
	free(argv);
	return (NULL);
}
