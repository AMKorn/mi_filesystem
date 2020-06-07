#include "verificacion.h"

#define NOMBRE_FICHERO_PRUEBA "prueba.dat"

#define DISCO 	argv[1]
#define DIR 	argv[2]

int main (int argc, char **argv) {
    // Comprobamos los parametros
	if (argc != 3) {
		fprintf(stderr, "Sintaxis esperada: ./verificacion <disco> </directorio> \n");
		return EXIT_FAILURE;
	}
	
	// Montamos el dispositivo virtual
	if (bmount(DISCO) == -1) {
		fprintf(stderr, "Error al montar el sistema de archivos\n");
		return EXIT_FAILURE;
	}
	
	// Obtenemos la metainformacion del directorio de simulacion
	struct STAT stat;
 	if (mi_stat(DIR, &stat) == -1) {
		fprintf(stderr, "Error al obtener la metainformacion del directorio de simulacion\n");
		return EXIT_FAILURE;
	}

	printf("\ndir_sim: %s\n", DIR);
    
    int numentradas = stat.tamEnBytesLog / sizeof(struct entrada);
    if (numentradas != NUMPROCESOS){
		fprintf(stderr, "Error: el número de entradas no coincide con el número de procesos.\n");
		return EXIT_FAILURE;
	}
	
    printf("numentradas: %d NUMPROCESOS: %d\n", numentradas, NUMPROCESOS);

    // Inicializamos el camino del fichero de informe
	char camino_fichero_informe[128];
	sprintf(camino_fichero_informe, "%s%s", DIR, "informe.txt");
	// Creamos el fichero de informe
	if (mi_creat(camino_fichero_informe, 7) == -1) {
		fprintf(stderr, "Error al crear el fichero informe\n");
		return EXIT_FAILURE;
	}
	
    int nbytes_informe = 0;
	struct entrada entradas[NUMPROCESOS*sizeof(struct entrada)];
	
	for (int i = 1; i <= NUMPROCESOS; i++) {
	    struct INFORMACION info;
        if (mi_read(DIR, entradas, 0, sizeof(entradas)) == -1) {
		    fprintf(stderr, "Error al leer entradas del directorio de simulacion\n");
			return EXIT_FAILURE;
	    }
	    
        //Inicializamos la informacion
        info.pid = atoi(strchr(entradas[i].nombre, '_') + 1);
		info.nEscrituras = 0;
        info.PrimeraEscritura.fecha = time(NULL);
        info.PrimeraEscritura.nEscritura = NUMESCRITURAS;
        info.PrimeraEscritura.nRegistro = 0;
        info.UltimaEscritura.fecha = time(NULL);
        info.UltimaEscritura.nEscritura = 0;
        info.UltimaEscritura.nRegistro = 0;
        info.MenorPosicion.fecha = time(NULL);
        info.MenorPosicion.nEscritura = 0;
        info.MenorPosicion.nRegistro = REGMAX;
        info.MayorPosicion.fecha = time(NULL);
        info.MayorPosicion.nEscritura = 0;
        info.MayorPosicion.nRegistro = 0;

		char camino_fichero_prueba[128];
		sprintf(camino_fichero_prueba, "%s%s/%s", DIR, entradas[i].nombre, NOMBRE_FICHERO_PRUEBA);
		
		int offset = 0;
		
		int cant_registros_buffer_escrituras = 256;
        struct REGISTRO buffer_escrituras [cant_registros_buffer_escrituras];
		memset(buffer_escrituras, 0, sizeof(buffer_escrituras));

        int lectura = mi_read(camino_fichero_prueba,buffer_escrituras,offset, sizeof(buffer_escrituras));
        while (info.nEscrituras < NUMESCRITURAS && lectura > 0) { // Mientras haya escrituras en prueba.dat
            for (int j = 0; j < cant_registros_buffer_escrituras; j++) {
                if (buffer_escrituras[j].pid == info.pid) { // Que una escritura sea válida significa que el pid de la escritura es el mismo que el del proceso.
					// Comparamos la escritura con el dato de nuestro registro y actualizamos éstos si es necesarios
					if (buffer_escrituras[j].nEscritura < info.PrimeraEscritura.nEscritura) {
                        info.PrimeraEscritura.fecha = buffer_escrituras[j].fecha;
                        info.PrimeraEscritura.nEscritura = buffer_escrituras[j].nEscritura;
                        info.PrimeraEscritura.nRegistro = buffer_escrituras[j].nRegistro;
                    }
                    
                    if (buffer_escrituras[j].nEscritura > info.UltimaEscritura.nEscritura) {
                        info.UltimaEscritura.fecha = buffer_escrituras[j].fecha;
                        info.UltimaEscritura.nEscritura = buffer_escrituras[j].nEscritura;
                        info.UltimaEscritura.nRegistro = buffer_escrituras[j].nRegistro;
                    }
                    
                    if (buffer_escrituras[j].nRegistro < info.MenorPosicion.nRegistro) {
                        info.MenorPosicion.fecha = buffer_escrituras[j].fecha;
                        info.MenorPosicion.nEscritura = buffer_escrituras[j].nEscritura;
                        info.MenorPosicion.nRegistro = buffer_escrituras[j].nRegistro;
                    }
                    
                    if (buffer_escrituras[j].nRegistro > info.MayorPosicion.nRegistro) {
                        info.MayorPosicion.fecha = buffer_escrituras[j].fecha;
                        info.MayorPosicion.nEscritura = buffer_escrituras[j].nEscritura;
                        info.MayorPosicion.nRegistro = buffer_escrituras[j].nRegistro;
                    }
                    info.nEscrituras++; // Incrementamos el número de escrituras validadas
                }
            }
            memset(buffer_escrituras, 0, sizeof(buffer_escrituras)); // Limpiamos el buffer
            offset += sizeof(buffer_escrituras); // Sumamos el tamaño de la escritura al offset para preparar la lectura de la siguiente escritura
            lectura = mi_read(camino_fichero_prueba,buffer_escrituras,offset, sizeof(buffer_escrituras)); // Y la leemos.
        }

        // Introducimos la informacion del registro en un buffer
		char buffer[1024];
		memset(buffer, 0, 1024);
		sprintf(buffer, "PID: %i\nNumero de escrituras: %i\n", info.pid, info.nEscrituras);

		sprintf(buffer + strlen(buffer), "Primera escritura: %i %i %s", info.PrimeraEscritura.nEscritura, 
			info.PrimeraEscritura.nRegistro, asctime(localtime(&info.PrimeraEscritura.fecha)));
		sprintf(buffer + strlen(buffer), "Última escritura: %i %i %s", info.UltimaEscritura.nEscritura, 
			info.UltimaEscritura.nRegistro, asctime(localtime(&info.UltimaEscritura.fecha)));
		sprintf(buffer + strlen(buffer), "Menor posición: %i %i %s", info.MenorPosicion.nEscritura, 
			info.MenorPosicion.nRegistro, asctime(localtime(&info.MenorPosicion.fecha)));
		sprintf(buffer + strlen(buffer), "Mayor posición: %i %i %s\n", info.MayorPosicion.nEscritura, 
			info.MayorPosicion.nRegistro, asctime(localtime(&info.MayorPosicion.fecha)));

		// Escribirmos toda la información del buffer en camino_fichero_informe
		if (mi_write(camino_fichero_informe, buffer, nbytes_informe, strlen(buffer)) == -1) {
			fprintf(stderr, "Error al escribir en el informe\n");
			return EXIT_FAILURE;
		}
		nbytes_informe += strlen(buffer); // Y actualizamos acordemente la cantidad de bytes del informe.
	    printf("%d) %d escrituras validadas en %s\n", i, info.nEscrituras, camino_fichero_prueba);
	}
    
	// Desmontamos el dispositivo
	if (bumount() == -1) {
		fprintf(stderr, "Error al desmontar el sistema de archivos\n");
		return EXIT_FAILURE;
	}

	printf("Verificacion finalizada\n");
	return EXIT_SUCCESS;
	
}
