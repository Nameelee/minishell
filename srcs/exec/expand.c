/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   expand.c                                            :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/06/12 14:09:58 by jelee          #+#    #+#                */
/*   Updated: 2025/06/12 14:10:02 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

/**
 * @brief Counts occurrences of a pattern in a string.
 */
static int	count_occurrences(const char *str, const char *pattern)
{
	int			count;
	const char	*temp_ptr;

	count = 0;
	temp_ptr = str;
	while (1)
	{
		temp_ptr = strstr(temp_ptr, pattern);
		if (!temp_ptr)
			break ;
		count++;
		temp_ptr += strlen(pattern);
	}
	return (count);
}

/**
 * @brief Replaces all occurrences of "$?" with the status string.
 */
static void	perform_exit_status_replacement(char *dest, const char *src,
	const char *status_str)
{
	const char	*current_pos;
	const char	*found_pos;
	size_t		status_len;

	current_pos = src;
	status_len = strlen(status_str);
	found_pos = strstr(current_pos, "$?");
	while (found_pos != NULL)
	{
		strncpy(dest, current_pos, found_pos - current_pos);
		dest += (found_pos - current_pos);
		strcpy(dest, status_str);
		dest += status_len;
		current_pos = found_pos + 2;
	}
	strcpy(dest, current_pos);
}

char	*expand_exit_status(const char *original_str)
{
	char	*status_val_str;
	char	*expanded_str;
	size_t	result_len;
	int		q_mark_count;

	if (!strstr(original_str, "$?"))
		return (ft_strdup(original_str));
	status_val_str = ft_itoa(g_exit_status);
	if (!status_val_str)
		return (ft_strdup(original_str));
	q_mark_count = count_occurrences(original_str, "$?");
	result_len = strlen(original_str) - (q_mark_count * 2)
		+ (q_mark_count * strlen(status_val_str));
	expanded_str = (char *)malloc(result_len + 1);
	if (!expanded_str)
	{
		free(status_val_str);
		return (ft_strdup(original_str));
	}
	perform_exit_status_replacement(expanded_str, original_str, status_val_str);
	free(status_val_str);
	return (expanded_str);
}

int	is_expendable_variable(char *var, char **envp)
{
	int		i;
	char	*var_env;

	i = 0;
	if (var[0] == '$' && var[1] == '\0')
		return (2);
	while (envp[i])
	{
		var_env = ft_extract_var(envp[i]);
		if (!ft_strncmp(var_env, &var[1], ft_strlen_longest(var, var_env)))
			return (1);
		i++;
	}
	return (0);
}
