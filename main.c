#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <time.h>
#else
#include <sys/sysinfo.h>
#endif

#define MAX_ARGS 64

void print_help() {
    printf("Built-in commands:\n");
    printf("  help       - Show this help message\n");
    printf("  greet NAME - Print a greeting to NAME\n");
    printf("  randnum    - Print a random number (0-99)\n");
    printf("  sysinfo    - Show basic system info\n");
    printf("  exit       - Exit the shell\n");
}

void greet_user(char *name) {
    if (name)
        printf("Hello, %s! Welcome to mini shell.\n", name);
    else
        printf("Usage: greet <name>\n");
}

void print_randnum() {
    printf("Random number: %d\n", rand() % 100);
}

void print_sysinfo() {
    char hostname[256];
    char *user = getenv("USER");
    gethostname(hostname, sizeof(hostname));
    printf("User: %s\n", user ? user : "Unknown");
    printf("Hostname: %s\n", hostname);

#if defined(_WIN32)
    ULONGLONG uptime_ms = GetTickCount64();
    printf("Uptime: %llu seconds\n", uptime_ms / 1000);
#elif defined(__APPLE__)
    struct timeval boottime;
    size_t size = sizeof(boottime);
    int mib[2] = { CTL_KERN, KERN_BOOTTIME };
    time_t now;
    time(&now);
    if (sysctl(mib, 2, &boottime, &size, NULL, 0) == 0) {
        time_t uptime = now - boottime.tv_sec;
        printf("Uptime: %ld seconds\n", uptime);
    } else {
        perror("sysctl failed");
    }
#else
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        printf("Uptime: %ld seconds\n", info.uptime);
    } else {
        perror("sysinfo failed");
    }
#endif
}

int main() {
    char line[1024];
    srand(time(NULL)); // Seed RNG

    while (1) {
        printf("mini> ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break; // EOF
        }

        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;

        // Tokenize input
        char *args[MAX_ARGS];
        int i = 0;
        char *token = strtok(line, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Built-in commands
        if (strcmp(args[0], "exit") == 0) break;
        else if (strcmp(args[0], "help") == 0) print_help();
        else if (strcmp(args[0], "greet") == 0) greet_user(args[1]);
        else if (strcmp(args[0], "randnum") == 0) print_randnum();
        else if (strcmp(args[0], "sysinfo") == 0) print_sysinfo();
        else {
            // External commands
            pid_t pid = fork();
            if (pid == 0) {
                execvp(args[0], args);
                perror("exec failed");
                exit(1);
            } else if (pid > 0) {
                int status;
                waitpid(pid, &status, 0);
            } else {
                perror("fork failed");
            }
        }
    }

    return 0;
}
