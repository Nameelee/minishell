/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   inline_functions1.h                                 :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/11 17:11:02 by jelee          #+#    #+#                */
/*   Updated: 2025/06/11 17:11:09 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#ifndef INLINE_FUNCTIONS1_H
# define INLINE_FUNCTIONS1_H

# include "tokenize.h"

static inline bool	is_redir_operator(int type)
{
	return (type == REDIR_OPEN || type == REDIR_WRITE
		|| type == REDIR_WRITE_A || type == HEREDOC);
}

static inline bool	is_filename_type(int type)
{
	return (type == WORD || type == VAR);
}

static inline bool	is_pipe_operator(int type)
{
	return (type == PIPE);
}

static inline bool	is_any_operator(int type)
{
	return (is_redir_operator(type) || is_pipe_operator(type));
}

static inline bool	is_command_component(int type)
{
	return (type == CMD || type == BUILTIN || type == WORD || type == VAR);
}

#endif