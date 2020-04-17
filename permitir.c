#include "ficheros.h"

int main(int argc, char **argv){
    //Comprobamos la sintaxis
    if (argc != 4) {
		fprintf(stderr, "Argumentos esperados: <nombre_dispositivo> <ninodo> <permisos>\n"); 
        return EXIT_FAILURE;
    }
    //Montamos el disco
    if(bmount(argv[1])==-1) return EXIT_FAILURE;
    //Iniciamos el proceso de permitr
	if(mi_chmod_f(atoi(argv[2]), atoi(argv[3])) == -1){
		return EXIT_FAILURE;
	}
    //Desmontamos el disco
    if(bumount(argv[1])==-1) return EXIT_FAILURE;
}