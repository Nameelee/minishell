#include "tokenize.h"
#include <assert.h>
#include <errno.h> 
#include <string.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h> 

int apply_redir_open(t_redir *redir_item) 
{
    int fd_opened; int target_fd = STDIN_FILENO; int dup2_result;
    if (!redir_item || !redir_item->filename) return (-1);
    fd_opened = open(redir_item->filename, O_RDONLY);
    if (fd_opened == -1) { perror(redir_item->filename); return (-1); }
    dup2_result = dup2(fd_opened, target_fd);
    if (dup2_result == -1) { perror("dup2 STDIN_FILENO"); close(fd_opened); return (-1); }
    if (close(fd_opened) == -1) { perror(redir_item->filename); }
    return (0);
}

int apply_redir_write(t_redir *redir_item) 
{
    int fd_opened; int target_fd = STDOUT_FILENO; mode_t mode = 0644; int dup2_result;
    if (!redir_item || !redir_item->filename) return (-1);
    fd_opened = open(redir_item->filename, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (fd_opened == -1) { perror(redir_item->filename); return (-1); }
    dup2_result = dup2(fd_opened, target_fd);
    if (dup2_result == -1) { perror("dup2 STDOUT_FILENO"); close(fd_opened); return (-1); }
    if (close(fd_opened) == -1) { perror(redir_item->filename); }
    return (0);
}

int apply_redir_append(t_redir *redir_item) 
{
    int fd; int target_fd = STDOUT_FILENO; mode_t mode = 0644;
    if (!redir_item || !redir_item->filename) return (-1);
    fd = open(redir_item->filename, O_WRONLY | O_CREAT | O_APPEND, mode);
    if (fd == -1) { perror(redir_item->filename); return (-1); }
    if (dup2(fd, target_fd) == -1) { perror("dup2 STDOUT_FILENO (append)"); close(fd); return (-1); }
    if (close(fd) == -1) { perror(redir_item->filename); }
    return (0); 
}

int apply_redir_heredoc(t_redir *redir_item) 
{
    int heredoc_fd_to_dup; int target_fd = STDIN_FILENO;
    if (!redir_item || !redir_item->heredoc_node || redir_item->heredoc_node->heredoc_state != HD_PROCESSED_OK || redir_item->heredoc_node->heredoc_pipe_fd < 0) {
        fprintf(stderr, "minishell: internal error or heredoc not ready for apply_redir_heredoc\n"); return (-1);
    }
    heredoc_fd_to_dup = redir_item->heredoc_node->heredoc_pipe_fd;
    if (dup2(heredoc_fd_to_dup, target_fd) == -1) { perror("dup2 STDIN_FILENO (heredoc)"); return (-1); }
    return (0); 
}