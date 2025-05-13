/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 12:29:53 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/09 16:13:32 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../builtin.h"
#include <errno.h>  // errno (에러 번호용)


int ft_isquote(char c)
{
    if(c == 39 || c == 34)
        return(1);
    return(0);
}

static	int ft_print_variable_int_str(char *str, int fd)
{
	char 	*code;
	
	if(str[1] == '?')
	{
		code = ft_itoa(g_exit_status);
		write(fd, code, ft_strlen(code));
		free(code);
		return(0);
	}
	return(1);
}

static int ft_putstr_echo(char *str, int fd_param, char **envp) // fd 파라미터 이름을 fd_param으로 변경
{
    size_t i;
    // (void)fd_param; // 이전 코드: fd_param을 사용하지 않았음 -> 이제 사용할 수 있도록 수정 고려
    (void)envp;

    fprintf(stderr, "DEBUG_PUTSTR_ECHO: Entered. Received str=[%s], fd_param=%d, STDOUT_FILENO=%d\n", str ? str : "NULL", fd_param, STDOUT_FILENO);
    fflush(stderr);

    i = 0;
    if (!str) { // NULL 문자열 처리
        fprintf(stderr, "DEBUG_PUTSTR_ECHO: str is NULL, returning.\n");
        fflush(stderr);
        return 0;
    }

    // while(str[i] == 32) // 앞쪽 공백 스킵은 ft_skip_space_and_quote에서 처리되었을 것으로 가정
    //      i++;

    while (str[i])
    {
        if (str[i] != '$')
        {
            if (str[i]) { // 이중 확인 불필요, while(str[i])에서 이미 처리
                fprintf(stderr, "DEBUG_PUTSTR_ECHO: Writing char '%c' (0x%02x) to STDOUT_FILENO (%d)\n", 
                        isprint(str[i]) ? str[i] : '?', (unsigned char)str[i], STDOUT_FILENO);
                fflush(stderr);
                int write_ret = write(STDOUT_FILENO, &str[i], 1); // ★ 중요: 실제 출력이 일어나는 곳
                if (write_ret == -1) {
                    fprintf(stderr, "DEBUG_PUTSTR_ECHO: write() failed for char '%c'. errno: %d, msg: %s\n", 
                            str[i], errno, strerror(errno));
                    fflush(stderr);
                    return -1; // 에러 발생 시 -1 반환
                }
            }
        }
        else // '$' 문자를 만났을 때
        {
            // ft_print_variable_int_str은 $?만 처리함. fd_param 사용.
            // $VAR는 ft_process_echo에서 ft_print_variable로 이미 처리되었어야 함.
            // 이 로직은 $?가 아닌 다른 $ 문자를 만나면 ft_print_variable_int_str을 호출하게 되는데,
            // 이는 $?가 아닐 경우 아무것도 출력하지 않고 1을 반환함.
            // 즉, "$abc" 같은 문자열은 여기서 '$'만 걸리고 ft_print_variable_int_str에서 처리 안 됨.
            // 결과적으로 'a', 'b', 'c'는 출력 안 될 수 있음.
            // echo의 기본 동작은 인자를 그대로 출력하는 것이므로, 이 $ 처리는 제거하거나 단순화해야 함.
            // 여기서는 일단 $? 처리 로직만 살려두고, 나머지는 $를 그대로 출력하도록 수정 제안.

            if (str[i+1] == '?') { // $? 인 경우
                fprintf(stderr, "DEBUG_PUTSTR_ECHO: Handling '$?' with ft_print_variable_int_str to fd_param %d\n", fd_param);
                fflush(stderr);
                // ft_print_variable_int_str은 fd_param을 사용.
                // 하지만 ft_putstr_echo는 대부분 STDOUT_FILENO를 사용하므로 fd 일관성 문제 가능성.
                // 여기서는 fd_param (ft_process_echo에서 1로 넘어옴)을 사용.
                // 리다이렉션이 적용된 STDOUT_FILENO를 일관되게 사용하는 것이 좋음.
                // 여기서는 fd_param 대신 STDOUT_FILENO를 사용하도록 변경.
                if (ft_print_variable_int_str(&str[i], STDOUT_FILENO) == 0) { // fd_param 대신 STDOUT_FILENO 사용
                     i += 1; // $와 ? 두 글자 건너뜀 (원래 i+=1 이었으므로, $? 처리 후 i++에서 ?를 건너뛰게 됨)
                } else { // $?가 아니거나 에러
                    // $?가 아닌 $문자는 그대로 출력
                    fprintf(stderr, "DEBUG_PUTSTR_ECHO: Writing char '$' (0x%02x) to STDOUT_FILENO (%d) because not $?\n", (unsigned char)'$', STDOUT_FILENO);
                    fflush(stderr);
                    if (write(STDOUT_FILENO, &str[i], 1) == -1) return -1;
                }
            } else { // $ 다음이 ?가 아닌 경우 (예: $HOME, 또는 그냥 $)
                fprintf(stderr, "DEBUG_PUTSTR_ECHO: Writing char '$' (0x%02x) to STDOUT_FILENO (%d)\n", (unsigned char)'$', STDOUT_FILENO);
                fflush(stderr);
                if (write(STDOUT_FILENO, &str[i], 1) == -1) return -1; // $ 문자 자체를 출력
            }
        }
        i++;
    }
    fprintf(stderr, "DEBUG_PUTSTR_ECHO: Finished processing string.\n");
    fflush(stderr);
    return(0);
}

