#include "verificacion.h"

int main (int argc, char **argv) {
    // Comprobamos los parametros
	if (argc != 3) {
		printf("ERROR: faltan argumentos\n");
		printf("SYNTAX:\n $./verificacion <disco> </directorio> \n");
		exit(0);
	}
	
	// Montamos el dispositivo virtual
	if (bmount(argv[1]) == -1) {
		printf("ERROR: Main: Fallo al montar el sistema de archivos\n");
		exit(0);
	}
	
	// Obtenemos la metainformacion del directorio de simulacion
	struct STAT stat;
 	if (mi_stat(argv[2], &stat) == -1) {
		printf("ERROR: main: Fallo obteniendo la metainformacion del directorio de simulacion\n");
		exit(0);
	}

	printf("\ndir_sim: %s\n", argv[2]);
    
    int numentradas = stat.tamEnBytesLog / sizeof(struct entrada);
    if (numentradas != NUM_PROCESOS) {
        return -1;
    }
	printf("numentradas: %d NUM_PROCESOS: %d\n", numentradas, NUM_PROCESOS);

    // Inicializamos el camino del fichero de informe
	char camino_fichero_informe[128];
	sprintf(camino_fichero_informe, "%s%s", argv[2], "informe.txt");
	// Creamos el fichero de informe
	if (mi_creat(camino_fichero_informe, 7) == -1) {
		printf("ERROR: main: Error creando el fichero informe\n");
		exit(0);
	}
	
	struct entrada entradas[NUM_PROCESOS];
	if (mi_read(argv[2], entradas, 0, sizeof(entradas)) == -1) {
		printf("ERROR: main: Error leyendo entradas del directorio de simulacion\n");
		exit(0);
	}
	int i, nbytes_informe = 0;
	for (int i = 0; i < NUM_PROCESOS; i++) {
	    struct INFORMACION info;
	    info.pid = atoi(strchr(entradas[i].nombre, '_') + 1);
		
		info.nEscrituras = 0;
		char camino_fichero_prueba[128];
		sprintf(camino_fichero_prueba, "%s%s/%s", argv[2], entradas[i].nombre, NOMBRE_FICHERO_PRUEBA);
		
		int offset = 0;
		
		int cant_registros_buffer_escrituras = 6144; //
		struct REGISTRO buffer_escrituras [cant_registros_buffer_escrituras];
		while (mi_read(camino_fichero_prueba,buffer_escrituras,offset, sizeof(buffer_escrituras)) > 0) {
			int j;
            for (j = 0; j < cant_registros_buffer_escrituras; j++) {
				if (buffer_escrituras[j].pid == info.pid) {
					
					if (info.nEscrituras == 0) {
						info.PrimeraEscritura = buffer_escrituras[j];
						info.UltimaEscritura = buffer_escrituras[j];
						info.MenorPosicion = buffer_escrituras[j];
						info.MayorPosicion = buffer_escrituras[j];
					} else {
						if(info.PrimeraEscritura.nEscritura > buffer_escrituras[j].nEscritura){
							info.PrimeraEscritura = buffer_escrituras[j];
						} else if(info.UltimaEscritura .nEscritura < buffer_escrituras[j].nEscritura){
							info.UltimaEscritura  = buffer_escrituras[j];
						}
						if (buffer_escrituras[j].posicion < info.MenorPosicion.posicion) {
							info.MenorPosicion = buffer_escrituras[j];
						} else if (buffer_escrituras[j].posicion > info.MayorPosicion.posicion) {
							info.MayorPosicion = buffer_escrituras[j];
						}
					}
					info.nEscrituras++;
				}
			}
			memset(buffer_escrituras, 0, sizeof(buffer_escrituras));
			offset += sizeof(buffer_escrituras);
        }
        // Escribimos la informacion del registros [j]_info en el fichero de informe
		char buffer[1024];
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
	// Desmontamos el dispositivo
	if (bumount() == -1) {
		printf("ERROR: main: Error desmontando el dispositivo\n");
		exit(0);
	}
	printf("Verificacion finalizada\n");
	return 0;
	
}
