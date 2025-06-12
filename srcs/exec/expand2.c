/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   expand2.c                                           :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/12 14:10:15 by jelee          #+#    #+#                */
/*   Updated: 2025/06/12 14:10:19 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @brief Handles variables in the format ${VAR}.
 */
char	*handle_braced_var(const char *start, size_t *len)
{
	const char	*end;
	char		*var_name;

	end = start + 1;
	while (*end && *end != '}')
		end++;
	if (*end == '}')
	{
		var_name = ft_substr(start + 1, 0, end - (start + 1));
		*len = (end - start) + 1;
		return (var_name);
	}
	return (NULL);
}

/**
 * @brief Handles standard variables like $VAR.
 */
char	*handle_standard_var(const char *start, size_t *len)
{
	const char	*end;

	end = start;
	while (ft_isalnum(*end) || *end == '_')
		end++;
	*len = end - start;
	return (ft_substr(start, 0, *len));
}

char	*get_var_name(const char *str_after_dollar, size_t *len_of_var_in_input)
{
	const char	*start;
	char		*var_name;

	start = str_after_dollar;
	var_name = NULL;
	*len_of_var_in_input = 0;
	if (!*start)
		return (NULL);
	if (*start == '{')
		var_name = handle_braced_var(start, len_of_var_in_input);
	else if (*start == '?')
	{
		var_name = ft_strdup("?");
		*len_of_var_in_input = 1;
	}
	else if (ft_isalpha(*start) || *start == '_')
		var_name = handle_standard_var(start, len_of_var_in_input);
	else
		return (NULL);
	return (var_name);
}
