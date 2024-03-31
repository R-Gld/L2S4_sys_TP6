#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

int system(const char *command);

int main() {
    int status = system("sleep 30");
    if(WIFEXITED(status)) {
        printf("Terminé, code de sortie = %d\n", WEXITSTATUS(status));
    } else if(WIFSIGNALED(status)) {
        printf("Terminé par un signal = %d\n", WTERMSIG(status));
    }
}

/**
 * Exécute une commande dans un shell.
 * Retourne:
 *  - le code de retour du processus fils si tout s'est bien passé,
 *  - -1 si une erreur s'est produite.
 *  - 127 si le shell n'a pas pu être exécuté.
 * @param command commande à exécuter
 * @return le code de retour du processus fils si tout s'est bien passé, -1 si une erreur s'est produite, 127 si le shell n'a pas pu être exécuté.
 */
int system(const char *command) {
    if (command == NULL) return 1;

    pid_t pid;
    int status;

    // Masquage de SIGCHLD dans le processus parent
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    // Ignorer SIGINT et SIGQUIT dans le père
    struct sigaction sa_ignore, sa_oldint, sa_oldquit;
    sa_ignore.sa_handler = SIG_IGN;
    sigemptyset(&sa_ignore.sa_mask);
    sa_ignore.sa_flags = 0;
    sigaction(SIGINT, &sa_ignore, &sa_oldint);
    sigaction(SIGQUIT, &sa_ignore, &sa_oldquit);

    pid = fork();
    if (pid == -1) {
        // Restauration des anciens gestionnaires de signaux en cas d'échec de fork
        sigaction(SIGINT, &sa_oldint, NULL);
        sigaction(SIGQUIT, &sa_oldquit, NULL);
        sigprocmask(SIG_SETMASK, &oldmask, NULL);
        return -1;
    } else if (pid == 0) {
        // Restauration des signaux par défaut pour le child
        sigaction(SIGINT, &sa_oldint, NULL);
        sigaction(SIGQUIT, &sa_oldquit, NULL);
        sigprocmask(SIG_SETMASK, &oldmask, NULL);

        execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        _exit(127);
    }

    while (waitpid(pid, &status, 0) == -1) {
        if (errno != EINTR) {
            status = -1;
            break;
        }
    }

    // Restauration des anciens gestionnaires de signaux
    sigaction(SIGINT, &sa_oldint, NULL);
    sigaction(SIGQUIT, &sa_oldquit, NULL);
    sigprocmask(SIG_SETMASK, &oldmask, NULL);

    return status;
}