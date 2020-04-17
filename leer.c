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
    if(leer_inodo(atoi(argv[2]), &ino)==-1)return EXIT_FAILURE;
    unsigned char buff_texto[TAM_LECTURA];
    memset(buff_texto, 0, TAM_LECTURA);
    int offset = 0;
    int leidos = 0;
    int total = 0;

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

    //Desmontamos el disco
    if(bumount(argv[1])==-1) return EXIT_FAILURE;
}