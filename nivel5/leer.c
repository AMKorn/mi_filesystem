
#include "ficheros.h"
#define TAM_LECTURA 1500
int main(int argc, char **argv){
    //Comprobamos la sintaxis
    if (argc != 3) {
		fprintf(stderr, "Argumentos esperados: <nombre_dispositivo> <inodo>\n"); 
        return EXIT_FAILURE;
    }
    //Montamos el disco
    if(bmount(argv[1])==-1) return EXIT_FAILURE;

    //Inicializamos variables
    struct inodo ino;
    if(leer_inodo(atoi(argv[2]), &ino)==-1){
        fprintf(stderr,"ERROR al leer el inodo");
        return EXIT_FAILURE;
    }
    unsigned char buff_texto[TAM_LECTURA];
    memset(buff_texto, 0, TAM_LECTURA);
    int offset = 0;
    int leidos = 0;
    int total = 0;

    //Comprobamos los permisos
    /*
    if((ino.permisos & 4) != 4) { 
        fprintf(stderr, "\nNo tiene permiso de lectura\n"); 
        fprintf(stderr,"\n\nTotal_leidos: %d" , total);
        fprintf(stderr,"\nTamEnBytesLog: %d \n" , ino.tamEnBytesLog);	
        return EXIT_SUCCESS; 
    }*/

    //Iniciamos la lectura
    leidos = mi_read_f(atoi(argv[2]), buff_texto,offset, TAM_LECTURA);
    if(leidos == -1) return -1;
    while (leidos>0){
        write(1,buff_texto,leidos);
        memset(buff_texto, 0, TAM_LECTURA);
        offset += TAM_LECTURA;
        total += leidos;
        
        leidos = mi_read_f(atoi(argv[2]), buff_texto,offset, TAM_LECTURA);
        if(leidos == -1) return -1;
    }

    //Imprimimos los resultados
    fprintf(stderr, "\n Total_leidos: %d", total);
    fprintf(stderr, "\n TamEnBytesLog: %d \n", ino.tamEnBytesLog);
    fprintf(stderr, "\n Total: %d", total);
    fprintf(stderr, "\n Offset: %d", offset);
    fprintf(stderr, "\n leidos: %d", leidos);

    //Desmontamos el disco
    if(bumount(argv[1])==-1) return EXIT_FAILURE;
}

/*
#include "ficheros.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char **argv){
	if (argc != 3) {
		perror("Numero de parametros incorrectos en leer\n\n./leer <nombre del disco> <inodo a leer> ('opcional' >>\"nombre de fichero a redireccionar\")");
		exit(-1);
	}

	if(bmount(argv[1])<0) printf("\nFallo al montar el disco\n");
	struct inodo inodo;
	leer_inodo(atoi(argv[2]), &inodo);
	char buffer[BLOCKSIZE];
	memset(buffer, 0, BLOCKSIZE);

	int offset = 0;
	int bytesLeidos=0;
	int bytesTotales=inodo.tamEnBytesLog;

	if((inodo.permisos & 4) != 4) { fprintf(stderr, "\nNo tiene permiso de lectura\n"); 
        fprintf(stderr,"\n\nTotal_leidos: %d" , offset);
        fprintf(stderr,"\nTamEnBytesLog: %d \n" , bytesTotales);	
        exit(-1); 
    } 

	while(offset < bytesTotales) {
		memset(buffer,0, BLOCKSIZE);
		bytesLeidos= mi_read_f(atoi(argv[2]), buffer, offset, BLOCKSIZE);
		write(1, buffer, bytesLeidos);
		offset += bytesLeidos;	
        if(bytesLeidos==-1)	return EXIT_FAILURE;
	}

	fprintf(stderr,"\n\nTotal_leidos: %d" , offset);
	fprintf(stderr,"\nTamEnBytesLog: %d \n" , bytesTotales);	
	
	bumount(argv[1]);
}
*/