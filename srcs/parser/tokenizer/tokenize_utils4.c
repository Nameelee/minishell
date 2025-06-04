#include "tokenize.h"

int ft_is_commande(char *str)
{
    char *env;
    char **split;
    char *total_path;
    int acces;
    int i;
    char *r;
    
    env = getenv("PATH");
    split = ft_split(env, ':');
    if(!env || !split)
        return(-1);
    i = 0;
    while (split[i])
    {
        r = ft_strtrim(str," ");
        total_path = ft_get_total_path(split[i], r);
        acces = access(total_path,X_OK);
        free(total_path);
        if(acces == 0)
        {
            ft_split_clean(&split);
            return(1);
        }
        i++;
    }
    ft_split_clean(&split);
    return(0);
}

int ft_is_builtin(char *str)
{
	if (ft_strlen(str) == ft_strlen("cd") && ft_strncmp(str, "cd", ft_strlen("cd")) == 0)
        return (1);
    if (ft_strlen(str) == ft_strlen("echo") && ft_strncmp(str, "echo", ft_strlen("echo")) == 0)
        return (1);
    if (ft_strlen(str) == ft_strlen("export") && ft_strncmp(str, "export", ft_strlen("export")) == 0)
        return (1);
    if (ft_strlen(str) == ft_strlen("env") && ft_strncmp(str, "env", ft_strlen("env")) == 0)
        return (1);
    if (ft_strlen(str) == ft_strlen("exit") && ft_strncmp(str, "exit", ft_strlen("exit")) == 0)
        return (1);
    if (ft_strlen(str) == ft_strlen("pwd") && ft_strncmp(str, "pwd", ft_strlen("pwd")) == 0)
        return (1);
    if (ft_strlen(str) == ft_strlen("unset") && ft_strncmp(str, "unset", ft_strlen("unset")) == 0)
        return (1);
        
    return (0); // Not an exact match for any builtin
}
int ft_get_token(char *str)
{
    if(ft_is_builtin(str))
        return(BUILTIN);
    //&& nnot necessaire but...
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

void    ft_token_lst_iter(t_token *token, void (*f)(t_token *token_node, int token))
{
    if(!token || !f)
        return ;
    while(token)
    {
        f(token, token->token);
        token = token->right;        
    }
}

int ft_count_occurence_of_token(t_token *token_lst, int token)
{
    int i;

    i = 0;
    while (token_lst)
    {
        if (token_lst->token == token)
            i++;
        token_lst = token_lst->right;
    }
    return (i);
    
}