/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export_display.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ast <ast@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:17:51 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/03 15:31:23 by ast              ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../../builtin.h"


void *ft_destroy_env(char **envp[], int idx);
/*
	add quote for export display variable
	exemple: 
		Z=1 >>> Z="1"
		Z 	>>> Z

*/
static char	*ft_add_quote(char *s)
{
	int		len_1;
	int		len_2;
	char	*new_str;
	int		i;
	char	*str;

	if (!s)
		return (NULL);
	str = ft_strdup(s);
	if (ft_index_of_char(str, '=') == -1)
		return (str);
	len_1 = ft_strlen(str);
	new_str = malloc(sizeof(char *) * (len_1 + 3));
	i = ft_index_of_char(str, '=');
	ft_strlcpy(new_str, str, i + 2);
	i += 1;
	new_str[i++] = '"';
	len_2 = ft_strlen(&str[ft_index_of_char(str, '=') + 1]) + 2;
	ft_strlcpy(&new_str[i], &str[ft_index_of_char(str, '=') + 1], len_2);
	i += ft_strlen(&str[ft_index_of_char(str, '=') + 1]);
	new_str[i++] = '"';
	new_str[i] = '\0';
	free(str);
	return (new_str);
}

/*
	add declare -x to the variable befor printing export env 
*/
char	*ft_add_declare_x(char *tmp)
{
	char	*concat_var;

	if (!tmp)
		return (NULL);
	if (ft_strncmp(tmp, "declare -x ", ft_strlen("declare -x ")) != 0)
	{
		concat_var = ft_strjoin("declare -x ", tmp);
		if (!concat_var)
			return (NULL);
		return (concat_var);
	}
	return (NULL);
}

/*
	Add declare -x and quote.
*/

char	*process_export_variable(char *var)
{
	char	*tmp;
	char	*concat_var;

	tmp = ft_add_quote(var);
	if (!tmp)
		return (NULL);
	concat_var = ft_add_declare_x(tmp);
	if (!concat_var)
		return (NULL);
	
	free(tmp);
	return (concat_var);
}

/*
	Dupplicate env for printing export action.

	Safe check memory Done!
	
*/
static char	**ft_duplicate_export_env(char *envp[])
{
	char	**dup_env;
	int		i;

	if (!envp)
		return (NULL);
	dup_env = malloc(sizeof(char *) * (ft_get_split_len(envp) + 1));
	if (!dup_env)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		dup_env[i] = envp[i];
		i++;
	}
	dup_env[i] = NULL;
	return (dup_env);
}

/*
	 specifique for export env: dupplicate env, add declare x, add quote, and sort .

*/
int	display_export_env(char ***env)
{
	char	**dup_env;
	int i;

	if (!env)
		return (1);
	dup_env = ft_duplicate_export_env(*env);

	if (!dup_env)
		return (1);
	env_quick_s(dup_env, ft_get_split_len(dup_env), ft_str_env_cmp);
	i = 0;
	while (dup_env[i])
	{
		printf("declare -x \"%s\"\n", dup_env[i]);
		i++;
	}
	
	ft_split_clean(&dup_env);
	return (0);
}
