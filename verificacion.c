#include "verificacion.h"

#define DISCO   argv[1]
#define DIR     argv[2]

int main(int argc, char **argv){
    // Comprobar sintaxis
    if(argc != 3){
        fprintf(stderr, "Sintaxis incorrecta: ./verificacion <disco> <directorio_simulacion>\n");
        return EXIT_FAILURE;
    }

    //Montamos el disco
    if(bmount(DISCO)==-1) {
        fprintf(stderr, "Error de montaje de disco.\n");
        return EXIT_FAILURE;
    }

    // Accedemos a los stats del directorio de simulacion 
    struct STAT stats;
    mi_stat(DIR, &stats);

    if(stats.nlinks != NUMPROCESOS) {
        fprintf(stderr, "Error: El número de entradas no coincide con el número de procesos.\n");
        return EXIT_FAILURE;
    }

    // *******************************************************************
    // ******* No sé si está bien ****************************************
    // Creamos el fichero informe.txt dentro del directorio de simulación
    char *camino = "";
    strcpy(camino, DIR);
    strcat(camino, "informe.txt");

    mi_creat(camino, 6);
    // ********************************************************************

    //char buff[sizeof(struct entrada)];
    
	//struct entrada entradas[NUMPROCESOS];
    struct entrada ent;
    //Para cada entrada del directorio de simulación (es decir, para cada proceso) hacer    
    for(int i = 0; i < NUMPROCESOS; i++){  
        // Leer la entrada de directorio.
        if (mi_read(DIR, ent, 0, sizeof(entrada)) == -1) {
		printf("Error: En la lectura del directorio\n");
		return -1;
	    } 

        struct INFORMACION info;
        //Extraer el PID a partir del nombre de la entrada y guardarlo en el registro info.
        info.pid= atoi(strchr(entradas[i].nombre, '_'));

        
        int cant_registros_buffer_escrituras = 256;
        struct REGISTRO buffer_escrituras[cant_registros_buffer_escrituras];
        char prueba[sizeof(buffer_escrituras)];

        struct REGISTRO reg;
        /* Recorrer secuencialmente el fichero prueba.dat utilizando un buffer de N registros de escrituras: */
        while(mi_read(prueba, buffer_escrituras, 0, sizeof(buffer_escrituras)) > 0){ //mientras haya escrituras
            
        }
    }



    //Desmontamos el disco
    if(bumount(DISCO)==-1) {
        fprintf(stderr, "Error de desmontaje de disco.\n");
        return EXIT_FAILURE;
    }
}