/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_quick_sort.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ast <ast@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:07:08 by cw3l              #+#    #+#             */
/*   Updated: 2025/04/30 15:54:55 by ast              ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../../builtin.h"

static char	*ft_extract_var(char *env_var)
{
	char	*var;
	int		equal_idx;

	if (!env_var)
		return (NULL);
	equal_idx = ft_index_of_char(env_var, '=');
	if (equal_idx == -1)
		return (ft_strdup(env_var));
	var = ft_substr(env_var, 0, equal_idx);
	if(!var)
		return(NULL);
	return (var);
}

static int	str_env_cmp_process(char *s1, char *s2, size_t n)
{
	size_t	i;
	char	*trimed_s1;
	char	*trimed_s2;

	i = 0;
	trimed_s1 = ft_extract_var(s1);
	trimed_s2 = ft_extract_var(s2);
	while ((trimed_s1[i] || trimed_s2[i]) && i < n)
	{
		if ((unsigned char)trimed_s1[i] != (unsigned char)trimed_s2[i])
		{
			if ((unsigned char)trimed_s1[i] > (unsigned char)trimed_s2[i])
			{
				free(trimed_s1);
				free(trimed_s2);
				return (1);
			}
			if ((unsigned char)trimed_s1[i] < (unsigned char)trimed_s2[i])
			{
				free(trimed_s1);
				free(trimed_s2);
				return (-1);
			}
		}
		i++;
	}
	free(trimed_s1);
	free(trimed_s2);
	return (0);
}


int	ft_str_env_cmp(char *s1, char *s2, size_t n)
{
	int		process_r;

	if (n == 0)
		return (0);
	process_r = str_env_cmp_process(s1, s2, ft_strlen_longest(s1, s2));
	if (process_r != 0)
		return (process_r);
	return (0);
}

static int	ft_part(char **s, int end, int (*cmp)(char *, char *, size_t len))
{
	int		i;
	int		j;
	char	*pivot;

	i = 0;
	j = end - 1;
	pivot = s[end / 2];
	while (i < j)
	{
		while (cmp(s[i], pivot, ft_strlen(s[i])) < 0)
			i++;
		while (cmp(s[j], pivot, ft_strlen(s[j])) > 0)
			j--;
		if (cmp(s[i], s[j], ft_strlen(s[i])) > 0)
			ft_swap_str(&s[i], &s[j]);
	}
	return (i);
}

void	env_quick_s(char **s, int end, int (*cmp)(char *, char *, size_t len))
{
	int	i;

	if (end == 0)
		return ;
	i = ft_part(s, end, cmp);
	env_quick_s(s, i, cmp);
	env_quick_s(&s[i + 1], end - 1 - i, cmp);
}
