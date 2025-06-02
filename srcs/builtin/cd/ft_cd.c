/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ast <ast@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 09:29:37 by cw3l              #+#    #+#             */
/*   Updated: 2025/06/02 17:05:15 by ast              ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../builtin.h"

char	*ft_trim_path(char *path)
{
	int	i;
	int	len;

	len = ft_strlen(path);
	i = 0;
	while (path[i] == 32)
		i++;
	while (path[i] == 32)
		i++;
	ft_memmove(path, &path[i], len - i);
	path[len - i] = '\0';
	return (path);
}

char	*ft_get_home_path(char *path, int cd)
{
	char	*env;
	char	*total_path;

	env = getenv("HOME");
	if (!env)
		return (NULL);
	if (cd == 0)
		total_path = ft_strjoin(env, &path[1]);
	else
		total_path = env;
	if (!total_path)
		return (NULL);
	return (total_path);
}

int	ft_check_path(char **split)
{
	if (ft_get_split_len(split) == 2)
		return (0);
	else if (ft_get_split_len(split) > 2)
		return (0);
	else
	{
		if (ft_get_split_len(split) == 1
			&& ft_strncmp(split[0], "$PWD", 4) == 0)
			return (2);
		if (ft_get_split_len(split) <= 2 && ft_get_split_len(split) > 0)
			return (1);
	}
	return (0);
}

int	ft_return_to_home(char *path)
{
	char	*home_path;
	int		chdir_return;

	chdir_return = 1;
	home_path = ft_get_home_path(path, 1);
	if (home_path)
		chdir_return = chdir(home_path);
	if (chdir_return == -1)
	{
		write(STDERR_FILENO, " Aucun fichier ou dossier de ce nom",
			ft_strlen(" Aucun fichier ou dossier de ce nom"));
		return (1);
	}
	return (0);
}

/*
int	ft_cd(char **args)
{
	int		chdir_return;
	char	*path;

	if (ft_check_path(&args[1]) == 2)
		return (0);
	else if (ft_check_path(&args[1]) == 0)
		return (print_cd_message(0));
	if (ft_get_split_len(args) == 1)
		return (ft_return_to_home(args[0]));
	else
	{
		path = ft_trim_path(args[1]);
		if (path[0] == '~' )
			path = ft_get_home_path(path, 0);
	}
	chdir_return = chdir(path);
	if (chdir_return == -1)
		return (print_cd_message(1));
	return (0);
}
*/

