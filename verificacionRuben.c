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
    // ******* No sé si está bien -- He cogido la de la version INSP *****
    // Creamos el fichero informe.txt dentro del directorio de simulación
    char informe[128]; // -Flag No se porque 128
    strcpy(informe, DIR);
    strcat(informe, "informe.txt");

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
        info.pid= atoi(strchr(ent.nombre, '_'));
        printf("PID: %d\n", info.pid);
        info.nEscrituras = 0;

        // --COPIADO--
        char prueba[128];
		sprintf(prueba, "%s%s/%s", DIR, entrada.nombre, "prueba.txt");
        int offset = 0;
        // --FIN COPIADO--

        int cant_registros_buffer_escrituras = 256;
        struct REGISTRO buffer_escrituras[cant_registros_buffer_escrituras];


        /* Recorrer secuencialmente el fichero prueba.dat utilizando un buffer de N registros de escrituras: */
        while (mi_read(prueba,buffer_escrituras,offset, sizeof(buffer_escrituras)) > 0){ //mientras haya escrituras

            // -- COPIADO -- Supongo que tenemos que recorrer los registros que hemos leido -Flag
            //leer una escritura
            for (int j = 0; j < cant_registros_buffer_escrituras; j++) {
                //Comprobar si la escritura es valida
                if (buffer_escrituras[j].pid == info.pid){  
                    //Si es la primera escritura validada
                    if (info.nEscrituras == 0) {
                        //Inicializar los registros significativos con los datos de esa escritura (que ya será la de menor posición).
						info.PrimeraEscritura = buffer_escrituras[j];
						info.UltimaEscritura = buffer_escrituras[j];
						info.MenorPosicion = buffer_escrituras[j];
						info.MayorPosicion = buffer_escrituras[j];
					} else {
                        //Comparar fecha  y, para una misma fecha, el nº de escritura (para obtener la primera y la última escritura)  con los datos de nuestros registros y actualizar éstos si es necesario.
						if (info.PrimeraEscritura.nEscritura > buffer_escrituras[j].nEscritura)
						{
							info.PrimeraEscritura = buffer_escrituras[j];
						}
						else if (info.UltimaEscritura.nEscritura < buffer_escrituras[j].nEscritura)
						{
							info.UltimaEscritura = buffer_escrituras[j];
						}
						if (buffer_escrituras[j].nRegistro < info.MenorPosicion.nRegistro)
						{
							info.MenorPosicion = buffer_escrituras[j];
						}
						else if (buffer_escrituras[j].nRegistro > info.MayorPosicion.nRegistro)
						{
							info.MayorPosicion = buffer_escrituras[j];
						}
					}
                    //Incrementar el contador de escrituras validadas.
					info.nEscrituras++;
				}
			}

        }

        //Obtener la escritura de la última posición //será la delimitada por el EOF
        //Añadir la información del struct info al fichero informe.txt por el final
        char buffer[1024]; // -Flag Ni idea de pq 1024
		memset(buffer, 0, 1024);
		sprintf(buffer, "PID: %i\nNumero de escrituras: %i\n", info.pid, info.nEscrituras);

		sprintf(buffer + strlen(buffer), "%s %i %i %s",
		"Primera escritura: ",
		info.PrimeraEscritura.nEscritura,
		info.PrimeraEscritura.posicion,
		asctime(localtime(&info.PrimeraEscritura.fecha)));

		sprintf(buffer + strlen(buffer), "%s %i %i %s",
		"Ultima escritura: ",
		info.UltimaEscritura.nEscritura,
		info.UltimaEscritura.posicion,
		asctime(localtime(&info.UltimaEscritura.fecha)));

		sprintf(buffer + strlen(buffer), "%s %i %i %s",
		"Menor posicion: ",
		info.MenorPosicion.nEscritura,
		info.MenorPosicion.posicion,
		asctime(localtime(&info.MenorPosicion.fecha)));

		sprintf(buffer + strlen(buffer), "%s %i %i %s %s",
		"Mayor posicion: ",
		info.MayorPosicion.nEscritura,
		info.MayorPosicion.posicion,
		asctime(localtime(&info.MayorPosicion.fecha)),
		"\n");

		if (mi_write(camino_fichero_informe, buffer, nbytes_informe, strlen(buffer)) == -1) {
			printf("ERROR: main: Error escribiendo en el informe\n");
		}
		nbytes_informe += strlen(buffer);
        printf("%d escrituras validadas en %s\n", info.nEscrituras, camino_fichero_prueba);
    }
    // **FIN COPIADO** -Flag



    //Desmontamos el disco
    if(bumount(DISCO)==-1) {
        fprintf(stderr, "Error de desmontaje de disco.\n");
        return EXIT_FAILURE;
    }
}