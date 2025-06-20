/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   execute_ast1.c                                      :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/16 17:27:06 by jelee          #+#    #+#                */
/*   Updated: 2025/06/16 17:27:14 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

// This is for the change of execvp > execve
static char *get_path_variable(char **envp)
{
    int i;

    i = 0;
    if (!envp)
        return (NULL);
    while(envp[i])
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
            return (envp[i] + 5); // "PATH=" 다음의 포인터 반환
        i++;
    }
    return (NULL);
}

// This is for the change of execvp > execve
static char *get_full_path(const char *cmd, char **envp)
{
    char    **paths;
    char    *path_var;
    char    *full_path;
    int     i;

    // 명령어가 비어있거나 NULL이면 경로를 찾을 수 없습니다.
    if (!cmd || cmd[0] == '\0')
        return (NULL);
    // 명령어가 이미 경로를 포함하고 있다면 (e.g., ./minishell, /bin/ls)
    // 실행 가능한지 확인하고 복사본을 반환합니다.
    if (ft_strchr(cmd, '/'))
    {
        if (access(cmd, F_OK) == 0)
            return (ft_strdup(cmd));
        return (NULL);
    }
    path_var = get_path_variable(envp);
    if (!path_var) // PATH 변수가 없으면 경로 검색 불가
        return (NULL);
    paths = ft_split(path_var, ':');
    if (!paths)
        return (NULL);
    i = -1;
    while (paths[++i])
    {
        // ft_get_total_path는 "path"와 "/"와 "cmd"를 합쳐줍니다.
        full_path = ft_get_total_path(paths[i], (char *)cmd);
        if (full_path && access(full_path, X_OK) == 0)
        {
            ft_split_clean(&paths); // 경로 배열 메모리 해제
            return (full_path); // 실행 가능한 경로 찾음
        }
        free(full_path); // 현재 경로는 실행 불가능하므로 해제
    }
    ft_split_clean(&paths); // 경로 배열 메모리 해제
    return (NULL); // 모든 경로를 확인했지만 찾지 못함
}

static void	handle_execve_error(char **argv, char *full_path)
{
	fprintf(stderr, "minishell: %s: ", argv[0]);
	// execve 실패 시의 오류 처리는 execvp와 거의 동일합니다.
	// is a directory 오류는 check_for_directory에서 처리됩니다.
	if (errno == EACCES)
		fprintf(stderr, "Permission denied\n");
	else
		perror(NULL); // 그 외의 오류는 perror가 설명해줍니다.

	free(full_path); // 할당된 경로 메모리 해제
	free_argv(argv); // 인자 배열 메모리 해제
	if (errno == EACCES)
		exit(126);
	exit(127);
}

/**
 * @brief Checks if a command path is a directory before trying to execute it.
 * This function exits with code 126 if the path is a directory.
 */
static void	check_for_directory(char **argv)
{
	struct stat	path_stat;

	if (ft_strchr(argv[0], '/'))
	{
		if (stat(argv[0], &path_stat) == 0)
		{
			if (S_ISDIR(path_stat.st_mode))
			{
				fprintf(stderr, "minishell: %s: Is a directory\n", argv[0]);
				free_argv(argv);
				exit(126);
			}
		}
	}
}

static void execute_simple_command(t_token *cmd_node, char ***envp, int l_exit)
{
    char    **argv;
    char    *full_path;

    argv = build_argv_from_ast(cmd_node, envp, l_exit);
    if (!argv)
        exit(0);

    check_for_directory(argv); // 실행 전 디렉토리인지 확인

    // 명령어의 전체 경로를 가져옵니다.
    full_path = get_full_path(argv[0], *envp);
    if (!full_path)
    {
        // 경로를 찾지 못하면 "command not found" 오류 처리
        fprintf(stderr, "minishell: %s: command not found\n", argv[0]);
        free_argv(argv);
        exit(127);
    }
    
    // execve를 사용하여 프로그램을 실행합니다.
    execve(full_path, argv, *envp);

    // execve 함수는 성공 시 반환되지 않습니다.
    // 이 라인에 도달했다면 오류가 발생한 것입니다.
    handle_execve_error(argv, full_path);
}

/**
 * @brief Handles unrecognized token types that cannot be executed.
 */
static void	handle_unknown_command(t_token *node, char ***envp)
{
	if (is_expendable_variable(node->string, *envp) == 2
		|| !ft_strncmp(node->string, "$?",
			ft_strlen_longest(node->string, "$?")))
	{
		write(STDERR_FILENO, " command not found\n", 20);
		exit(127);
	}
	if (ft_is_variable(node->string)
		&& !is_expendable_variable(node->string, *envp))
		exit(0);
	if (ft_is_variable(node->string)
		&& is_expendable_variable(node->string, *envp) == 1)
	{
		write(STDERR_FILENO, " Is a directory\n", 16);
		exit(126);
	}
	fprintf(stderr, "minishell: %s: command not found (token type %d)\n",
		node->string, node->token);
	exit(127);
}

/**
 * @brief Dispatches a command node to the correct execution function.
 */
void	dispatch_command_execution(t_token *cmd_node, char ***envp, int l_exit)
{
	if (!cmd_node)
		exit(0);
	if (cmd_node->token == PIPE)
		exit(execute_pipe(cmd_node, envp));
	else if (cmd_node->token == BUILTIN)
		exit(ft_execute_builtin(cmd_node, envp, l_exit));
	else if (cmd_node->token == CMD || cmd_node->token == WORD
		|| cmd_node->token == VAR)
		execute_simple_command(cmd_node, envp, l_exit);
	else
		handle_unknown_command(cmd_node, envp);
}
