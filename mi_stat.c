#include "directorios.h"

#define DISCO   argv[1]
#define RUTA    argv[2]

int main(int argc, char **argv){
    if(argc!=3) {
        fprintf(stderr, "Formato de comando esperado: ./mi_stat <disco> </ruta>.\n");
        return EXIT_FAILURE;
    }
    struct STAT p_stat;
    return mi_stat(RUTA, &p_stat);
}