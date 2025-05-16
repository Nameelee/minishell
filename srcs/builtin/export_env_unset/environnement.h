/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   environnement.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 13:06:51 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/16 12:51:32 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENVIRONNEMENT_H
# define ENVIRONNEMENT_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <fcntl.h>

# include "../../parser/tokenizer/tokenize.h"

// variable tools
int		ft_is_variable(char *str);
char	*ft_parse_env_variable(char *env_var);
int		ft_count_valide_variable(char **var, char **envp, int *err);
int		ft_check_variable_export(char *var, char **envp);
int		ft_print_variable_int_str(char *str);
int		ft_print_variable(char **envp, char *str, int *idx);

// environnement tools
char	*ft_get_env_variable(char **envp, char *var);
int		ft_get_var_idx_in_env(char **envp, char *var);
char	**ft_add_variable_to_env(char **old_env, char **new_var);
int		ft_is_var_on(char **env, char *var);
void	clean_swap(char **new, char **old);
char	*ft_extract_var(char *env_var);

//quick sort
void	env_quick_s(char **s, int end, int (*cmp)(char *, char *, size_t len));
int		ft_str_env_cmp(char *s1, char *s2, size_t n);

//display
int		display_export_env(char ***env);
void	ft_print_env(char **env);

#endif