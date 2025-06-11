/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   create_ast.c                                        :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/11 11:04:20 by jelee          #+#    #+#                */
/*   Updated: 2025/06/11 11:04:24 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"

t_token	*find_redir_attach_point(t_token *redir_chain)
{
	t_token	*current;

    current = redir_chain;
    while (current->left && IS_REDIR_OPERATOR(current->left->token))
        current = current->left;
    return (current);
}

static	int handle_command(t_ast_state *s)
{
	t_token	*attach_point;
    if (*(s->csc_head) == NULL)
	{
        *(s->csc_head) = s->new_node;
        if (!attach_new_command(s))
			return (0);
    }
	else
	{
        attach_point = find_last_argument(*(s->csc_head));
        if (!attach_point)
			return (0);
        attach_point->right = s->new_node;
        s->new_node->parent = attach_point;
    }
    return (1);
}
/**
 * @brief 완성된 AST가 문법적으로 유효한지 최종 검사합니다.
 */
static int	is_valid_ast(t_token *root)
{
    if (root && root->token == PIPE && (!root->left || !root->right))
    {
        fprintf(stderr, "minishell: syntax error: incomplete pipe\n");
        return (0);
    }
    return (1);
}

/**
 * @brief 토큰 리스트로부터 추상 구문 트리(AST)를 생성합니다.
 *
 * @param token_list 렉서에서 생성된 토큰의 연결 리스트.
 * @return 생성된 AST의 루트 노드를 반환합니다. 실패 시 NULL을 반환합니다.
 */
t_token *ft_create_ast(t_token *token_list)
{
    t_token     *root;
    t_token     *csc_head; // current_simple_cmd_head
    t_ast_state state;

    root = NULL;
    csc_head = NULL;
    state.root = &root;
    state.csc_head = &csc_head;
    while (token_list)
    {
        state.new_node = get_next_node(&token_list);
        if (IS_PIPE_OPERATOR(state.new_node->token)) {
            if (!handle_pipe(&state)) return (NULL);
        } else if (IS_REDIR_OPERATOR(state.new_node->token)) {
            if (!handle_redir(&state, &token_list)) return (NULL);
        } else if (IS_COMMAND_COMPONENT(state.new_node->token)) {
            if (!handle_command(&state)) return (NULL);
        } else {
            fprintf(stderr, "AST Error: Unhandled token type %d\n", state.new_node->token);
            return (NULL);
        }
    }
    if (!is_valid_ast(root))
        return (NULL); // 최종 유효성 검사 실패
    return (root);
}


