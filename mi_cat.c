#include "directorios.h"

#define DISCO       argv[1]
#define RUTA        argv[2]
#define TAM_BUFFER  1500

int main (int argc, char **argv) {
    //Primero: Comprobamos la sintaxis
    if(argc!=3) {
        fprintf(stderr, "Sintaxis: ./mi_mkdir <nombre_dispositivo>  </ruta_fichero>.\n");
        return EXIT_FAILURE;
    }

    //Luego Comprobamos si la ruta corresponde a un fichero
	//char *path = RUTA;
    if(RUTA[strlen(RUTA)-1] == '/'){
        printf("ERROR: No es un fichero.\n");
        return -1;
    }

    //Montamos el disco
    if(bmount(DISCO)==-1) return EXIT_FAILURE;

    //Inicializamos variables
    unsigned char buff_texto[TAM_BUFFER];
    memset(buff_texto, 0, TAM_BUFFER);
    int offset = 0;
    int leidos = 0;
    int total = 0;


    //Iniciamos la lectura
    leidos = mi_read(RUTA, buff_texto,offset, TAM_BUFFER);
    //if(leidos == -1) return -1;
    while (leidos>0){
        write(1,buff_texto,leidos);
        total += leidos;
        offset += TAM_BUFFER;
        memset(buff_texto, 0, TAM_BUFFER);
        leidos = mi_read(RUTA, buff_texto,offset, TAM_BUFFER);
        
        //if(leidos == -1) return -1;
    }
    //Imprimimos los resultados
    fprintf(stderr, "\nTotal_leidos: %d", total);


    //Desmontamos el disco
    if(bumount(DISCO)==-1) return EXIT_FAILURE;    

    EXIT_SUCCESS;
}