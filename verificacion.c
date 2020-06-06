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
    if(mi_stat(DIR, &stats) == EXIT_FAILURE) {
        fprintf(stderr, "Error al acceder a los stats del directorio de simulación.\n");
        return EXIT_FAILURE;
    }

    int numentradas = stats.tamEnBytesLog / sizeof(struct entrada);
    if(numentradas != NUMPROCESOS) {
        fprintf(stderr, "Error: El número de entradas no coincide con el número de procesos.\n");
        return EXIT_FAILURE;
    }

    printf("Número de entradas: %d\n", numentradas);

    // Creamos el fichero informe.txt dentro del directorio de simulación
    char camino_informe[128] = "";
    strcpy(camino_informe, DIR);
    strcat(camino_informe, "informe.txt");

    if(mi_creat(camino_informe, 7) == -1){
        fprintf(stderr, "Error creando el fichero informe.txt\n");
        return EXIT_FAILURE;
    }

    struct entrada ent;
    //Para cada entrada del directorio de simulación (es decir, para cada proceso) hacer    
    for(int i = 0; i < NUMPROCESOS; i++){  
        // Leer la entrada de directorio.
        if (mi_read(DIR, &ent, 0, sizeof(ent)) == -1) {
            printf("Error: En la lectura del directorio.\n");
            return EXIT_FAILURE;
	    } 

        struct INFORMACION info;
        //Extraer el PID a partir del nombre de la entrada y guardarlo en el registro info.
        info.pid= atoi(strchr(ent.nombre, '_'));
        info.nEscrituras = 0;
        char camino_prueba[128] = "";
        strcpy(camino_prueba, DIR);
        strcat(camino_prueba, ent.nombre);
        strcat(camino_prueba, "prueba.dat");
        
        int cant_registros_buffer_escrituras = 256;
        struct REGISTRO buffer_escrituras[cant_registros_buffer_escrituras];

        struct REGISTRO reg;
        /* Recorrer secuencialmente el fichero prueba.dat utilizando un buffer de N registros de escrituras: */
        // while(mi_read(camino_prueba, buffer_escrituras, 0, sizeof(buffer_escrituras)) > 0){ //mientras haya escrituras
        //     Leer una escritura => _escritura_
        //     if(_escritura_.pid == info.pid){
        //         if(info.nEscrituras=0){
        //             info.PrimeraEscritura = _escritura_.PrimeraEscritura;
        //             info.UltimaEscritura = _escritura_.UltimaEscritura;
        //             info.MenorPosicion = _escritura_.MenorPosicion;
        //             info.MayorPosicion = _escritura_.MayorPosicion;
        //         } else {
        //             // Comparar fecha  y, para una misma fecha, el nº de escritura (para obtener la primera y la última escritura)
        //             // con los datos de nuestros registros y actualizar éstos si es necesario.
        //             int dif = difftime(info.UltimaEscritura.fecha, _escritura_.UltimaEscritura.fecha)==0);
        //             if(dif==0){
        //                 dif = info.UltimaEscritura.nEscritura - _escritura_.UltimaEscritura.nEscritura;
        //             }
        //             if(dif<0) { // Nos quedamos el mayor, el mas reciente
        //                 // Si info - _escritura_ < 0, _escritura > info
        //                 info.PrimeraEscritura = _escritura_.PrimeraEscritura;
        //                 info.UltimaEscritura = _escritura_.UltimaEscritura;
        //                 info.MenorPosicion = _escritura_.MenorPosicion;
        //                 info.MayorPosicion = _escritura_.MayorPosicion;
        //             }
        //         }
        //         info.nEscrituras++;
        //     }
        // }
        // Obtener la escritura de la última posición //será la delimitada por el EOF
        // Añadir la información del struct info al fichero informe.txt por el final 
    }

    //Desmontamos el disco
    if(bumount(DISCO)==-1) {
        fprintf(stderr, "Error al desmontar el disco.\n");
        return EXIT_FAILURE;
    }
}