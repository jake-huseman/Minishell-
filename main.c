#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64

int main() {
    char line[1024];

    while (1) {
        printf("mini> ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break; // EOF (e.g., Ctrl+D)
        }

        // Remove newline
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "exit") == 0) break;
        if (strlen(line) == 0) continue;

        // Split input into args
        char *args[MAX_ARGS];
        int i = 0;
        char *token = strtok(line, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // null-terminate the array

        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            execvp(args[0], args);
            perror("exec failed");
            exit(1);
        } else if (pid > 0) {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        } else {
            perror("fork failed");
        }
    }

    return 0;
}
