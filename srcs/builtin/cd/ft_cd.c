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

// /**
//  * @brief Determines the target path for the 'cd' command.
//  * @param args The arguments passed to 'cd'.
//  * @return Allocated string representing the target path, or NULL on error.
//  */
// static char	*get_target_path(char **args)
// {
// 	char	*path_arg;

// 	path_arg = args[1];
// 	if (path_arg == NULL || path_arg[0] == '\0'
// 		|| (path_arg[0] == '~' && (path_arg[1] == '\0' || path_arg[1] == '/')))
// 	{
// 		return (ft_get_home_path(path_arg,
// 				(path_arg == NULL || path_arg[0] == '\0')));
// 	}
// 	else
// 	{
// 		return (ft_strdup(path_arg));
// 	}
// }

// static void	print_cd_error(char *original_arg, char *used_path)
// {
// 	ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
// 	if (original_arg && (original_arg[0] == '~'
// 			|| ft_strchr(original_arg, '$')))
// 		ft_putstr_fd(original_arg, STDERR_FILENO);
// 	else
// 		ft_putstr_fd(used_path, STDERR_FILENO);
// 	ft_putstr_fd(": ", STDERR_FILENO);
// 	perror(NULL);
// }

// 환경 변수를 설정하거나 추가하는 헬퍼 함수
// static int	set_env_var(const char *key, const char *value, char ***envp)
// {
// 	char	*new_var_str;
// 	char	*full_var_str;
// 	char	**new_vars;
// 	char	**temp_env;

// 	if (!key || !value || !envp)
// 		return (1);
// 	new_var_str = ft_strjoin(key, "=");
// 	if (!new_var_str)
// 		return (1);
// 	full_var_str = ft_strjoin(new_var_str, value);
// 	free(new_var_str);
// 	if (!full_var_str)
// 		return (1);
// 	new_vars = (char **)malloc(sizeof(char *) * 2);
// 	if (!new_vars)
// 	{
// 		free(full_var_str);
// 		return (1);
// 	}
// 	new_vars[0] = full_var_str;
// 	new_vars[1] = NULL;
// 	temp_env = ft_add_variable_to_env(*envp, new_vars);
// 	free(new_vars); // new_vars 배열 자체만 해제, 안의 문자열은 temp_env로 이동
// 	if (!temp_env)
// 	{
// 		free(full_var_str);
// 		return (1);
// 	}
// 	free(*envp); // 이전 환경 변수 목록 메모리 해제
// 	*envp = temp_env; // 새로 만들어진 환경 변수 목록으로 교체
// 	return (0);
// }

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

	// "KEY=" 문자열을 만듭니다. (예: "PWD=")
	temp_str = ft_strjoin(key, "=");
	if (!temp_str)
		return ;
	// "KEY=PATH" 전체 문자열을 만듭니다. (예: "PWD=/home/user")
	full_export_str = ft_strjoin(temp_str, path);
	free(temp_str);
	if (!full_export_str)
		return ;
	// ft_export에 전달할 인자 배열을 구성합니다.
	export_args[0] = "export";
	export_args[1] = full_export_str;
	export_args[2] = NULL;
	// 기존의 export 함수를 재활용하여 환경 변수를 설정합니다.
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

	// 1. OLDPWD를 이전 경로로 설정합니다.
	set_pwd_env("OLDPWD", old_pwd, envp);
	// 2. 현재 경로를 얻어와 PWD로 설정합니다.
	if (getcwd(new_pwd_buffer, sizeof(new_pwd_buffer)) != NULL)
	{
		set_pwd_env("PWD", new_pwd_buffer, envp);
	}
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
	// 디렉토리를 변경하기 전에 현재 경로를 저장합니다.
	if (getcwd(old_pwd_buffer, sizeof(old_pwd_buffer)) == NULL)
	{
		perror("minishell: cd: getcwd");
		return (1);
	}
	path_to_go = args[1];
	if (path_to_go == NULL)
		path_to_go = getenv("HOME");
	if (path_to_go == NULL)
	{
		ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
		return (1);
	}
	if (chdir(path_to_go) == -1)
	{
		perror("minishell: cd");
		return (1);
	}
	// 디렉토리 변경이 성공하면, 환경 변수를 업데이트합니다.
	update_pwd_vars(old_pwd_buffer, envp);
	return (0);
}