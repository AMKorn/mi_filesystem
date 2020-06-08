/* Autores:
 * Andreas Manuel Korn
 * Sergio Vega García
 * Rubén López Babón
 */

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

    // Montamos el disco
    if(bmount(DISCO)==-1) return EXIT_FAILURE;

    //Inicializamos las variables
    int tam = strlen(TEXTO);
    printf("longitud del texto: %d\n", tam);
    char buffer[tam];
    memset(buffer,0,tam);
    strcpy(buffer,TEXTO);

    int nbytes = mi_write(RUTA, buffer, OFFSET, tam);

    printf("Bytes escritos: %d\n", nbytes);

    // Desontamos el disco
    if(bumount(DISCO)==-1) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
