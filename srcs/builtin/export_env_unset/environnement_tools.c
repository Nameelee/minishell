/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environnement_tools.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 13:27:38 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/16 12:45:50 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin.h"

void	clean_swap(char **new, char **old)
{
	char	*tmp;

	tmp = *old;
	*old = *new;
	free(tmp);
}

int	ft_get_var_idx_in_env(char **envp, char *var) // 'var' is the variable name to find, e.g., "PWD"
{
	char	*env_var_name;      // Stores the name part of the current environment variable
	char	*tmp_env_entry_dup; // Stores the duplicated current environment string
	int		i;
	size_t	var_len;
	size_t	env_var_name_len;

	i = 0;
	if (!envp || !var) // Basic null check for robustness
		return (-1);

	var_len = ft_strlen(var); // Length of the name we are looking for (e.g., strlen("PWD"))

	while (envp[i])
	{
		// Duplicate the current environment entry to safely parse it
		tmp_env_entry_dup = ft_strdup(envp[i]); // e.g., strdup("PWD=/home/user")
		if (!tmp_env_entry_dup)
		{
			// Malloc failure; ideally, log an error or handle more gracefully.
			// Skipping this entry for now to prevent a crash.
			i++;
			continue;
		}

		// ft_parse_env_variable is expected to return a new string with just the NAME
		env_var_name = ft_parse_env_variable(tmp_env_entry_dup); // Extracts "NAME"
		
		if (env_var_name) // Check if parsing was successful
		{
			env_var_name_len = ft_strlen(env_var_name);
			// Compare lengths first, then content if lengths are equal for an exact match
			if (env_var_name_len == var_len && ft_strncmp(env_var_name, var, var_len) == 0)
			{
				free(tmp_env_entry_dup);
				free(env_var_name);
				return (i); // Variable found, return its index
			}
			free(env_var_name); // Free the parsed name if it's not a match
		}
		free(tmp_env_entry_dup); // Free the duplicated entry
		i++;
	}
	return (-1); // Variable not found
}


/*
int	ft_get_var_idx_in_env(char **envp, char *var)
{
	char	*env_var;
	char	*tmp;
	int		i;

	i = 0;
	while (envp[i])
	{
		tmp = ft_strdup(envp[i]);
		env_var = ft_parse_env_variable(tmp);
		if (!ft_strncmp(env_var, var, ft_strlen_longest(env_var, var)))
		{
			free(tmp);
			free(env_var);
			return (i);
		}
		free(tmp);
		free(env_var);
		i++;
	}
	return (-1);
}
*/

char	*ft_get_env_variable(char **envp, char *var)
{
	int	idx;

	if (!ft_is_variable(var))
		return (NULL);
	idx = ft_get_var_idx_in_env(envp, &var[1]);
	if (idx > -1)
		return (envp[idx]);
	return (NULL);
}

void	ft_print_env(char **env) // (이 함수가 있는 파일에서 수정)
{
	char	**dup_env;
	int		count = 0;

	//fprintf(stderr, "[DEBUG FT_ENV %d] ft_print_env started. Will print to STDOUT_FILENO (%d).\n", getpid(), STDOUT_FILENO);

	if (!env)
	{
	//	fprintf(stderr, "[DEBUG FT_ENV %d] Environment is NULL.\n", getpid());
		return ;
	}
	dup_env = ft_duplicate_env(env); // 이 부분은 디버깅에 필수적이진 않지만, 기존 로직 유지
	// ft_split_print(dup_env); // 기존 출력 함수
    
    // ft_split_print 대신 직접 루프 돌며 디버깅 메시지 추가 (선택 사항)
    if (dup_env) {
        for (int i = 0; dup_env[i]; i++) {
            // fprintf(stderr, "[DEBUG FT_ENV %d] Writing to STDOUT: %s\n", getpid(), dup_env[i]); // 너무 많은 출력을 유발할 수 있음
            write(STDOUT_FILENO, dup_env[i], ft_strlen(dup_env[i]));
            write(STDOUT_FILENO, "\n", 1);
            count++;
        }
    }
	ft_split_clean(&dup_env);
	//fprintf(stderr, "[DEBUG FT_ENV %d] ft_print_env finished. Printed %d lines.\n", getpid(), count);
}