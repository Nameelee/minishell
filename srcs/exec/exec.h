/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   exec.h                                              :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/12 14:09:39 by jelee          #+#    #+#                */
/*   Updated: 2025/06/12 14:09:41 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <string.h>
# include <errno.h>
# include <sys/stat.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../../libft/libft.h"
# include "../parser/tokenizer/tokenize.h"
# include "../builtin/builtin.h"
# include "../../include/main.h"

typedef struct s_token			t_token;
typedef struct s_quote_status	t_quote_status;
typedef struct s_exec_context	t_exec_context;
typedef struct s_redir			t_redir;

typedef struct s_exec_context
{
	char	***envp_ptr;
	int		l_exit;
}	t_exec_context;

typedef struct s_exp_data
{
	const char	**pos;
	char		**buffer;
	char		**envp;
	bool		is_double_quoted;
	const char	*input_str;
}	t_exp_data;

int		execute_ast(t_token *node, char ***envp, bool is_top_level,
			int exit_status);
int		ft_execute_builtin(t_token *node, char ***envp_ptr, int exit_status);
int		preprocess_heredocs(t_token *node);
void	close_all_heredoc_fds_in_tree(t_token *node);
int		read_heredoc_to_pipe(const char *delimiter);
char	*expand_all_variables(const char *input_str, t_quote_status quotes,
			t_exec_context *ctx);
int		is_expendable_variable(char *var, char **envp);
char	**build_argv_from_ast(t_token *cmd_node, char ***envp_ptr, int l_exit);
char	**free_and_nullify_argv(char **argv, int count);
bool	process_single_token(t_token *curr, char **argv,
			int *actual_i_ptr, t_exec_context *ctx);
void	dispatch_command_execution(t_token *cmd_node, char ***envp, int i_exit);
int		execute_toplevel_command(t_token *node, char ***envp, int l_exit);
void	execute_child_command(t_token *node, char ***envp, int l_exit);
void	check_for_directory(char **argv);
void	handle_execve_error(char **argv, char *full_path);
char	*get_full_path(const char *cmd, char **envp);
int		wait_and_get_status(pid_t pid);
// expand
char	*get_var_name(const char *str_after_dollar,
			size_t *len_of_var_in_input);
char	*handle_standard_var(const char *start, size_t *len);
char	*handle_braced_var(const char *start, size_t *len);
bool	handle_variable_expansion(const char **pos, char **buffer,
			char **envp, int l_exit);
char	*append_char(char *buffer, char c);
//execute_pipe
void	free_argv(char **argv);
int		execute_pipe(t_token *node, char ***envp);
int		wait_for_children_and_get_status(pid_t left_pid, pid_t right_pid);
pid_t	fork_right_child(t_token *node, char ***envp, int pipefd[2],
			pid_t left_pid);
pid_t	fork_left_child(t_token *node, char ***envp, int pipefd[2]);
void	execute_pipe_right_child(t_token *node, char ***envp, int pipefd[2]);
void	execute_pipe_left_child(t_token *node, char ***envp, int pipefd[2]);
t_redir	*parse_redirection_nodes(t_token **cmd_node_ptr);

#endif
