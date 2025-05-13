/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 11:02:58 by cbouhadr          #+#    #+#             */
/*   Updated: 2025/05/09 16:25:42 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTIN_H
# define BUILTIN_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <signal.h>
# include <fcntl.h>
# include <unistd.h>
# include "export_env_unset/environnement.h"
# include "../../libft/libft.h"
# include "../parser/tokenizer/tokenize.h"

int	    ft_pwd(char **string);
int	    ft_process_echo(char **split_args, char **envp, int n, int fd);
int		ft_cd(char **path);
void	ft_exit(char **split, char **env);
int		ft_env(char **processed_argv, char ***envp);
int		ft_export(char ***env, char **processed_argv);
int	    ft_unset(char **var, char ***envp);
int	    ft_echo(char **split_args, int fd, char **envp);

char    **ft_duplicate_env(char *envp[]);
int	    ft_str_env_cmp(char *s1, char *s2, size_t n);

#endif