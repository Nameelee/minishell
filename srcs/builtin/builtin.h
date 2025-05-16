/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 12:18:12 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/16 12:19:20 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTIN_H
# define BUILTIN_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <signal.h>
# include <fcntl.h>
# include <ctype.h>
# include <unistd.h>
# include "export_env_unset/environnement.h"
# include "../../libft/libft.h"
# include "../parser/tokenizer/tokenize.h"

int		ft_pwd(char **string);
int		ft_cd(char **path);
void	ft_exit(char **env);
int		ft_env(char **processed_argv, char ***envp);
int		ft_unset(char **var, char ***envp);
int		ft_export(char ***env, char **processed_argv);
int		ft_echo(char **split_args, int squote, int dquote, char **envp);

int		ft_str_env_cmp(char *s1, char *s2, size_t n);
char	**ft_duplicate_env(char *envp[]);
void	*ft_destroy_env(char **envp[], int idx);

int		print_cd_message(int code);
void	print_exit_message(int code);
int		print_export_message(int code);
void	*print_add_variable_msg(char *var);

#endif