/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   inline_functions2.h                                 :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/11 17:11:25 by jelee          #+#    #+#                */
/*   Updated: 2025/06/11 17:11:28 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#ifndef INLINE_FUNCTIONS2_H
# define INLINE_FUNCTIONS2_H

# include "tokenize.h"

static inline bool	is_argument_type(int type)
{
	return (type == WORD || type == VAR);
}

#endif
