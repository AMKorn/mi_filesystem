#include "directorios.h"

#define DISCO       argv[1]
#define PERMISOS    atoi(argv[2])
#define RUTA        argv[3]

int main(int argc, char **argv){
    if(argc!=4) {
        fprintf(stderr, "Formato de comando esperado: ./mi_mkdir <disco> <permisos> </ruta>.\n");
        return EXIT_FAILURE;
    }
    unsigned char permisos = PERMISOS;
    if(permisos > 7){
        fprintf(stderr, "Modo inválido: <<%d>>. Debe ser un número del 0 al 7.\n", permisos);
        return EXIT_FAILURE;
    }
    return mi_creat(RUTA, permisos);
}