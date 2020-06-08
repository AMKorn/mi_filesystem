/* Autores:
 * Andreas Manuel Korn
 * Sergio Vega García
 * Rubén López Babón
 */

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
    struct STAT stat;
    unsigned int ninodo = atoi(argv[2]);

    unsigned char buff_texto[TAM_LECTURA];
    memset(buff_texto, 0, TAM_LECTURA);
    int offset = 0;
    int leidos = 0;
    int total = 0;

    //Iniciamos la lectura
    leidos = mi_read_f(ninodo, buff_texto,offset, TAM_LECTURA);
    //if(leidos == -1) return -1;
    while (leidos>0){
        write(1,buff_texto,leidos);
        total += leidos;
        offset += TAM_LECTURA;
        memset(buff_texto, 0, TAM_LECTURA);
        leidos = mi_read_f(ninodo, buff_texto,offset, TAM_LECTURA);
    }
    mi_stat_f(ninodo, &stat);
    //Imprimimos los resultados
    fprintf(stderr, "\nTotal_leidos: %d\n", total);

    //Desmontamos el disco
    if(bumount(argv[1])==-1) return EXIT_FAILURE;
}
