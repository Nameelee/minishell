#include "tokenize.h"
#include <assert.h>
#include <errno.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>  

bool is_whitespace(char c) 
{ 
	return (c == ' ' || c == '\t' || c == '\n' 
		|| c == '\v' || c == '\f' || c == '\r'); 
}

bool is_operator_char(char c) 
{ 
	return (c == '|' || c == '<' || c == '>'); 
}

void free_single_token_node_content_and_node(t_token *node) 
{ 
    if (!node) 
		return;
    if (node->string) 
		free(node->string);
    node->string = NULL;
    free(node);
}

void free_token_list(t_token *list_head) { 
    t_token *current = list_head; t_token *next;
    while (current) {
        next = current->right; 
        free_single_token_node_content_and_node(current);
        current = next;
    }
}