int   ft_process_echo(char **split_args_passed, char **envp, int n_option, int fd_target) // 변수명 변경
{
    char  *tmp_arg_str;
    int   current_arg_idx;

    current_arg_idx = 0;
    
    fprintf(stderr, "DEBUG_PROCESS_ECHO: Entered. Target FD: %d. n_option: %d\n", fd_target, n_option);
    if (split_args_passed && split_args_passed[0]) {
         fprintf(stderr, "DEBUG_PROCESS_ECHO: First actual arg to print: [%s]\n", split_args_passed[0]);
    } else {
         fprintf(stderr, "DEBUG_PROCESS_ECHO: No actual arguments to print.\n");
    }
    fflush(stderr);
     
    while (split_args_passed[current_arg_idx])
    {
        // $PWD 관련 하드코딩은 일반적인 echo 동작과 맞지 않으므로,
        // 실제 $PWD 확장은 일반 변수 확장 메커니즘에 맡겨야 합니다.
        // 여기서는 일단 주석 처리하거나, 일반적인 echo로직을 우선합니다.
        /*
        if(ft_strncmp(split_args_passed[current_arg_idx], "\'$PWD\'", ft_strlen_longest("\'$PWD\'", split_args_passed[current_arg_idx])) == 0)
        {
            // write(STDOUT_FILENO, "$PWD\n", 5); // STDOUT_FILENO 대신 fd_target 사용해야 함
            // return(0);
        }
        */
        // ft_skip_space_and_quote 함수의 역할이 중요합니다.
        // 이 함수가 새 메모리를 할당한다면 free(tmp_arg_str) 필요.
        // 여기서는 원본을 수정하지 않고 포인터만 변경하거나, 새 메모리를 반환한다고 가정.
        //tmp_arg_str = ft_skip_space_and_quote(split_args_passed[current_arg_idx]); // 이 함수의 동작을 알아야 함
		tmp_arg_str = split_args_passed[current_arg_idx]; // 이 함수의 동작을 알아야 함
        fprintf(stderr, "DEBUG_PROCESS_ECHO: Processing arg [%s], after skip_space_and_quote: [%s]\n", 
                split_args_passed[current_arg_idx], tmp_arg_str ? tmp_arg_str : "NULL");
        fflush(stderr);

        if (tmp_arg_str)
        {
            // $VAR (non-$?) 확장 및 $PWD 특별 처리는 여기서 하면 안 됩니다. 
            // echo는 받은 문자열을 그대로 출력해야 합니다.
            // $? 확장은 ft_putstr_echo 내부 (또는 그 이전의 일반 확장 단계)에서 처리.
            // 여기서는 ft_putstr_echo를 직접 호출하여 처리합니다.
            // ft_putstr_echo에 fd_target을 전달해야 합니다. (이전엔 1로 하드코딩)

            fprintf(stderr, "DEBUG_PROCESS_ECHO: Calling ft_putstr_echo with tmp_arg_str=[%s], fd=%d\n", 
                    tmp_arg_str ? tmp_arg_str : "NULL", fd_target); // fd_target을 전달
            fflush(stderr);
            if(ft_putstr_echo(tmp_arg_str, fd_target, envp) == -1) // fd_target 전달, envp는 ft_putstr_echo에서 $? 외엔 불필요
                 return (1); // 에러 시 1 반환 (Bash는 보통 성공 시 0)
            
            if (split_args_passed[current_arg_idx + 1] != NULL) {
                 // && !ft_isquote(split_args_passed[current_arg_idx + 1][0])) // 이 조건은 불필요
                 fprintf(stderr, "DEBUG_PROCESS_ECHO: Printing space to fd %d\n", fd_target);
                 fflush(stderr);
                 if(write(fd_target, " ", 1) == -1) return 1; // fd_target 사용
            }
        }
        
        // ft_skip_space_and_quote가 새 메모리를 할당했다면 여기서 free(tmp_arg_str);
        // 그렇지 않고 split_args_passed[current_arg_idx] 내부 포인터라면 해제하면 안됨.
        // 사용자님의 ft_skip_space_and_quote 구현에 따라 달라집니다.
        // 여기서는 일단 tmp_arg_str을 free 하지 않는다고 가정. (만약 해야 한다면 추가 필요)
        // free(tmp_arg_str); // <--- ft_skip_space_and_quote 구현에 따라 결정

        current_arg_idx++;
    }

    if (!n_option) { // -n 옵션이 없을 때
        fprintf(stderr, "DEBUG_PROCESS_ECHO: Printing newline to fd %d\n", fd_target);
        fflush(stderr);
        if(write(fd_target, "\n", 1) == -1) return 1;
    }
    
    fprintf(stderr, "DEBUG_PROCESS_ECHO: Finished. Returning 0 (success).\n");
    fflush(stderr);
    return(0); // 성공 시 0 반환
}

