#include "directorios.h"

#define DISCO   argv[1]
#define RUTA    argv[2]

int main(int argc, char **argv){
    if(argc!=3){
        fprintf(stderr, "Argumentos esperados: ./mi_rm <disco> </ruta>\n");
        return EXIT_FAILURE;
    }

    // Comprobación del directorio raíz.
    if(strcmp(RUTA, "/")==0){
        fprintf(stderr, "No se puede borrar el directorio raíz.\n");
        return EXIT_FAILURE;
    }

    //Comprobación de que se quiere eliminar un fichero
    if(RUTA[strlen(RUTA)-1] == '/'){
        fprintf(stderr, "mi_rm no puede eliminar directorios\n");
        return EXIT_FAILURE;
    }

    if(bmount(DISCO)==-1) return EXIT_FAILURE;

    if(mi_unlink(RUTA)!=EXIT_SUCCESS) return EXIT_FAILURE;

    if(bumount(DISCO)==-1) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}   