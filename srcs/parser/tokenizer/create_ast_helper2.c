#include "tokenize.h"


/**
 * @brief 현재 간단한 명령어의 마지막 인자를 찾습니다.
 * 명령어와 인자들은 'right' 포인터로 연결되어 있습니다.
 */
t_token *find_last_argument(t_token *cmd_head)
{
    t_token *current = cmd_head;

    if (!current)
        return (NULL);

    // 다음 토큰이 있고, 그 토큰의 타입이 인자 타입(WORD 또는 VAR)인 동안 계속 진행
    while (current->right && IS_ARGUMENT_TYPE(current->right->token))
    {
        current = current->right;
    }
    return (current);
}
