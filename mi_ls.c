/* Autores:
 * Andreas Manuel Korn
 * Sergio Vega García
 * Rubén López Babón
 */

#include "directorios.h"

#define DISCO argv[1]
#define CAMINO argv[2]
#define FORMATO argv[3]

// Definitions of color codes.
#define RESET_COLOR             "\x1b[0m"
#define ROJO_T                  "\x1b[91m"
#define VERDE_T                 "\x1b[92m"
#define AZUL_T                  "\x1b[94m"

//El parametro formato tiene dos posibilidades -s y -e (simple y expandido, respectivamente), si no se especifica se muestra el formato expandido
int main(int argc, char **argv){
    if(argc == 4){

        if(bmount(DISCO) < 0){
            return EXIT_FAILURE;
        }
        
        char buffer[TAMBUFFER];
        memset(buffer, 0, sizeof(buffer));
        int dir;
        
        if(strcmp(FORMATO, "-s")!=0 && strcmp(FORMATO, "-e")!=0){
            fprintf(stderr, "Formatos aceptados: -s y -e (simple y expandido, respectivamente)\n");
            return EXIT_FAILURE;
        }

        if(CAMINO[strlen(CAMINO)-1] == '/'){
            dir = mi_dir(CAMINO, buffer, 'd', FORMATO[1]);
        } else {
            dir = mi_dir(CAMINO, buffer, 'f', FORMATO[1]);
        }
        
        if(dir < 0){
            return EXIT_FAILURE;
        } else if (dir==0){
            printf("Total: %d\n", dir);
        } else {
            if(CAMINO[strlen(CAMINO)-1] == '/'){
                printf("Total: %d\n", dir);
            }
            printf("Tipo\tNombre\n");
            printf("-----------------\n");
            printf("%s\n", buffer);
        }
        bumount();

    } else {
        if(argc != 3){
            printf("Sintaxis: ./mi_ls <disco> </ruta> <formato>\n");
            return EXIT_FAILURE;
        }
    
        if(bmount(DISCO) < 0){
            return EXIT_FAILURE;
        }
        
        char buffer[TAMBUFFER];
        memset(buffer, 0, sizeof(buffer));
        int dir;
        
        if(CAMINO[strlen(CAMINO)-1] == '/'){
            dir = mi_dir(CAMINO, buffer, 'd', 'e');
        } else {
            dir = mi_dir(CAMINO, buffer, 'f', 'e');
        }
        
        if(dir < 0){
            return EXIT_FAILURE;
        } else if (dir==0){
            printf("Total: %d\n", dir);
        } else {
            if(CAMINO[strlen(CAMINO)-1] == '/'){
                printf("Total: %d\n", dir);
            }
            printf("Tipo\tPermisos\tmTime\t\t\tTamaño\t\tNombre\n");
            printf("--------------------------------------------------------------------------------\n");
            printf("%s\n", buffer);
        }
        bumount();
    }
    
    return EXIT_SUCCESS;
     
}