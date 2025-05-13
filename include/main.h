/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ast <ast@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 22:15:28 by cw3l              #+#    #+#             */
/*   Updated: 2025/05/04 16:22:35 by ast              ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef MAIN_H
#define MAIN_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include <fcntl.h> // O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC, O_APPEND
#include <stdbool.h> // For bool type

#include "../srcs/exec/exec.h"
#include "../srcs/parser/parser.h"
#include "../libft/libft.h"
#include "../srcs/builtin/builtin.h"
#include "../srcs/parser/tokenizer/tokenize.h"

#include <string.h>

#define DEBUGG printf("DEBUGG\n");
#define PRINT_INT(msg) printf("\x1b[31m" "DEBBUG  INT " "%d\n", msg);
#define PSTR(msg) printf("\x1b[31m" "DEBBUG  str >>> " "%s\n", msg);
#define PPTR(msg) printf("\x1b[31m" "DEBBUG  de pointeur: >>> "  "%p\n" "\x1b[0m", msg);
#define DEBUGG printf("DEBUGG\n");
#define TEST_START printf("\033[0;33m" "Initiating function test: %s\n" "\x1b[0m", __func__);
#define TEST_SUCCES printf("\033[0;32m" "Function: %s executed successfully.\n" "\x1b[0m", __func__);


extern int g_exit_status;


char **ft_duplicate_env(char *envp[]);
void free_duplicated_env(char **dup_envp);


#endif 