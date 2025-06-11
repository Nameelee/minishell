/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   create_ast_helper2.c                                :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/11 11:33:03 by jelee          #+#    #+#                */
/*   Updated: 2025/06/11 11:33:05 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"

/**
 * @brief 현재 간단한 명령어의 마지막 인자를 찾습니다.
 * 명령어와 인자들은 'right' 포인터로 연결되어 있습니다.
 */
t_token	*find_last_argument(t_token *cmd_head)
{
	t_token	*current;

	current = cmd_head;
	if (!current)
		return (NULL);
	while (current->right && IS_ARGUMENT_TYPE(current->right->token))
	{
		current = current->right;
	}
	return (current);
}