int	ft_cd(char **args) // args는 전체 명령어 라인 (예: {"cd", "$PWD", "hi", NULL})
{
	char	*path_to_go;
    char    *original_path_arg = NULL;
    char    *path_after_expansion = NULL; // 변수 확장 등을 거친 실제 경로 문자열
	int		chdir_return;
    int     return_status = 0; // 성공 시 0, 실패 시 1 (또는 다른 에러 코드)

    // args[0]은 "cd"
    // args[1]이 실제 경로 인자 (또는 NULL)
	if(ft_get_split_len(args) > 2)
	{
		write(STDERR_FILENO, " too many arguments\n", ft_strlen(" too many arguments\n"));
		return (1);
	}
	if (args[1] == NULL || args[1][0] == '\0' || (args[1][0] == '~' && (args[1][1] == '\0' || args[1][1] == '/'))) {
        // 경우 1: "cd" 또는 "cd ~" 또는 "cd ~/"
        // "$PWD" 와 "hi"가 있는 경우, args[1]은 "$PWD"가 될 것임.
        // "~" 로 시작하는 경우, ft_get_home_path가 처리.
        // args[1]이 NULL (cd만 입력) 또는 비어있는 문자열인 경우 HOME으로.
        bool go_home_directly = (args[1] == NULL || args[1][0] == '\0');
        path_to_go = ft_get_home_path(args[1] ? args[1] : "~", go_home_directly); // args[1]이 NULL이면 "~"를 기준으로 HOME 경로 가져옴
        if (!path_to_go) { // HOME을 못 얻어오거나 메모리 할당 실패
            return (1); // 실패
        }
        original_path_arg = NULL; // ft_get_home_path가 새 문자열을 할당했으므로 따로 free할 원본 없음
	} else {
		// 경우 2: "cd path" (path는 $PWD, 일반경로 등)
        // args[1]을 사용. 추가 인자(args[2] 이후)는 bash처럼 무시.
        original_path_arg = args[1]; // 원본 인자 포인터 (free 불필요, argv의 일부)
        // 여기서 $PWD와 같은 환경 변수 확장이 필요.
        // ft_cd로 전달되는 args는 이미 확장이 끝난 상태라고 가정해야 함 (minishell 주 실행 루프에서 처리).
        // 만약 ft_cd가 직접 확장을 해야 한다면, expand_variable(args[1], envp) 와 같은 함수 호출 필요.
        // 여기서는 args[1]이 이미 확장된 경로라고 가정하고 진행.
        path_after_expansion = ft_strdup(original_path_arg); // 실제 사용할 경로 문자열 복사
        if (!path_after_expansion) {
            perror("minishell: ft_strdup for path");
            return (1);
        }

        // 경로 앞뒤 공백 제거 (필요하다면)
        // path_to_go = ft_trim_path(path_after_expansion); // ft_trim_path는 원본을 변경하므로 주의
        // 여기서는 ft_trim_path가 원본 변경한다고 가정하지 않고, path_after_expansion을 직접 사용
        path_to_go = path_after_expansion;
	}

    // Test 40 ("cd $PWD hi")의 경우,
    // args = {"cd", "/expanded/path/of/PWD", "hi", NULL} 로 들어온다고 가정.
    // path_to_go는 "/expanded/path/of/PWD" 를 가리키게 됨.

	if (path_to_go) { // 유효한 경로 문자열이 준비되었으면
        chdir_return = chdir(path_to_go); // 실제 디렉토리 변경 시도
        if (chdir_return == -1)
        {
            // chdir 실패 시 bash와 유사한 오류 메시지 출력
            ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
            // path_to_go 대신 오류 발생 시점의 "원본 사용자 입력 인자" (확장 전 $PWD 또는 그냥 경로)를 보여주는 것이 더 자연스러울 수 있음.
            // 하지만 여기서는 실제 시도한 path_to_go를 사용.
            // Test 40의 경우, 사용자가 입력한 것은 $PWD. 실제 시도 경로는 /expanded/path.
            // bash는 "cd: $PWD: No such file or directory" (만약 $PWD가 없는 경로라면) 식으로 보여줌.
            // 또는 "cd: some_path: No such file or directory"
            // 여기서는 original_path_arg (변수 확장 전 또는 ~, 없는 경우엔 path_to_go)를 사용 고려.
            if (args[1] && (args[1][0] == '~' || ft_strchr(args[1], '$'))) { // 사용자가 ~나 변수를 썼다면 그것을 보여줌
                 ft_putstr_fd(args[1], STDERR_FILENO);
            } else {
                 ft_putstr_fd(path_to_go, STDERR_FILENO);
            }
            ft_putstr_fd(": ", STDERR_FILENO);
            perror(NULL); // OS의 에러 메시지 (예: "No such file or directory")
            return_status = 1; // 실패
        } else {
            // OLDPWD, PWD 환경변수 업데이트 로직 필요 (ft_export 등으로)
            // 예: char *cwd = getcwd(NULL, 0); set_env("PWD", cwd, envp); free(cwd);
            return_status = 0; // 성공
        }
    } else { // path_to_go가 NULL인 경우 (ft_get_home_path 등에서 실패)
        // 오류 메시지는 ft_get_home_path 등에서 이미 출력되었을 수 있음
        return_status = 1; // 실패
    }

    // path_to_go가 ft_get_home_path 또는 ft_strdup으로 할당된 경우 free
    if (path_to_go && (path_to_go != original_path_arg)) { // path_to_go가 args[1]을 직접 가리키는게 아니라 새로 할당된 경우만
        free(path_to_go);
    }

	return (return_status);
}
