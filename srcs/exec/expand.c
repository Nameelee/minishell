#include "exec.h"


static char *get_var_name(const char *str_after_dollar, size_t *len_of_var_in_input) {
    // str_after_dollar: '$' 바로 다음 문자를 가리킵니다.
    // len_of_var_in_input: 입력 문자열에서 변수명 부분("VAR" 또는 "{VAR}" 또는 "?")의 길이를 저장합니다.
    const char *start = str_after_dollar;
    const char *end = start;
    char *var_name = NULL;

    *len_of_var_in_input = 0;

    if (!*start) return NULL; // '$' 뒤에 아무것도 없는 경우

    if (*start == '{') { // ${VAR} 형태 처리
        end = start + 1;
        while (*end && *end != '}') {
            end++;
        }
        if (*end == '}') { // 닫는 '}'를 찾은 경우
            var_name = ft_substr(start + 1, 0, end - (start + 1));
            *len_of_var_in_input = (end - start) + 1; // "{VAR}"의 길이
        } else {
            return NULL; // 닫는 괄호가 없는 경우
        }
    } else if (*start == '?') { // $? 처리
        var_name = ft_strdup("?");
        *len_of_var_in_input = 1; // "?"의 길이
    } else if (ft_isalpha(*start) || *start == '_') { // 일반적인 변수명 (알파벳 또는 '_'로 시작)
        end = start;
        while (ft_isalnum(*end) || *end == '_') { // 알파벳, 숫자, '_'가 연속되는 부분까지
            end++;
        }
        var_name = ft_substr(start, 0, end - start);
        *len_of_var_in_input = end - start; // "VAR"의 길이
    } else {
        // 유효한 변수명 시작 문자가 아닌 경우 (예: "$$", "$ ")
        // 현재 미니쉘 요구사항에서는 이런 경우 '$'를 글자 그대로 취급할 수 있습니다.
        return NULL;
    }
    return var_name;
}

// "your_libft.h" 또는 필요한 곳에 ft_itoa, ft_strdup가 있다고 가정합니다.
// extern int g_exit_status; // 전역 변수 선언 (예: minishell.h 또는 utils.h)

// ft_itoa가 반환하는 메모리는 호출자가 free해야 한다고 가정합니다.
// 이 함수는 $? 가 포함된 문자열을 받아 $? 부분을 실제 종료 코드로 치환한
// "새로운" 문자열을 반환합니다. 반환된 문자열은 반드시 free 해주어야 합니다.
// $? 가 없으면 원본 문자열의 복사본(strdup)을 반환합니다.
char *expand_exit_status(const char *original_str) {
    const char *pattern = "$?";
    char *status_val_str = NULL;
    char *expanded_str = NULL;
    const char *current_pos = original_str;
    const char *found_pos = NULL;
    size_t original_len = strlen(original_str);
    size_t result_len = 0;
    char *write_ptr = NULL;

    // $? 패턴 찾기
    found_pos = strstr(current_pos, pattern);

    if (!found_pos) { // $? 패턴이 없으면 원본 문자열 복제본 반환
        return ft_strdup(original_str);
    }

    // $? 패턴이 하나 이상 존재함
    status_val_str = ft_itoa(g_exit_status); // g_exit_status 값을 문자열로 변환
    if (!status_val_str) {
        return ft_strdup(original_str); // ft_itoa 실패 시 원본 복제본 반환 (에러 처리)
    }
    size_t status_len = strlen(status_val_str);

    // 결과 문자열을 위한 메모리 할당 (충분히 크게, 나중에 정확한 크기로 realloc 가능)
    // 간단한 버전: $? 하나당 최대 3자리 숫자 + 널 문자 가정
    // 더 정확하려면 $? 개수를 세고, 각 $?가 status_len으로 대체될 것을 계산
    // 여기서는 우선 간단하게 충분한 버퍼를 할당하는 예시를 보입니다.
    // 좀 더 정확한 계산:
    int q_mark_count = 0;
    const char *temp_ptr = original_str;
    while ((temp_ptr = strstr(temp_ptr, pattern))) {
        q_mark_count++;
        temp_ptr += 2; // "$?" 다음으로 이동
    }
    result_len = original_len - (q_mark_count * strlen(pattern)) + (q_mark_count * status_len);
    
    expanded_str = (char *)malloc(result_len + 1);
    if (!expanded_str) {
        free(status_val_str);
        return ft_strdup(original_str); // malloc 실패 시 원본 복제본 반환
    }
    write_ptr = expanded_str;

    // 문자열 치환cd .
    current_pos = original_str; // 다시 처음부터
    while ((found_pos = strstr(current_pos, pattern)) != NULL) {
        // $? 앞부분 복사
        strncpy(write_ptr, current_pos, found_pos - current_pos);
        write_ptr += (found_pos - current_pos);

        // $? 부분을 상태 값 문자열로 대체
        strcpy(write_ptr, status_val_str);
        write_ptr += status_len;

        current_pos = found_pos + strlen(pattern); // $? 다음으로 이동
    }
    // 나머지 뒷부분 복사
    strcpy(write_ptr, current_pos);

    free(status_val_str); // ft_itoa로 할당된 메모리 해제
    return expanded_str;
}

