#include "exec.h"

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

    // 문자열 치환
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