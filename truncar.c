#include "ficheros.h"

#define DISCO   argv[1]
#define NINODO  atoi(argv[2])
#define NBYTES  atoi(argv[3])

int main(int argc, char **argv){
    if(argc!=4){
        fprintf(stderr, "Formato deseado: %s <nombre_dispositivo> <ninodo> <nbytes>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    // Montamos el disco
    if(bmount(DISCO)==-1) return EXIT_FAILURE;

    // Liberar inodo o truncar
    if(NBYTES==0){
        liberar_inodo(NINODO);
    } else {
        mi_truncar_f(NINODO, NBYTES);
    }

    // Desmontar el disco
    if(bumount(DISCO)==-1) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}