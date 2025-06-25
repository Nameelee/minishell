/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cw3l <cw3l@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 10:52:29 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/10 09:50:17 by cw3l             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../builtin.h"

static int	is_unset_valide_variable(char *var)
{
	int	i;

	i = 0;
	while (var[i])
	{
		if (!ft_isalnum(var[i]))
		{
			printf("unset: %s: invalid parameter name\n", var);
			return (0);
		}
		i++;
	}
	return (1);
}

int	ft_count_unset_valide_variable(char **var)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (var[i])
	{
		if (is_unset_valide_variable(var[i]))
			count++;
		i++;
	}
	return (count);
}

static int	is_valid_identifier(char *var)
{
	int	i;

	i = 0;
	// 변수 이름은 알파벳이나 밑줄(_)로 시작해야 합니다.
	if (!var || (!ft_isalpha(var[0]) && var[0] != '_'))
	{
		ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
		ft_putstr_fd(var, STDERR_FILENO);
		ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
		return (0);
	}
	// 그 뒤에는 알파벳, 숫자, 밑줄(_)만 올 수 있습니다.
	while (var[i])
	{
		if (!ft_isalnum(var[i]) && var[i] != '_')
		{
			ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
			ft_putstr_fd(var, STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			return (0);
		}
		i++;
	}
	return (1);
}

int	ft_delete_from_env(char *env, char **var)
{
	int	i;

	i = 0;
	if (!env)
		return (-1);
	while (var[i] != NULL)
	{
		if (env && var[i])
		{
			if (ft_strncmp(env, var[i], ft_strlen(var[i])) == 0)
				return (i);
		}
		i++;
	}
	return (-1);
}

char	**ft_delete_variable(char **old_env, char **del_var, int valide_var)
{
	size_t	i;
	size_t	j;
	size_t	env_len;
	char	**new_env;

	env_len = ft_get_split_len(old_env) - valide_var;
	new_env = malloc(sizeof(char *) * (env_len + 3));
	if (!new_env)
		return (NULL);
	i = 0;
	j = 0;
	while (i < ft_get_split_len(old_env))
	{
		if (ft_delete_from_env(old_env[i], del_var) != -1)
		{
			free(old_env[i]);
			i++;
			continue ;
		}
		else
			new_env[j++] = old_env[i++];
	}
	new_env[j] = NULL;
	return (new_env);
}

// 환경변수 배열에서 특정 변수의 인덱스를 찾는 함수
static int	find_var_index(char **envp, const char *var_to_find)
{
	int		i;
	char	*var_name;

	i = 0;
	while (envp[i])
	{
		var_name = ft_parse_env_variable(envp[i]);
		if (var_name && ft_strncmp(var_name, var_to_find, ft_strlen(var_to_find) + 1) == 0)
		{
			free(var_name);
			return (i); // 인덱스를 찾으면 반환
		}
		free(var_name);
		i++;
	}
	return (-1); // 찾지 못하면 -1 반환
}

// 특정 인덱스의 환경변수를 제거하는 함수
static void	remove_var_at_index(char ***envp, int index_to_remove)
{
	int	i;

	free((*envp)[index_to_remove]); // 해당 문자열 메모리 해제
	i = index_to_remove;
	while ((*envp)[i])
	{
		// 포인터들을 한 칸씩 앞으로 당겨서 덮어씁니다.
		(*envp)[i] = (*envp)[i + 1];
		i++;
	}
}

// 변수 4개 규칙을 만족하는 새로운 ft_unset 함수
int	ft_unset(char **args, char ***envp)
{
	int	i;
	int	var_index;
	int	ret_status;

	i = 1;
	ret_status = 0;
	while (args[i])
	{
		if (is_valid_identifier(args[i]))
		{
			var_index = find_var_index(*envp, args[i]);
			if (var_index != -1)
			{
				remove_var_at_index(envp, var_index);
			}
		}
		else
		{
			ret_status = 1;
		}
		i++;
	}
	return (ret_status);
}
