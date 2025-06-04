/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouhadr <cbouhadr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 15:48:49 by cw3l              #+#    #+#             */
/*   Updated: 2025/04/09 13:25:03 by cbouhadr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"

void ft_binary_tree_traversal(t_token *node)
{
    if(!node)
        return;
    ft_binary_tree_traversal(node->left);

    printf("node %s\n", node->string);
    //execute_ast(node);
    ft_binary_tree_traversal(node->right);

}

void    ft_display_token_node(t_token *token_lst)
{
   if(token_lst)
   {
        printf("\nvoici la valeur du noeud: \n");
        printf("string : %s\n", token_lst->string);
        printf("Token : %s\n", ft_get_str_token(token_lst->token));
        printf("Précedence : %d\n", token_lst->precedence);
        printf("Associativité : %d\n", token_lst->asso);
        token_lst = token_lst->right;
        printf("\n");
   } 
}


void    ft_display_token_node_lst(t_token *token_lst)
{
    if(!token_lst)
        return ;
    while (token_lst)
    {
       ft_display_token_node(token_lst);
       token_lst = token_lst->right;
       printf("\n");
    }
}
//  double check by nami
int ft_get_precedence(int token)
{
    if(token == PIPE)
        return (3);
    if(token == REDIR_OPEN
        || token == REDIR_WRITE
        || token == REDIR_WRITE_A
        || token == DOUBLE_REDIR)
        return(2);
    return (1);
}


