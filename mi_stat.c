#include "directorios.h"

#define DISCO   argv[1]
#define RUTA    argv[2]

int main(int argc, char **argv){
    if(argc!=3) {
        fprintf(stderr, "Formato de comando esperado: ./mi_stat <disco> </ruta>.\n");
        return EXIT_FAILURE;
    }

    // Montamos el disco
    if(bmount(DISCO)==-1) return EXIT_FAILURE;

    struct STAT p_stat;
    if(mi_stat(RUTA, &p_stat)==EXIT_FAILURE) return EXIT_FAILURE;

    // Desmontar el disco
    if(bumount(DISCO)==-1) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}