#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

void formatDuration(long long nanoseconds, char *buffer);

int main(int argc, char **argv) {
    if(argc == 1) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct timespec starttime;
    int res = clock_gettime(CLOCK_MONOTONIC_RAW, &starttime);
    if(res == -1) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    int child_pid = fork();
    if(child_pid == 0) {
        execvp(argv[1], argv + 1);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        int status;
        wait(&status);
        struct timespec endtime;
        res = clock_gettime(CLOCK_MONOTONIC_RAW, &endtime);
        if(res == -1) {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }


        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            printf("%d exited, status code= %d\n", child_pid, exit_status);
        } else if (WIFSIGNALED(status)) {
            int signal_number = WTERMSIG(status);
            printf("%d killed by signal %d\n", child_pid, signal_number);
        }


        long long elapsed = (endtime.tv_sec - starttime.tv_sec) * 1000000000 + (endtime.tv_nsec - starttime.tv_nsec);
        char buffer[100];
        formatDuration(elapsed, buffer);
        printf("Duration of child process: %s\n", buffer);
    }
}

void formatDuration(long long nanoseconds, char *buffer) {
    const long long nanosPerSecond = 1000000000LL;
    const long long nanosPerMillisecond = 1000000LL;
    const int secondsPerMinute = 60;

    // Calcul des composants
    long long minutes = nanoseconds / (nanosPerSecond * secondsPerMinute);
    nanoseconds %= nanosPerSecond * secondsPerMinute;
    long long seconds = nanoseconds / nanosPerSecond;
    nanoseconds %= nanosPerSecond;
    long long milliseconds = nanoseconds / nanosPerMillisecond;
    nanoseconds %= nanosPerMillisecond;

    // Formatage de la chaîne
    sprintf(buffer, "%lldm %llds %lldms %lldns.", minutes, seconds, milliseconds, nanoseconds);
}