#include "directorios.h"

int main(int argc, char **argv){
    if(argc != 3){
        printf("Sintaxis: ./mi_ls <disco> </ruta> \n");
        return -1;
    }
   
    bmount(argv[1]);
      
    char buffer[50*BLOCKSIZE];
    memset(buffer, 0, sizeof(buffer));
    int dir = mi_dir(argv[2], buffer);
    if(dir < 0){
        printf("Error al listar directorio\n");
        return -1;
    } else if (dir == 0){
        printf("No hay ninguna entrada a imprimir\n");
    } else {
        printf("Total de entradas: %d\n\n", dir);
    printf("Tipo    Permisos    mTime                Tamaño         Nombre\n");
    printf("---------------------------------------------------------------\n");
    printf("%s\n", buffer);
    }
    bumount();
    return 0;
     
}