
#include "tokenize.h"
#include <assert.h>
#include <errno.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>  

// --- Redirection List Handling Functions ---
void add_redirection_to_list(t_redir **list, int type, const char *filename_or_delimiter, t_token *heredoc_node) {
    t_redir *new_redir = (t_redir *)malloc(sizeof(t_redir));
    if (!new_redir) { perror("minishell: malloc in add_redirection_to_list"); return; }
    new_redir->type = type; new_redir->filename = NULL;
    if (filename_or_delimiter) {
        new_redir->filename = ft_strdup(filename_or_delimiter);
        if (!new_redir->filename && filename_or_delimiter[0] != '\0') { perror("minishell: ft_strdup in add_redirection_to_list"); free(new_redir); return; }
    }
    new_redir->heredoc_node = heredoc_node; new_redir->next = *list; *list = new_redir;
}
int apply_redirections(t_redir *list) {
    t_redir *current = list; int status = 0;
    while (current != NULL) {
        status = 0; 
        switch (current->type) {
            case REDIR_OPEN: status = apply_redir_open(current); break;
            case REDIR_WRITE: status = apply_redir_write(current); break;
            case REDIR_WRITE_A: status = apply_redir_append(current); break;
            case HEREDOC: status = apply_redir_heredoc(current); break;
            default: fprintf(stderr, "minishell: unknown redirection type %d\n", current->type); status = -1; break;
        }
        if (status == -1) return (-1); 
        current = current->next;
    }
    return (0);
}
void free_redir_list(t_redir *list) {
    t_redir *current = list; t_redir *next;
    while (current) {
        next = current->next;
        if(current->filename) free(current->filename);
        free(current); current = next;
    }
} 
