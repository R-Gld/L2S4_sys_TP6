#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    FILE* file = fopen("myself.pid", "w");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    fprintf(file, "%d\n", getpid());

    if(fclose(file)) {
        perror("fclose");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}