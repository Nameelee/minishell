/* ************************************************************************** */
/*                                                                            */
/*                                                         ::::::::           */
/*   main.h                                              :+:    :+:           */
/*                                                      +:+                   */
/*   By: jelee <marvin@42.fr>                          +#+                    */
/*                                                    +#+                     */
/*   Created: 2025/07/02 15:40:09 by jelee          #+#    #+#                */
/*   Updated: 2025/07/02 15:40:25 by jelee          ########   odam.nl        */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

# include <sys/types.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <sys/resource.h>
# include <sys/wait.h>
# include <unistd.h>
# include <dirent.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <dirent.h>
# include <stdio.h>
# include <assert.h>
# include <signal.h>
# include <fcntl.h>
# include <stdbool.h>

# include "../srcs/exec/exec.h"
# include "../srcs/parser/parser.h"
# include "../libft/libft.h"
# include "../srcs/builtin/builtin.h"
# include "../srcs/parser/tokenizer/tokenize.h"

# include <string.h>

char	**ft_duplicate_env(char *envp[]);
void	free_duplicated_env(char **dup_envp);
void	ft_fprintf(const char *prefix, const char *arg, const char *suffix);

#endif 
