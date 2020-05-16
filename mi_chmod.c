#include "directorios.h"

#define DISCO       argv[1]
#define PERMISOS    atoi(argv[2])
#define RUTA        argv[3]

int main(int argc, char **argv){
    if(argc!=4) {
        fprintf(stderr, "Sintaxis: ./mi_chmod <nombre_dispositivo> <permisos> </ruta>.\n");
        return EXIT_FAILURE;
    }

    // Montamos el disco
    if(bmount(DISCO)==-1) return EXIT_FAILURE;

    unsigned char permisos = PERMISOS;
    if(permisos > 7){
        fprintf(stderr, "Modo inválido: <<%d>>. Debe ser un número del 0 al 7.\n", permisos);
        return EXIT_FAILURE;
    }
    if(mi_chmod(RUTA, permisos)==EXIT_FAILURE) return EXIT_FAILURE;

    // Desmontar el disco
    if(bumount(DISCO)==-1) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}