char *expand_all_variables(const char *input_str, char **envp, bool is_single_quoted, bool is_double_quoted) {

	// --- DEBUG START ---
    // fprintf(stderr, "\nDEBUG [expand_all_variables CALLED]\n");
    // fprintf(stderr, "  input_str: [%s]\n", input_str ? input_str : "NULL");
    // fprintf(stderr, "  is_single_quoted: %d\n", is_single_quoted);
    // fprintf(stderr, "  is_double_quoted: %d\n", is_double_quoted);

    // Specifically check if we are processing "$PWD"
    // if (input_str && (ft_strlen(input_str) == 4 && ft_strncmp(input_str, "$PWD", 4) == 0)) {
    //     fprintf(stderr, "  >> Input is specifically '$PWD'. Checking env for PWD variable:\n");
    //     if (envp) {
    //         int found_pwd_in_envp = 0;
    //         for (int k = 0; envp[k] != NULL; ++k) {
    //             if (ft_strncmp(envp[k], "PWD=", 4) == 0) {
    //                 fprintf(stderr, "      Found in envp: %s\n", envp[k]);
    //                 found_pwd_in_envp = 1;
    //                 break;
    //             }
    //         }
    //         if (!found_pwd_in_envp) {
    //             fprintf(stderr, "      PWD= not found in envp.\n");
    //         }
    //     } else {
    //         fprintf(stderr, "      envp is NULL for PWD check.\n");
    //     }
    // }
    // --- DEBUG END ---

    if (is_single_quoted) {
        //fprintf(stderr, "DEBUG [expand_all_variables]: Single quote path. Returning copy of [%s]\n", input_str ? input_str : "NULL");
        char *result = ft_strdup(input_str ? input_str : ""); // Handle NULL input_str for ft_strdup
        //fprintf(stderr, "DEBUG [expand_all_variables]: Result from ft_strdup for single quote: [%s]\n", result ? result : "NULL");
        return result;
    }


    char *result_buffer = ft_strdup(""); // 최종 결과 문자열을 저장할 버퍼
    const char *current_pos = input_str; // 입력 문자열을 순회하는 포인터
    size_t var_identifier_len; // 입력에서 "$VAR" 또는 "${VAR}" 등이 차지하는 길이

    while (*current_pos) {
        // '$' 문자를 만났고, 큰따옴표 내에서 '\$'로 이스케이프된 경우가 아니라면 변수 확장을 시도
        if (*current_pos == '$' && !(is_double_quoted && current_pos > input_str && *(current_pos - 1) == '\\')) {
            char *var_name = get_var_name(current_pos + 1, &var_identifier_len); // '$' 다음 문자부터 변수명 분석

            if (var_name) { // 유효한 변수명을 추출한 경우
                char *value_str = NULL;
                if (ft_strncmp(var_name, "?", 2) == 0) { // "$?"인 경우
                    value_str = ft_itoa(g_exit_status);
                } else { // 일반 환경 변수인 경우
                    // `ft_get_env_variable` 함수는 변수명 앞에 '$'가 붙은 형태("'$VARNAME'")를 인자로 받습니다.
                    char *dollar_var_name = ft_strjoin("$", var_name);
                    if (!dollar_var_name) { /* 메모리 할당 오류 처리 */ free(var_name); free(result_buffer); return ft_strdup(""); }
                    
                    char *env_entry = ft_get_env_variable(envp, dollar_var_name); // 예: "VAR=value" 형태로 반환
                    free(dollar_var_name);

                    if (env_entry) {
                        char *eq_ptr = ft_strchr(env_entry, '=');
                        if (eq_ptr) {
                            value_str = ft_strdup(eq_ptr + 1); // '=' 다음의 값만 복사
                        } else { // '='가 없는 경우 (일반적이지 않음)
                            value_str = ft_strdup("");
                        }
                    } else { // 환경 변수가 없는 경우, 빈 문자열로 확장
                        value_str = ft_strdup("");
                    }
                }
                free(var_name);

                if (value_str) { // 추출한 값(value_str)을 결과 버퍼에 추가
                    char *temp = ft_strjoin(result_buffer, value_str);
                    free(result_buffer);
                    free(value_str);
                    result_buffer = temp;
                    if (!result_buffer) return ft_strdup(""); // 메모리 할당 오류
                }
                // current_pos를 확장된 변수 부분 다음으로 이동
                current_pos++; // '$' 건너뛰기
                current_pos += var_identifier_len; // 변수명("VAR", "{VAR}", "?") 부분 건너뛰기
            } else { // '$' 뒤에 유효한 변수명이 없는 경우 (예: "$ " 또는 문자열 끝의 '$'), '$'를 글자 그대로 처리
                char append_char[2] = {'$', '\0'};
                char *temp = ft_strjoin(result_buffer, append_char);
                free(result_buffer);
                result_buffer = temp;
                if (!result_buffer) return ft_strdup("");
                current_pos++; // '$' 문자 건너뛰기
            }
        }
        // 큰따옴표 내의 이스케이프 문자 처리 ('\$', '\"', '\\')
        else if (is_double_quoted && *current_pos == '\\' &&
                   (*(current_pos + 1) == '$' || *(current_pos + 1) == '"' || *(current_pos + 1) == '\\')) {
            current_pos++; // '\' 건너뛰고, 다음 문자를 결과 버퍼에 추가
            char append_char[2] = {*current_pos, '\0'};
            char *temp = ft_strjoin(result_buffer, append_char);
            free(result_buffer);
            result_buffer = temp;
            if (!result_buffer) return ft_strdup("");
            if (*current_pos) current_pos++; else break; // 이스케이프된 문자 건너뛰기 (NULL 아닌 경우)
        }
        // 일반 문자 처리
        else {
            char append_char[2] = {*current_pos, '\0'};
            char *temp = ft_strjoin(result_buffer, append_char);
            free(result_buffer);
            result_buffer = temp;
            if (!result_buffer) return ft_strdup("");
            current_pos++;
        }
		//fprintf(stderr, "DEBUG: current_pos char: '%c', result_buffer so far: [%s]\n", *current_pos, result_buffer);
    }
    return result_buffer;
}