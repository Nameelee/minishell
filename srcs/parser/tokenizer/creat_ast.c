#include "tokenize.h"

static t_token *find_redir_attach_point(t_token *redir_chain)
{
    t_token *current = redir_chain;
    while (current->left && IS_REDIR_OPERATOR(current->left->token))
        current = current->left;
    return (current);
}

static int attach_new_command(t_ast_state *s)
{
    t_token *root = *(s->root);
    t_token *new_cmd = s->new_node;
    t_token *attach_point;

    if (root == NULL) { // 1. AST가 비었을 때
        *(s->root) = new_cmd;
    } else if (root->token == PIPE) { // 2. 파이프 오른쪽
        attach_point = root->right;
        if (!attach_point) { // 2a. 파이프 바로 뒤 명령어 (ls | wc)
            root->right = new_cmd;
            new_cmd->parent = root;
        } else { // 2b. 리다이렉션 뒤 명령어 (ls | >out wc)
            attach_point = find_redir_attach_point(attach_point);
            attach_point->left = new_cmd;
            new_cmd->parent = attach_point;
        }
    } else if (IS_REDIR_OPERATOR(root->token)) { // 3. 리다이렉션 뒤 명령어
        attach_point = find_redir_attach_point(root);
        attach_point->left = new_cmd;
        new_cmd->parent = attach_point;
    } else {
        fprintf(stderr, "AST Error: Command in unexpected state.\n");
        return (0);
    }
    return (1);
}

static int handle_command(t_ast_state *s)
{
    if (*(s->csc_head) == NULL) {
        *(s->csc_head) = s->new_node;
        if (!attach_new_command(s)) return (0);
    } else {
        t_token *attach_point = find_last_argument(*(s->csc_head));
        if (!attach_point) return (0);
        attach_point->right = s->new_node;
        s->new_node->parent = attach_point;
    }
    return (1);
}
/**
 * @brief 완성된 AST가 문법적으로 유효한지 최종 검사합니다.
 */
static int is_valid_ast(t_token *root)
{
    if (root && root->token == PIPE && (!root->left || !root->right))
    {
        fprintf(stderr, "minishell: syntax error: incomplete pipe\n");
        return (0);
    }
    // 필요에 따라 다른 유효성 검사 추가
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


