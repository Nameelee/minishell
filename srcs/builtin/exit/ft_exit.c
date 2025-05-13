#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "../builtin.h"

static int ft_isquote(char c)
{
    if(c == 39 || c == 34)
        return(1);
    return(0);
}

char *exit_skip_space_and_quote(char *str)
{
    int i;
    int j;
    int len;
    char *new_str;

    if(!str)
        return(NULL);
    i = 0;
    j = 0;
    while(str[i] == 32)
        i++;
    len = ft_strlen(&str[i]);
    if(str[i] == '"')
        i++;
    new_str = malloc(sizeof(char *) * (len + 1));
    if(!new_str)
        return(NULL);
    while (str[i] && i < len && str[i] != 32)
    {
        if((ft_isquote(str[i])) || (ft_isquote(str[i])))
            i++;
        else
            new_str[j++] = str[i++];
    }
    new_str[j] = '\0';
    return(new_str);
}

int	is_numeric(const char *str)
{
	if (!str || *str == '\0')
		return (0);
	if (*str == '+' || *str == '-')
		str++;
	while (*str)
	{
		if (!isdigit((unsigned char)*str))
			return (0);
		str++;
	}
	return (1);
}

char *ft_process_split(char *str)
{
	char new[7];
	int i;
	int j;

	i = 0;
	j = 0;
	while (str[i])
	{

		if(ft_isdigit(str[i]))
		{

			new[j++] = str[i];
		}
		i++;
	}
	new[j] = '\0';
	return(ft_strdup(new));
}

void	ft_exit(char **split, char **env)
{

	//write(STDERR_FILENO, "exit\n", 5); // Toujours afficher "exit"
	// if(!split)
	// 	exit();
	if(split)
	{
		if (split[1])
		{
			char *tmp =  exit_skip_space_and_quote(split[1]);
			if (!is_numeric(tmp))
			{
				write(STDERR_FILENO, " argument numérique nécessaire\n", ft_strlen(" argument numérique nécessaire\n"));
				exit(2);
			}
			if (split[2])
			{

				if(ft_strncmp(split[1], "+", 1) == 0 || ft_strncmp(split[1], "-", 1) == 0)
				{
					if(split[2])
					{
						
						char *tmp = ft_process_split(split[2]);
						if(tmp && split[1][0] == '+')
							exit(ft_atoi(tmp));
						else
							exit(156);
					}
				}
				write(STDERR_FILENO, " trop d'arguments\n", ft_strlen(" trop d'arguments\n"));
				exit(1);
			}
			exit(atoi(tmp));
		}
		char *var = ft_get_env_variable(env, "$?");
		int r = ft_atoi(ft_strchr(var, '=') + 1);
		exit(r);
	}
	exit(0);
}
