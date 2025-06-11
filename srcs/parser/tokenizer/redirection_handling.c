/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   redirection_handling.c                              :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/11 11:40:28 by jelee          #+#    #+#                */
/*   Updated: 2025/06/11 11:40:30 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"

void	add_redirection_to_list(t_redir **list, int type,
			const char *filename_or_delimiter, t_token *heredoc_node)
{
	t_redir	*new_redir;

	new_redir = (t_redir *)malloc(sizeof(t_redir));
	if (!new_redir)
	{
		perror("minishell: malloc in add_redirection_to_list");
		return ;
	}
	new_redir->type = type;
	new_redir->filename = NULL;
	if (filename_or_delimiter)
	{
		new_redir->filename = ft_strdup(filename_or_delimiter);
		if (!new_redir->filename && filename_or_delimiter[0] != '\0')
		{
			perror("minishell: ft_strdup in add_redirection_to_list");
			free(new_redir);
			return ;
		}
	}
	new_redir->heredoc_node = heredoc_node;
	new_redir->next = *list;
	*list = new_redir;
}

int	apply_redirections(t_redir *list)
{
	t_redir	*current;
	int		status;

	current = list;
	while (current != NULL)
	{
		status = 0;
		if (current->type == REDIR_OPEN)
			status = apply_redir_open(current);
		else if (current->type == REDIR_WRITE)
			status = apply_redir_write(current);
		else if (current->type == REDIR_WRITE_A)
			status = apply_redir_append(current);
		else if (current->type == HEREDOC)
			status = apply_redir_heredoc(current);
		else
		{
			fprintf(stderr, "unknown redirection type %d\n", current->type);
			status = -1;
		}
		if (status == -1)
			return (-1);
		current = current->next;
	}
	return (0);
}

void	free_redir_list(t_redir *list)
{
	t_redir	*current;
	t_redir	*next;

	current = list;
	while (current)
	{
		next = current->next;
		if (current->filename)
			free(current->filename);
		free(current);
		current = next;
	}
}
