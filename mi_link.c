/* Autores:
 * Andreas Manuel Korn
 * Sergio Vega García
 * Rubén López Babón
 */

#include "directorios.h"

#define DISCO   argv[1]
#define RUT_OG  argv[2]
#define RUT_LK  argv[3]

int main(int argc, char **argv){
    if(argc != 4){
        fprintf(stderr, "Argumentos esperados: ./mi_link disco /ruta_fichero_original /ruta_enlace\n");
        return EXIT_FAILURE;
    }
    
    // Montamos el disco
    if(bmount(DISCO)==-1) return EXIT_FAILURE;

    // Comprobamos si las rutas son ficheros comprobando si terminan con /
    int tam1 = strlen(RUT_OG);
    if(RUT_OG[tam1-1]=='/') {
        fprintf(stderr, "Error: La ruta original es un directorio. Ambas rutas deben ser un fichero.\n");
        return EXIT_FAILURE;
    }
    
    int tam2 = strlen(RUT_LK);
    if(RUT_LK[tam2-1]=='/'){
        fprintf(stderr, "Error: La ruta de enlace es un directorio. Ambas rutas deben ser un fichero.\n");
        return EXIT_FAILURE;
    }

    if(mi_link(RUT_OG, RUT_LK)==EXIT_FAILURE) return EXIT_FAILURE;

    // Desmontamos el disco
    if(bumount(DISCO)==-1) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
