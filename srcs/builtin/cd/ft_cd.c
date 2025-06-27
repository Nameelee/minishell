/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   ft_cd.c                                             :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/17 14:46:39 by jelee          #+#    #+#                */
/*   Updated: 2025/06/17 14:46:42 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin.h"

/*
** @brief      지정된 key와 path로 환경 변수를 설정하는 헬퍼 함수
** (예: key="PWD", path="/home/user")
** @param key  환경 변수의 이름 (PWD 또는 OLDPWD)
** @param path 설정할 경로 문자열
** @param envp 환경 변수 배열의 포인터
*/
static void	set_pwd_env(const char *key, const char *path, char ***envp)
{
	char	*temp_str;
	char	*full_export_str;
	char	*export_args[3];

	temp_str = ft_strjoin(key, "=");
	if (!temp_str)
		return ;
	full_export_str = ft_strjoin(temp_str, path);
	free(temp_str);
	if (!full_export_str)
		return ;
	export_args[0] = "export";
	export_args[1] = full_export_str;
	export_args[2] = NULL;
	ft_export(envp, export_args);
	free(full_export_str);
}

/*
** @brief      디렉토리 변경 후 PWD와 OLDPWD 환경 변수를 업데이트합니다.
** @param old_pwd  chdir을 호출하기 전의 이전 작업 디렉토리 경로
** @param envp     환경 변수 배열의 포인터
*/
static void	update_pwd_vars(const char *old_pwd, char ***envp)
{
	char	new_pwd_buffer[1024];

	set_pwd_env("OLDPWD", old_pwd, envp);
	if (getcwd(new_pwd_buffer, sizeof(new_pwd_buffer)) != NULL)
	{
		set_pwd_env("PWD", new_pwd_buffer, envp);
	}
}

/*
** @brief      Resolves the target path for the 'cd' command.
** @param arg  The argument from the command line (can be NULL).
** @return     The path to change to, or NULL if resolution fails.
*/
static char	*resolve_path(const char *arg)
{
	const char	*home;
	
	if (arg == NULL)
	{
		home = getenv("HOME");
		if (home == NULL)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
			return (NULL);
		}
		return (ft_strdup(home));
	}
	return (ft_strdup(arg));
}

/*
** @brief      내장 명령어 'cd'를 실행합니다.
*/
int	ft_cd(char **args, char ***envp)
{
	char	*path_to_go;
	char	old_pwd_buffer[1024];

	if (ft_get_split_len(args) > 2)
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	if (getcwd(old_pwd_buffer, sizeof(old_pwd_buffer)) == NULL)
	{
		perror("minishell: cd: getcwd");
		return (1);
	}
	path_to_go = resolve_path(args[1]);
	if (path_to_go == NULL)
		return (1);
	if (chdir(path_to_go) == -1)
	{
		perror("minishell: cd");
		free(path_to_go);
		return (1);
	}
	free(path_to_go);
	update_pwd_vars(old_pwd_buffer, envp);
	return (0);
}
