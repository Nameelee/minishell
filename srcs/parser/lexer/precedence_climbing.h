/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   precedence_climbing.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 19:58:58 by cw3l              #+#    #+#             */
/*   Updated: 2025/03/26 10:04:39 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PRECEDENCE_H
#define PRECEDENCE_H



#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../../libft/libft.h"
#include "../../../include/main.h"

void ft_climb_token(char *str, int precedence, int start, int end);
char *ft_join(char *str, char c);
int ft_get_prec(char c);

#endif