#include "directorios.h"

#define DISCO   argv[1]
#define RUTA    argv[2]
#define TEXTO   argv[3]
#define OFFSET  atoi(argv[4])

int main(int argc, char **argv){
    if(argc!=5){
        fprintf(stderr, "Argumentos esperados: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
        return EXIT_FAILURE;
    }
}