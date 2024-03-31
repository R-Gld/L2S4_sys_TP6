#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void handSIGINT(int sig);
void handSIGALRM(int sig);

volatile int kb_max = -1;

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s N", argv[0]);
        exit(EXIT_FAILURE);
    }

    kb_max = atoi(argv[1]);
    if(kb_max == 0) {
        fprintf(stderr, "N should be an int.");
        return EXIT_FAILURE;
    }

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_handler = handSIGINT;
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);

    struct sigaction alarm_act;
    sigemptyset(&alarm_act.sa_mask);
    alarm_act.sa_handler = handSIGALRM;
    alarm_act.sa_flags = 0;
    sigaction(SIGALRM, &alarm_act, NULL);

    alarm(1);
    while(1) {
        pause();
    }
}

/**
 * Handler pour le signal SIGALRM
 * @param sig signal (non utilisé ici)
 */
void handSIGALRM(int sig) {
    alarm(1);
    static int incr = 1;
    char tab[16];
    sprintf(tab, "%d\n", incr++);
    write(1, tab, strlen(tab));
}

/**
 * Handler pour le signal SIGINT
 * @param sig signal (non utilisé ici)
 */
void handSIGINT(int sig) {
    static int kb_nb = 0;
    if(kb_nb >= kb_max) {
        write(1, " KBOOM!", 7);
        _exit(1);
    } else {
        write(1, " BOOM!\n", 7);
        kb_nb++;
        alarm(5);
    }
}