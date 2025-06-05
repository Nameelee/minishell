/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 11:52:00 by cw3l              #+#    #+#             */
/*   Updated: 2025/05/15 14:10:13 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h> 
#include <string.h> 
#include <fcntl.h> 

#include "../../../include/main.h"

#define WORD 0
#define CMD 1
#define ARG 3
#define PIPE 4
#define REDIR_OPEN 5
#define REDIR_WRITE 6
#define REDIR_WRITE_A 7
#define DOUBLE_REDIR 8
#define HEREDOC 10
#define VAR 11
#define BUILTIN 12

#ifndef IS_REDIR_OPERATOR
#define IS_REDIR_OPERATOR(type) (type == REDIR_OPEN || type == REDIR_WRITE || type == REDIR_WRITE_A || type == HEREDOC)
#define IS_FILENAME_TYPE(type) (type == WORD || type == VAR)
#define IS_PIPE_OPERATOR(type) (type == PIPE)
#define IS_ANY_OPERATOR(type) (IS_REDIR_OPERATOR(type) || IS_PIPE_OPERATOR(type))
#define IS_COMMAND_COMPONENT(type) (type == CMD || type == BUILTIN || type == WORD || type == VAR)
#define IS_ARGUMENT_TYPE(type) (type == WORD || type == VAR)

#endif


typedef enum e_heredoc_state {
    HD_NOT_PROCESSED,
    HD_PROCESSING_FAILED,
    HD_PROCESSED_OK
} t_heredoc_state;

typedef struct s_token
{
    char            *string;  
    int             token;
	bool            followed_by_whitespace;
    int             precedence;
    int             asso;
    struct s_token  *parent;
    struct s_token  *right;
    struct s_token  *left;
	int             heredoc_pipe_fd;
	int             single_quote;
    int             double_quote;
    t_heredoc_state heredoc_state;
} t_token;

typedef struct s_redir {
    int type;
    char *filename;
    t_token *heredoc_node;
    struct s_redir *next;
} t_redir;

/**
 * @brief AST 빌드 중 상태를 관리하는 구조체
 */
typedef struct s_ast_state {
    t_token **root;     // 전체 AST의 루트 노드
    t_token **csc_head; // 현재 처리 중인 간단한 명령어(simple command)의 헤드
    t_token *new_node;  // 현재 처리할 새 토큰 노드
} t_ast_state;

typedef enum e_finalize_quote_type {
    FNL_QUOTE_NONE,       // 따옴표 없음, 혹은 복합적인 경우 (기존 로직상 특별한 플래그 설정 없음)
    FNL_QUOTE_ALL_SINGLE, // 모든 세그먼트가 단일 따옴표로만 구성 (all_single && !has_unquoted && !all_double)
    FNL_QUOTE_ALL_DOUBLE  // 모든 세그먼트가 이중 따옴표로만 구성 (all_double && !has_unquoted && !all_single)
} t_finalize_quote_type;

typedef struct s_word_aggregator {
    char    **buffer_ptr;      // 현재 빌드 중인 단어 버퍼에 대한 포인터
    bool    *all_s_ptr;        // 모든 세그먼트가 단일 따옴표였는지 여부에 대한 포인터
    bool    *all_d_ptr;        // 모든 세그먼트가 이중 따옴표였는지 여부에 대한 포인터
    bool    *has_unq_ptr;      // 따옴표 없는 세그먼트가 있었는지 여부에 대한 포인터
    int     *count_ptr;        // 현재까지 처리된 세그먼트 수에 대한 포인터
    t_token **list_head_ptr;   // 오류 발생 시 전체 토큰 리스트 해제를 위한 포인터
} t_word_aggregator;

typedef struct s_parse_state {
    const char *str_content; // Renamed to avoid conflict if struct variable is 'str'
    size_t     *current_idx_ptr;
    size_t     total_len;
} t_parse_state;

