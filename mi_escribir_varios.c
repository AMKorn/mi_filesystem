/* Autores:
 * Andreas Manuel Korn
 * Sergio Vega García
 * Rubén López Babón
 */

#include "directorios.h"

int main(int argc, char **argv){

    //Comprobamos sintaxis
    if(argc!=5){
        fprintf(stderr, "Sintaxis: ./mi_escribir_varios <nombre_dispositivo> </ruta_fichero> <texto> <offset>\n");
        exit(-1);    
    }

    //Montamos el dispositivo
    if(bmount(argv[1])<0) return -1;
    //Obtenemos el texto y su longitud
    char *buffer_texto = argv[3];
    int longitud=strlen(buffer_texto);

    //Obtenemos la ruta y comprobamos que no se refiera a un directorio
    if(argv[2][strlen(argv[2])-1]=='/'){
        fprintf(stderr, "Error: la ruta se corresponde a un directorio\n");
        exit(-1);
    }
    char *camino = argv[2];

    //Obtenemos el offset
    unsigned int offset = atoi(argv[4]);

    //Escribimos el texto
    int escritos=0;
    int varios=10;
    fprintf(stderr, "Longitud texto: %d\n", longitud);
    for (int i=0; i<varios; i++){
        //Escribimos varias veces el texto despalazado 1 bloque
        escritos += mi_write(camino, buffer_texto, offset+BLOCKSIZE*i, longitud);
    }

    fprintf(stderr, "Bytes escritos: %d\n", escritos);
    bumount();
}
