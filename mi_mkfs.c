/* Autores:
 * Andreas Manuel Korn
 * Sergio Vega García
 * Rubén López Babón
 */

#include "directorios.h"

#define DISCO argv[1]
#define NUM_BLOQUES atoi(argv[2])

int main(int argc, char **argv){
    if(argc!=3){                            // Si no hay el número adecuado de argumentos
        fprintf(stderr, "Argumentos esperados: <nombre_dispositivo> <nbloques>\n"); 
                                            // se imprime un error que muestra el formato adecuado
        return EXIT_FAILURE;                // y finalizamos el programa.
    }

    if(!bmount(DISCO)){                     // Empezamos montando el disco.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));  // Si hay un error, lo imprimimos
        return EXIT_FAILURE;                // y salimos del programa.
    }
    unsigned char buf[BLOCKSIZE];           // Preparamos el espacio para la información
    memset(buf, 0, BLOCKSIZE);              // y lo inicializamos a ceros.
    for(int i = 0; i<NUM_BLOQUES; i++){
        bwrite(i, buf);
    }
    initSB(NUM_BLOQUES, NUM_BLOQUES/4);
    initMB();
    initAI();
    reservar_inodo('d', 7);
    bumount();                              // finalmente cerramos el disco
    return EXIT_SUCCESS;                    // y salimos del programa
}
