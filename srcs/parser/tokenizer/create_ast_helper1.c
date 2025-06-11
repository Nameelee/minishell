/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   create_ast_helper1.c                                :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/11 11:30:06 by jelee          #+#    #+#                */
/*   Updated: 2025/06/11 11:30:09 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"
/**
 * @brief 토큰 리스트에서 다음 노드를 분리하고 기본 초기화를 수행합니다.
 */
t_token	*get_next_node(t_token **token_list)
{
	t_token	*node;

	if (!*token_list)
		return (NULL);
	node = *token_list;
	*token_list = node->right;
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;
	return (node);
}

/**
 * @brief 명령어 앞에 오는 리다이렉션을 트리에 연결합니다. (예: > out cmd)
 */
int	attach_prefix_redir(t_ast_state *s)
{
	if (*(s->root) == NULL)
		*(s->root) = s->new_node;
	else if ((*(s->root))->token == PIPE && (*(s->root))->right == NULL)
	{
		(*(s->root))->right = s->new_node;
		s->new_node->parent = *(s->root);
	}
	else if (IS_REDIR_OPERATOR((*(s->root))->token))
	{
		s->new_node->left = *(s->root);
		(*(s->root))->parent = s->new_node;
		*(s->root) = s->new_node;
	}
	else
	{
		fprintf(stderr, "AST Error: Prefix redirection in unexpected state.\n");
		return (0);
	}
	return (1);
}

/**
 * @brief 명령어 뒤에 오는 리다이렉션을 트리에 연결합니다. (예: cmd > out)
 */
void	attach_suffix_redir(t_ast_state *s)
{
	t_token	**target_ptr;

	if ((*(s->root))->token == PIPE)
		target_ptr = &((*(s->root))->right);
	else
		target_ptr = s->root;
	s->new_node->left = *target_ptr;
	if (*target_ptr)
		(*target_ptr)->parent = s->new_node;
	*target_ptr = s->new_node;
}

/**
 * @brief 파이프 토큰 ('|')을 처리합니다.
 */
int	handle_pipe(t_ast_state *s)
{
	if (!*(s->root) || !*(s->csc_head))
	{
		fprintf(stderr, "minishell: syntax error near unexpected token `|'\n");
		return (0);
	}
	s->new_node->left = *(s->root);
	if (*(s->root))
		(*(s->root))->parent = s->new_node;
	*(s->root) = s->new_node;
	*(s->csc_head) = NULL;
	return (1);
}

/**
 * @brief 리다이렉션 토큰을 처리합니다. (***수정된 핵심 로직***)
 * 접두/접미 구분 없이 일관된 방식으로 리다이렉션을 트리의 최상단에 연결합니다.
 */
int	handle_redir(t_ast_state *s, t_token **token_list)
{
	t_token	*filename_node;
	t_token	**attach_target;

	if (!*token_list || !IS_FILENAME_TYPE((*token_list)->token))
	{
		fprintf(stderr, "minishell: syntax error: missing filename\n");
		return (0);
	}
	filename_node = get_next_node(token_list);
	s->new_node->right = filename_node;
	filename_node->parent = s->new_node;
	if (*(s->root) && (*(s->root))->token == PIPE)
		attach_target = &((*(s->root))->right);
	else
		attach_target = s->root;
	s->new_node->left = *attach_target;
	if (*attach_target)
		(*attach_target)->parent = s->new_node;
	*attach_target = s->new_node;
	return (1);
}
