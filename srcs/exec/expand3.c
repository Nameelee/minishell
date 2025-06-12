/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   expand3.c                                           :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/12 14:10:29 by jelee          #+#    #+#                */
/*   Updated: 2025/06/12 14:10:32 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @brief Appends a single character to a buffer, handling memory reallocation.
 */
char	*append_char(char *buffer, char c)
{
	char	append_str[2];
	char	*new_buffer;

	append_str[0] = c;
	append_str[1] = '\0';
	new_buffer = ft_strjoin(buffer, append_str);
	free(buffer);
	return (new_buffer);
}

/**
 * @brief Gets the value of a variable ('$' or from the environment).
 * Returns a newly allocated string that must be freed by the caller.
 */
static char	*get_var_value(const char *var_name, char **envp)
{
	char	*dollar_var_name;
	char	*env_entry;
	char	*eq_ptr;

	if (ft_strncmp(var_name, "?", 2) == 0)
		return (ft_itoa(g_exit_status));
	dollar_var_name = ft_strjoin("$", var_name);
	if (!dollar_var_name)
		return (ft_strdup(""));
	env_entry = ft_get_env_variable(envp, dollar_var_name);
	free(dollar_var_name);
	if (env_entry)
	{
		eq_ptr = ft_strchr(env_entry, '=');
		if (eq_ptr)
			return (ft_strdup(eq_ptr + 1));
	}
	return (ft_strdup(""));
}

/**
 * @brief Gets the value for a variable name and appends it to the buffer.
 *
 * @param buffer A pointer to the result buffer string, which will be modified.
 * @param var_name The name of the variable to expand (e.g., "USER" or "?").
 * @param envp The environment variable array.
 * @return Returns true on success, false on memory allocation failure.
 */
static bool	ft_expand_and_append_value(char **buffer, const char *var_name,
	char **envp)
{
	char	*value_str;
	char	*temp_buffer;

	value_str = get_var_value(var_name, envp);
	if (!value_str)
		return (false);
	temp_buffer = ft_strjoin(*buffer, value_str);
	free(*buffer);
	free(value_str);
	*buffer = temp_buffer;
	if (!*buffer)
		return (false);
	return (true);
}

/**
 * @brief Handles a '$' character, dispatching to expand a variable or
 * treat it as a literal. Advances the position pointer.
 */
bool	handle_variable_expansion(const char **pos, char **buffer, char **envp)
{
	char	*var_name;
	size_t	var_len_in_input;

	var_name = get_var_name(*pos + 1, &var_len_in_input);
	if (var_name)
	{
		if (!ft_expand_and_append_value(buffer, var_name, envp))
		{
			free(var_name);
			return (false);
		}
		free(var_name);
		*pos += var_len_in_input + 1;
	}
	else
	{
		*buffer = append_char(*buffer, *(*pos));
		(*pos)++;
	}
	if (!*buffer)
		return (false);
	return (true);
}