typedef struct s_quoted_piece_data {
    char *extracted_str;            // 추출된 문자열 조각
    bool is_single_quoted_segment; // 현재 조각이 단일 따옴표로만 묶였는지 여부
    bool is_double_quoted_segment; // 현재 조각이 이중 따옴표로만 묶였는지 여부
    bool op_success;               // 작업 성공 여부
} t_quoted_piece_data;

typedef struct s_unquoted_piece_data {
    char *extracted_str;
    bool op_success;
} t_unquoted_piece_data;

typedef struct s_op_build_state {
    char   op_str[3]; // Buffer for the operator string (e.g., ">", ">>")
    int    type;      // Token type, initialized to WORD
    size_t op_len;    // Operator length, initialized to 1
} t_op_build_state;

typedef struct s_segment_extraction_result {
    char *piece_str;    // The extracted string segment
    bool is_single;     // True if the segment was purely single-quoted
    bool is_double;     // True if the segment was purely double-quoted
    bool success;       // True if extraction was successful
} t_segment_extraction_result;

t_token *ft_tokenize(char *str);
t_token *ft_create_ast(t_token *token_list);
t_token *ft_parse(char *str);
t_token *ft_new_token_node(char *str, int token);
void    ft_add_back_node(t_token **lst, t_token *node);
int     ft_is_commande(char *str);
int     ft_get_token(char *str);
const char *get_token_type_string(int token_type);
int     ft_count_occurence_of_token(t_token *token_lst, int token);
int     ft_delete_token_lst(t_token **token_lst);
char	*ft_get_total_path(char *path, char *str);

//tokenize

t_token *ft_handle_operator(const char *str, size_t *i, size_t input_len);
char	*ft_extract_quoted_segment(t_parse_state *p_state, char quote_char, t_word_aggregator *agg);
char *ft_extract_unquoted_segment(const char *str, size_t *i, size_t input_len);
char *ft_process_segment_concatenation(char *current_buffer, char *piece_str,
                                             t_token **list_head);
t_token *ft_finalize_word_node(char *buffer, t_finalize_quote_type quote_status,int seg_count);
t_token *ft_handle_word(const char *str, size_t *i, size_t input_len, t_token **list_head);
bool ft_append_next_segment(const char *str, size_t *idx, size_t input_len, t_word_aggregator *agg);
bool is_whitespace(char c);
bool is_operator_char(char c); 
void free_token_list(t_token *list); 
void free_single_token_node_content_and_node(t_token *node);
t_segment_extraction_result ft_extract_current_segment_info(
    const char *str, size_t *idx, size_t input_len, t_word_aggregator *agg);
t_unquoted_piece_data	ft_handle_unquoted_piece(
	const char *str_input, size_t *idx_ptr, size_t len_input, t_word_aggregator *agg);
t_quoted_piece_data	ft_handle_quoted_piece(
	const char *str_input, size_t *idx_ptr, size_t len_input, t_word_aggregator *agg);
//create_ast
t_token *get_next_node(t_token **token_list);
t_token *find_last_argument(t_token *cmd_head);
int handle_pipe(t_ast_state *s);
int handle_redir(t_ast_state *s, t_token **token_list);

//display fonction
void    ft_display_commande_lst(t_token *token_lst);
void    ft_display_token_node(t_token *token_lst);
void    ft_display_token_node_lst(t_token *token_lst);
void    ft_display_token_sequence_lst(t_token *token_lst);
void 	print_ast_start(t_token *root); 

int     ft_get_precedence(int token);
int     ft_get_associativity(int token);
char    *ft_get_str_token(int token);

//redirection
void add_redirection_to_list(t_redir **list, int type, const char *filename_or_delimiter, t_token *heredoc_node);
int apply_redirections(t_redir *list);
void free_redir_list(t_redir *list);

void    ft_binary_tree_traversal(t_token *node);

int apply_redir_heredoc(t_redir *redir_item);
int apply_redir_append(t_redir *redir_item);
int apply_redir_write(t_redir *redir_item); 
int apply_redir_open(t_redir *redir_item); 

#endif