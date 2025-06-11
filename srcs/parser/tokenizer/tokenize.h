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
# define TOKENIZE_H

# include <sys/types.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <sys/resource.h>
# include <sys/wait.h>
# include <unistd.h>
# include <dirent.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <dirent.h>
# include <stdbool.h>
# include <assert.h>
# include <errno.h> 
# include <string.h> 
# include <fcntl.h> 
# include "../../../include/main.h" 

# define WORD 0
# define CMD 1
# define ARG 3
# define PIPE 4
# define REDIR_OPEN 5
# define REDIR_WRITE 6
# define REDIR_WRITE_A 7
# define DOUBLE_REDIR 8
# define HEREDOC 10
# define VAR 11
# define BUILTIN 12

typedef enum e_heredoc_state {
	HD_NOT_PROCESSED,
	HD_PROCESSING_FAILED,
	HD_PROCESSED_OK
}	t_heredoc_state;

typedef struct s_token {
	char			*string;
	int				token;
	bool			followed_by_whitespace;
	int				precedence;
	int				asso;
	struct s_token	*parent;
	struct s_token	*right;
	struct s_token	*left;
	int				heredoc_pipe_fd;
	int				single_quote;
	int				double_quote;
	t_heredoc_state	heredoc_state;
}	t_token;

typedef struct s_redir {
	int				type;
	char			*filename;
	t_token			*heredoc_node;
	struct s_redir	*next;
}	t_redir;

/**
 * @brief AST 빌드 중 상태를 관리하는 구조체
 */
typedef struct s_ast_state {
	t_token	**root;
	t_token	**csc_head;
	t_token	*new_node;
}	t_ast_state;

typedef enum e_finalize_quote_type {
	FNL_QUOTE_NONE,
	FNL_QUOTE_ALL_SINGLE,
	FNL_QUOTE_ALL_DOUBLE
}	t_fin_quote;

typedef struct s_word_aggregator {
	char	**buffer_ptr;
	bool	*all_s_ptr;
	bool	*all_d_ptr;
	bool	*has_unq_ptr;
	int		*count_ptr;
	t_token	**list_head_ptr;
}	t_word_aggregator;

typedef struct s_parse_state {
	const char	*str_content;
	size_t		*current_idx_ptr;
	size_t		total_len;
}	t_parse_state;

typedef struct s_quoted_piece_data {
	char	*extracted_str;
	bool	is_single_quoted_segment;
	bool	is_double_quoted_segment;
	bool	op_success;
}	t_quoted_pdata;

typedef struct s_unquoted_piece_data {
	char	*extracted_str;
	bool	op_success;
}	t_unquot_pdata;

typedef struct s_op_build_state {
	char	op_str[3];
	int		type;
	size_t	op_len;
}	t_op_build_state;

typedef struct s_segment_extraction_result {
	char	*piece_str;
	bool	is_single;
	bool	is_double;
	bool	success;
}	t_seg_extract;

typedef struct s_word_build_state {
	char	*buffer;
	bool	all_s_q;
	bool	all_d_q;
	bool	has_unq;
	int		seg_count;
}	t_word_build_state;

typedef enum e_tokenize_status {
	TOKENIZE_SUCCESS_CONTINUE,
	TOKENIZE_SUCCESS_BREAK,
	TOKENIZE_ERROR
}	t_tokenize_status;

t_token			*ft_tokenize(char *str);
t_token			*ft_create_ast(t_token *token_list);
t_token			*ft_parse(char *str);
t_token			*ft_new_token_node(char *str, int token);
void			ft_add_back_node(t_token **lst, t_token *node);
int				ft_is_commande(char *str);
int				ft_get_token(char *str);
const char		*get_token_type_string(int token_type);
int				ft_count_occurence_of_token(t_token *token_lst, int token);
int				ft_delete_token_lst(t_token **token_lst);
char			*ft_get_total_path(char *path, char *str);

//tokenize
t_token			*ft_handle_operator(const char *str, size_t *i,
					size_t input_len);
char			*ft_extract_quoted_segment(t_parse_state *p_state,
					char quote_char, t_word_aggregator *agg);
char			*ft_extract_unquoted_segment(const char *str, size_t *i,
					size_t input_len);
char			*ft_seg_concaten(char *current_buffer, char *piece_str,
					t_token **list_head);
t_token			*ft_finalize_word_node(char *buffer, t_fin_quote quote_status,
					int seg_count);
t_token			*ft_handle_word(const char *str, size_t *i, size_t input_len,
					t_token **list_head);
bool			ft_append_next_segment(const char *str, size_t *idx,
					size_t input_len, t_word_aggregator *agg);
bool			is_whitespace(char c);
bool			is_operator_char(char c);
void			free_token_list(t_token *list);
void			free_single_token_node_content_and_node(t_token *node);
t_seg_extract	ft_extract_current_segment_info(const char *str,
					size_t *idx, size_t input_len, t_word_aggregator *agg);
t_unquot_pdata	ft_handle_unquoted_piece(const char *str_input,
					size_t *idx_ptr, size_t len_input, t_word_aggregator *agg);
t_quoted_pdata	ft_handle_quoted_piece(const char *str_input, size_t *idx_ptr,
					size_t len_input, t_word_aggregator *agg);

//create_ast
t_token			*get_next_node(t_token **token_list);
t_token			*find_last_argument(t_token *cmd_head);
t_token			*find_redir_attach_point(t_token *redir_chain);
int				handle_pipe(t_ast_state *s);
int				handle_redir(t_ast_state *s, t_token **token_list);
int				attach_new_command(t_ast_state *s);
int				is_valid_ast(t_token *root);
int				handle_command(t_ast_state *s);

//display fonction
void			ft_display_commande_lst(t_token *token_lst);
void			ft_display_token_node(t_token *token_lst);
void			ft_display_token_node_lst(t_token *token_lst);
void			ft_display_token_sequence_lst(t_token *token_lst);
void			print_ast_start(t_token *root);
int				ft_get_precedence(int token);
int				ft_get_associativity(int token);
char			*ft_get_str_token(int token);

//redirection
void			add_redirection_to_list(t_redir **list, int type,
					const char *filename_or_delimiter, t_token *heredoc_node);
int				apply_redirections(t_redir *list);
void			free_redir_list(t_redir *list);
void			ft_binary_tree_traversal(t_token *node);
int				apply_redir_heredoc(t_redir *redir_item);
int				apply_redir_append(t_redir *redir_item);
int				apply_redir_write(t_redir *redir_item);
int				apply_redir_open(t_redir *redir_item);

#endif