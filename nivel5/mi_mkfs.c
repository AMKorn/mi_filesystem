#include "ficheros_basico.h"
/*
***¿Version copiada o era la vuestra?****

#define DISCO argv[1]
#define NUM_BLOQUES atoi(argv[2])

int main(int argc, char **argv){
    if(!argv[1] || !argv[2] || argv[3]){    // If not enough arguments or too many,
        fprintf(stderr, "Argumentos esperados: <nombre_dispositivo> <nbloques>\n"); 
                                            // an error is printed asking for the correct command format
        return EXIT_FAILURE;                // and we finish the program
    }

    if(!bmount(DISCO)){             // We start by mounting the disk.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));  // if error, we print it
        return EXIT_FAILURE;        // and exit the program.
    }
    unsigned char buf[BLOCKSIZE];   // We prepare space for the information
    memset(buf, 0, BLOCKSIZE);      // and start it full of zeros
    initSB(NUM_BLOQUES, NUM_BLOQUES/4);
    initMB();
    initAI();
    bumount();  // lastly we close the disk
    return EXIT_SUCCESS;    // and exit the program
}
*/
int main(int argc, char **argv){
    //mejorar
    /*
    argv[0]="mi_mkfs"
    argv[1]=nombre_dispositivo
    argv[2]=nbloques (puede sernos útil la función atoi() para obtener el valor numérico a partir del string)
    */

    if (argc != 3) {
		fprintf(stderr, "Argumentos esperados: <nombre_dispositivo> <nbloques>\n"); 
        return EXIT_FAILURE;
    }

    unsigned int nbloques = atoi(argv[2]);
    unsigned char cadena [BLOCKSIZE];
    memset(cadena,0,BLOCKSIZE);
    int ninodos = nbloques/4;
    bmount(argv[1]);
    for(int i = 0; i<nbloques; i++){
        bwrite(i, cadena);
    }
    //Inicializamos el superbloque, el mapa de bits y el array de inodos
    initSB(nbloques, ninodos);
    initMB();
    initAI();

    //reservamos un nuevo inodo para testear
    reservar_inodo('d', 7);

    bumount(argv[1]);
    /*
    En este caso, el buffer de memoria empleado puede ser un array de tipo unsigned char del tamaño de un bloque 
    (lo inicializaremos a 0 con la función memset(), 

    unsigned int nbloques = atoi(argv[2]);
    unsigned char cadena [BLOCKSIZE];
    memset(cadena,0,BLOCKSIZE);
    int ninodos = nbloques/4;
    if (bmount(argv[1])=-1)return EXIT_FAILURE;
    for(int i = 0; i<nbloques; i++){
        bwrite(i, cadena);
    }
    //Inicializamos el superbloque, el mapa de bits y el array de inodos
    initSB(nbloques, ninodos);
    initMB();
    initAI();

    //reservamos un nuevo inodo para testear
    reservar_inodo('d', 7);

    if (bumount(argv[1])=-1)return EXIT_FAILURE;



    */
    return EXIT_SUCCESS;
}