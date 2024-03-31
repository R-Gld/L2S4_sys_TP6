#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>

void printUsageAndExit(char* exec);
bool isAValidInt(char *arg, long int *resp);
bool isAValidPid(pid_t pid);

/**
 * Default 15 - SIGTERM
 * @param argc nombre d'arguments
 * @param argv arguments
 * @return 0 si tout s'est bien passé, 1 sinon.
 */
int main(int argc, char **argv) {
    if(argc == 2 && strcmp("-l", argv[1]) == 0) {
        for (int sig = 1; sig < NSIG; ++sig) {
            char* sigName = strsignal(sig);
            if (sigName == NULL || strlen(sigName) == 0) continue; // Ce numéro de signal n'est pas utilisé
            printf("%d: %s\n", sig, sigName);
        }
        exit(EXIT_SUCCESS);
    }

    if (argc == 1 // Aucun arguments.
    || (argv[1][0] == '-' && argc <= 2) // aucun PID en paramètre.
    ) {
        printUsageAndExit(argv[0]);
    }
    int option_s = strcmp("-s", argv[1]) == 0;

    int sig = 15; // SIGTERM par défaut.
    if(option_s) {
        long int sig_i;
        if(isAValidInt(argv[2], &sig_i)) {
            sig = (int) sig_i;
        } else {
            fprintf(stderr, "Invalid signal: %s\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }

    for(int i = (option_s ? 3: 1); i < argc; i++) {
        long int pid_i;
        if (isAValidInt(argv[i], &pid_i)) {
            pid_t pid = (pid_t) pid_i;
            if(isAValidPid(pid)) {
                kill(pid, sig);
            } else {
                fprintf(stderr, "'%d' is not the pid of a running process.", pid);
            }
        } else {
            fprintf(stderr, "Invalid PID: %s\n", argv[i]);
        }
    }
}

/**
 * Vérifie si une chaîne de caractères est un entier valide.
 * @param arg la chaine de caractères à vérifier.
 * @param resp le pointeur où stocker la valeur de l'entier.
 * @return true si l'entier est valide, false sinon.
 */
bool isAValidInt(char *arg, long int *resp) {
    errno = 0;

    char *endptr;
    long int val = strtol(arg, &endptr, 10);

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
        || (errno != 0 && val == 0)) {
        perror("strtol");
        return false;
    }

    if (endptr == arg) {
        fprintf(stderr, "No digits were found in '%s'\n", arg);
        return false;
    }

    if (*endptr != '\0') {
        fprintf(stderr, "Further characters after number in %s: %s\n", arg, endptr);
        return false;
    }

    if (val <= 0) {
        fprintf(stderr, "Negtive number: %ld\n", val);
        return false;
    }

    *resp = val;
    return true;
}

/**
 * Vérifie si un PID est valide.
 * @param pid le PID à vérifier.
 * @return true si le PID est valide, false sinon.
 */
bool isAValidPid(pid_t pid) {
    return kill(pid, 0) == 0;
}

/**
 * Affiche l'utilisation du programme et termine le programme.
 * @param exec nom du programme.
 */
void printUsageAndExit(char* exec) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s [-s SIG] PID...\n", exec);
    fprintf(stderr, "  %s -l\n", exec);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -l    List all signal names.\n");
    fprintf(stderr, "  -s    Specify the signal to send.\n");
    exit(EXIT_FAILURE);
}
