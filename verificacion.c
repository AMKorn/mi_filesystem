#include "verificacion.h"

#define NOMBRE_FICHERO_PRUEBA "prueba.dat"

#define DISCO argv[1]
#define DIR argv[2]

int main (int argc, char **argv) {
    // Comprobamos los parametros
	if (argc != 3) {
		printf("ERROR: faltan argumentos\n");
		printf("SYNTAX:\n $./verificacion <disco> </directorio> \n");
		exit(0);
	}
	
	// Montamos el dispositivo virtual
	if (bmount(DISCO) == -1) {
		printf("ERROR: Main: Fallo al montar el sistema de archivos\n");
		exit(0);
	}
	
	// Obtenemos la metainformacion del directorio de simulacion
	struct STAT stat;
 	if (mi_stat(DIR, &stat) == -1) {
		printf("ERROR: main: Fallo obteniendo la metainformacion del directorio de simulacion\n");
		exit(0);
	}

	printf("\ndir_sim: %s\n", DIR);
    
    int numentradas = stat.tamEnBytesLog / sizeof(struct entrada);
    if (numentradas != NUMPROCESOS) {
        return -1;
    }
	
    printf("numentradas: %d NUMPROCESOS: %d\n", numentradas, NUMPROCESOS);

    // Inicializamos el camino del fichero de informe
	char camino_fichero_informe[128];
	sprintf(camino_fichero_informe, "%s%s", DIR, "informe.txt");
	// Creamos el fichero de informe
	if (mi_creat(camino_fichero_informe, 7) == -1) {
		printf("ERROR: main: Error creando el fichero informe\n");
		exit(0);
	}
	
    int nbytes_informe = 0;
	struct entrada entradas[NUMPROCESOS*sizeof(struct entrada)];
	
	for (int i = 0; i < NUMPROCESOS; i++) {
	    struct INFORMACION info;
        if (mi_read(DIR, entradas, 0, sizeof(entradas)) == -1) {
		    printf("ERROR: main: Error leyendo entradas del directorio de simulacion\n");
		    exit(0);
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
        while (info.nEscrituras < NUMESCRITURAS && lectura > 0) {
            for (int j = 0; j < cant_registros_buffer_escrituras; j++) {
                if (buffer_escrituras[j].pid == info.pid) {
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
                    info.nEscrituras++;
                }
            }
            memset(buffer_escrituras, 0, sizeof(buffer_escrituras));
            offset += sizeof(buffer_escrituras);
            lectura = mi_read(camino_fichero_prueba,buffer_escrituras,offset, sizeof(buffer_escrituras));
        }

        // Escribimos la informacion del registros [j]_info en el fichero de informe
		char buffer[1024];
		memset(buffer, 0, 1024);
		sprintf(buffer, "PID: %i\nNumero de escrituras: %i\n", info.pid, info.nEscrituras);

		sprintf(buffer + strlen(buffer), "%s %i %i %s",
		"Primera escritura: ",
		info.PrimeraEscritura.nEscritura,
		info.PrimeraEscritura.nRegistro,
		asctime(localtime(&info.PrimeraEscritura.fecha)));

		sprintf(buffer + strlen(buffer), "%s %i %i %s",
		"Ultima escritura: ",
		info.UltimaEscritura.nEscritura,
		info.UltimaEscritura.nRegistro,
		asctime(localtime(&info.UltimaEscritura.fecha)));

		sprintf(buffer + strlen(buffer), "%s %i %i %s",
		"Menor posicion: ",
		info.MenorPosicion.nEscritura,
		info.MenorPosicion.nRegistro,
		asctime(localtime(&info.MenorPosicion.fecha)));

		sprintf(buffer + strlen(buffer), "%s %i %i %s %s",
		"Mayor posicion: ",
		info.MayorPosicion.nEscritura,
		info.MayorPosicion.nRegistro,
		asctime(localtime(&info.MayorPosicion.fecha)),
		"\n");

		if (mi_write(camino_fichero_informe, buffer, nbytes_informe, strlen(buffer)) == -1) {
			printf("ERROR: main: Error escribiendo en el informe\n");
		}
		nbytes_informe += strlen(buffer);
	    printf("%d) %d escrituras validadas en %s\n", i, info.nEscrituras, camino_fichero_prueba);
	}
    
	// Desmontamos el dispositivo
	if (bumount() == -1) {
		printf("ERROR: main: Error desmontando el dispositivo\n");
		exit(0);
	}
	printf("Verificacion finalizada\n");
	return 0;
	
}
