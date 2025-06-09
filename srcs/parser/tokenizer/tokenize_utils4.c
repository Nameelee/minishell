/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   tokenize_utils4.c                                   :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/09 16:56:05 by jelee          #+#    #+#                */
/*   Updated: 2025/06/09 16:56:08 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"

static int	ft_setup_path_check(char *str, char ***paths_ptr, char **cmd_ptr)
{
	*cmd_ptr = ft_strtrim(str, " ");
	*paths_ptr = ft_split(getenv("PATH"), ':');
	if (!*cmd_ptr || !*paths_ptr)
	{
		if (*cmd_ptr)
			free(*cmd_ptr);
		if (*paths_ptr)
			ft_split_clean(paths_ptr);
		return (-1);
	}
	return (0);
}

int	ft_is_commande(char *str)
{
	char	**paths;
	char	*cmd;
	char	*full_path;
	int		result;
	int		i;

	if (ft_setup_path_check(str, &paths, &cmd) == -1)
		return (-1);
	result = 0;
	i = 0;
	while (paths[i] && result == 0)
	{
		full_path = ft_get_total_path(paths[i], cmd);
		if (access(full_path, X_OK) == 0)
			result = 1;
		free(full_path);
		i++;
	}
	free(cmd);
	ft_split_clean(&paths);
	return (result);
}

int	ft_is_builtin(char *str)
{
	if (ft_strlen(str) == ft_strlen("cd")
		&& ft_strncmp(str, "cd", ft_strlen("cd")) == 0)
		return (1);
	if (ft_strlen(str) == ft_strlen("echo")
		&& ft_strncmp(str, "echo", ft_strlen("echo")) == 0)
		return (1);
	if (ft_strlen(str) == ft_strlen("export")
		&& ft_strncmp(str, "export", ft_strlen("export")) == 0)
		return (1);
	if (ft_strlen(str) == ft_strlen("env")
		&& ft_strncmp(str, "env", ft_strlen("env")) == 0)
		return (1);
	if (ft_strlen(str) == ft_strlen("exit")
		&& ft_strncmp(str, "exit", ft_strlen("exit")) == 0)
		return (1);
	if (ft_strlen(str) == ft_strlen("pwd")
		&& ft_strncmp(str, "pwd", ft_strlen("pwd")) == 0)
		return (1);
	if (ft_strlen(str) == ft_strlen("unset")
		&& ft_strncmp(str, "unset", ft_strlen("unset")) == 0)
		return (1);
	return (0);
}

int	ft_get_token(char *str)
{
	if (ft_is_builtin(str))
		return (BUILTIN);
	if (ft_is_commande(str) && !ft_is_builtin(str))
		return (CMD);
	if (ft_strlen(str) == 1 && str[0] == '|')
		return (PIPE);
	if (ft_strlen(str) == 1 && str[0] == '<')
		return (REDIR_OPEN);
	if (ft_strlen(str) == 1 && str[0] == '>')
		return (REDIR_WRITE);
	if (ft_strlen(str) == 2 && str[0] == '>' && str[1] == '>')
		return (REDIR_WRITE_A);
	if (ft_strlen(str) == 2 && str[0] == '<' && str[1] == '<')
		return (HEREDOC);
	if (ft_strlen(str) == 2 && str[0] == '&' && str[1] == '>')
		return (DOUBLE_REDIR);
	if (str[0] == '$')
		return (VAR);
	return (WORD);
}

void	ft_token_lst_iter(t_token *token,
	void (*f)(t_token *token_node, int token))
{
	if (!token || !f)
		return ;
	while (token)
	{
		f(token, token->token);
		token = token->right;
	}
}
