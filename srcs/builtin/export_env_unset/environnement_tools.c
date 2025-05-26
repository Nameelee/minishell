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