#include "directorios.h"

#define disco argv[1]
#define camino argv[2]

// Definitions of color codes.
#define RESET_COLOR             "\x1b[0m"
#define ROJO_T                  "\x1b[91m"
#define VERDE_T                 "\x1b[92m"
#define AZUL_T                  "\x1b[94m"

int main(int argc, char **argv){
    if(argc != 3){
        printf("Sintaxis: ./mi_ls <disco> </ruta> \n");
        return EXIT_FAILURE;
    }
   
    bmount(disco);
      
    char buffer[TAMBUFFER];
    memset(buffer, 0, sizeof(buffer));
    int dir;
    
    if(camino[strlen(camino)-1] == '/'){
        dir = mi_dir(camino, buffer, 'd');
    } else {
        dir = mi_dir(camino, buffer, 'f');
    }
    
    if(dir < 0){
        printf("Error: No existe el archivo o directorio\n");
        return EXIT_FAILURE;
    } else if (dir==0){
        printf("Total: %d\n", dir);
    } else {
        if(camino[strlen(camino)-1] == '/'){
            printf("Total: %d\n", dir);
        }
        printf("Tipo\tPermisos\tmTime\t\t\tTamaño\t\tNombre\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("%s\n", buffer);
    }
    bumount();
    return EXIT_SUCCESS;
     
}