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
	(void)node;
	return (0);
}

void	close_all_heredoc_fds_in_tree(t_token *node)
{
	(void)node;
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
