#include _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>

void printUsage(char* exec);

int main(int argc, char **argv) {
    if(argc == 1) {
        printUsage(argv[0]);
        exit(1);
    }


}

void printUsage(char* exec) {
    fprintf(stderr, "Usage: %s [-s SIG] PID...", exec);
}