int   ft_echo(char **split_args_from_caller, int fd_to_use, char **envp) // 변수명 변경
{
    int     i = 1; // split_args_from_caller[0]은 "echo"
    int     n_option_flag = 0;
    int     child_status_val; // 자식 프로세스 종료 상태 저장

    // 이 함수는 이미 ft_execute_builtin에 의해 fork된 자식(C1)에서 실행되어야 합니다.
    // 따라서 ft_echo 내부에서 또 fork하는 것은 불필요하며 문제를 야기할 수 있습니다.
    // 만약 fork를 제거한다면, ft_split_clean도 여기서 하면 안됩니다.
    // 아래는 현재 구조를 유지하되, 종료 상태를 올바르게 반환하도록 수정하는 예시입니다.

    fprintf(stderr, "DEBUG_ECHO_MAIN: Entered. Received fd_to_use: %d\n", fd_to_use);
    fflush(stderr);

    // `-n` 옵션 처리
    // 주의: bash의 echo는 여러 개의 -n (예: -n -n -n) 이나 -nnnn 같은 것도 처리합니다.
    // 여기서는 첫 번째 인자만 확인하는 간단한 버전입니다.
    if (split_args_from_caller[i] && 
        ft_strncmp(split_args_from_caller[i], "-n", ft_strlen_longest(split_args_from_caller[i], "-n")) == 0
        /* && 여기에 추가로 -n 뒤에 다른 문자가 없는지 확인하는 로직이 있으면 더 정확 */
        /* 예: (ft_strlen(split_args_from_caller[i]) == 2 || (ft_strlen(split_args_from_caller[i]) > 2 && split_args_from_caller[i][2] == 'n' ...)) */
       )
    {
        n_option_flag = 1;
        i++; // 실제 출력할 인자의 시작 인덱스를 다음으로 넘김
    }
    
    // ft_process_echo 호출 시, 명령어("echo")와 옵션("-n")은 제외하고 전달
    child_status_val = ft_process_echo(&split_args_from_caller[i], envp, n_option_flag, fd_to_use);
    
    // ft_split_clean(&split_args_from_caller); // ★★★ 이 부분은 ft_execute_builtin에서 처리해야 하므로 여기서 하면 안됨 ★★★

    // ft_echo는 직접 exit() 하는 대신, 계산된 상태를 반환해야 합니다.
    // ft_execute_builtin이 이 반환값을 받아 exit() 합니다.
    fprintf(stderr, "DEBUG_ECHO_MAIN: ft_process_echo returned %d. ft_echo will return this status.\n", child_status_val);
    fflush(stderr);
    return(child_status_val);


    // --- 이전 fork 로직 (제거 권장) ---
    // pid = fork();
    // if(pid == 0)
    // {
    //     if(!split_args_from_caller || !*split_args_from_caller) exit(1);
    //     if(!split_args_from_caller[i])
    //         write(fd_to_use,"\n",1); // fd_to_use 사용
    //     else
    //     {
    //         if (ft_strncmp(split_args_from_caller[i], "-n", ft_strlen_longest(split_args_from_caller[i], "-n")) == 0)
    //         {
    //             n_option_flag = 1;
    //             i++;
    //         }
    //         ft_process_echo(&split_args_from_caller[i], envp, n_option_flag, fd_to_use);
    //     }
    //     // ft_split_clean(&split_args_from_caller); // ★★★ 여기서 호출하면 안됨 ★★★
    //     exit(0); // ★★★ ft_process_echo 결과와 관계없이 항상 0으로 종료하는 문제 ★★★
    // }
    // else
    // {
    //     waitpid(pid, &status, 0);
    //     if(WIFEXITED(status)) { return(WEXITSTATUS(status)); }
    //     else { return(0); } // 또는 다른 에러 처리
    // }
    // return(0); // 이 부분도 도달하지 않아야 함
}