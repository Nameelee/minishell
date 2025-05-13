/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_skip_space.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ast <ast@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/12 10:51:46 by cw3l              #+#    #+#             */
/*   Updated: 2025/05/05 08:37:19 by ast              ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "libft.h"
#include <assert.h>

static int ft_isquote(char c)
{
    if(c == 39 || c == 34)
        return(1);
    return(0);
}

char *ft_skip_space_and_quote(char *str)
{
    int i;
    int j;
    int len;
    char *new_str;

    if(!str)
        return(NULL);
    i = 0;
    j = 0;
    while(str[i] == 32)
        i++;
    len = ft_strlen(&str[i]);
    if(str[i] == '"')
        i++;
    new_str = malloc(sizeof(char) * (len + 1));
    if(!new_str)
        return(NULL);
    while (str[i] && i < len && str[i] != 32)
    {
        if((ft_isquote(str[i])  && i == 0) || (ft_isquote(str[i]) && i == len - 1))
            i++;
        else
            new_str[j++] = str[i++];
    }
    new_str[j] = '\0';
    return(new_str);
}

