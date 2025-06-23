#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

/*
    This is the handler for when a command is "running".
    This is the core of our test.
*/
void handle_signal_execution(int signal)
{
    (void)signal;
    // We will add a newline here to see if it works in isolation.
    write(STDOUT_FILENO, "\n", 1);
}

int main(void)
{
    pid_t pid;
    int status;

    // 1. Set up the signal handler, just like in your minishell's execution phase.
    struct sigaction sa;
    sa.sa_handler = handle_signal_execution;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // No SA_RESTART for this test
    sigaction(SIGINT, &sa, NULL);

    printf("Forking a child process that will sleep for 10 seconds.\n");
    printf("Press ctrl-c while it is sleeping.\n");

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        // Child process: just sleeps.
        // It will be terminated by ctrl-c.
        sleep(10);
        _exit(0);
    }
    else
    {
        // Parent process: waits for the child.
        waitpid(pid, &status, 0);

        // After waiting, check how the child exited.
        if (WIFSIGNALED(status))
        {
            if (WTERMSIG(status) == SIGINT)
            {
                printf("Parent: Child was terminated by ctrl-c (SIGINT).\n");
            }
        }
        else if (WIFEXITED(status))
        {
             printf("Parent: Child finished normally.\n");
        }
    }

    printf("Parent: End of program. New prompt should appear below.\n");
    return 0;
}