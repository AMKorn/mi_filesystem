#include "ficheros_basico.h"
#include <string.h>

int main(int argc, char **argv){
/*
argv[0]="mi_mkfs"
argv[1]=nombre_dispositivo
argv[2]=nbloques (puede sernos útil la función atoi() para obtener el valor numérico a partir del string)
*/
unsigned int nbloques = atoi(argv[2]);
unsigned char cadena [BLOCKSIZE];
memset(cadena,0,BLOCKSIZE);
int ninodos = nbloques/4
bmount(argv[1]);
for(int i = 0; i<nbloques; i++){
    bwrite(i, cadena);
}
int initSB(nbloques, ninodos);
int initMB();
int initAI();
bumount(argv[1]);
/*
En este caso, el buffer de memoria empleado puede ser un array de tipo unsigned char del tamaño de un bloque 
(lo inicializaremos a 0 con la función memset(), 
*/
return 0;
}