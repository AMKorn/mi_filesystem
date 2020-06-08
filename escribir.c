/* Autores:
 * Andreas Manuel Korn
 * Sergio Vega García
 * Rubén López Babón
 */

#include "ficheros.h"

#define DISCO argv[1]
#define FICHERO argv[2]
#define INODOS atoi(argv[3])
#define MODO argv[4]


//2 modos (-t, directamente desde consola y -f, lectura desde fichero)
int main(int argc, char **argv){

    int offsets[5]={9000,209000,30725000,409605000,480000000};

    //Comprobamos la sintaxis
    if(argc != 5){
		fprintf(stderr, "Sintaxis: ./escribir <nombre_dispositivo> <texto|ruta_fichero> <diferentes_inodos> <modo>\n");
        fprintf(stderr,"Offsets:");
        fprintf(stderr," %d",offsets[0]);
        for(int j = 1; j<(sizeof(offsets)/sizeof(offsets[0])); j++){
            fprintf(stderr,", %d",offsets[j]);
        }
        fprintf(stderr,"\nSi diferentes_inodos=0 se reserva un solo inodo para todos los offsets\n");
		return -1;
 	}

    if(MODO[1]!='f' && MODO[1]!='t'){
        fprintf(stderr, "Modo introducido invalido, modos validos: -f(lectura desde fichero), -t(lectura desde consola)\n");
        return EXIT_FAILURE;
    }

    //Montamos el disco
    if(bmount(DISCO)==-1) {
        fprintf(stderr, "Error de montaje de disco.\n");
        return EXIT_FAILURE;
    }

    //Inicializamos las variables
    int escritos;
    int ninodo;
    //struct inodo ino;
    struct STAT st;
    struct tm *ts;   //struct de la libreria <time.h>, nos sirve para guardar la hora        POSIBLE BORRAR
    char atime[80];
    char mtime[80];
    char ctime[80];

    if(MODO[1]=='f'){        
        char *buffer;
        long tam;
        FILE *f = fopen(FICHERO, "rb");
        fseek (f, 0, SEEK_END);
        tam = ftell(f);
        fseek (f, 0, SEEK_SET);
        buffer = (char*)malloc ((tam+1)*sizeof(char));
        if(buffer){
            fread(buffer, sizeof(char), tam, f);
        }
        fclose (f);

        //Iniciamos la escritura
        printf("Offsets:");
        printf(" %d",offsets[0]);
        for(int j = 1; j<(sizeof(offsets)/sizeof(offsets[0])); j++){
            printf(", %d",offsets[j]);
        }
        printf("\nLongitud texto: %ld\n\n", tam);

        //Un mismo inodo
        int diferentes_inodos = INODOS;
        
        for(int i = 0; i<(sizeof(offsets)/sizeof(offsets[0])); i++){
            //Si diferentes_inodo vale 1, reservaremos un nuevo inodo en cada iteracion
            if (diferentes_inodos || i == 0){
                ninodo = reservar_inodo('f',6);
            }
            
            printf("Nº Inodo reservado: %d\n", ninodo);
            printf("Offset: %d\n", offsets[i]);

            escritos = mi_write_f(ninodo, buffer, offsets[i], tam);
            if(escritos==-1) {
                fprintf(stderr,"Error durante la escritura.\n");
                return EXIT_FAILURE;
            }
            if(mi_stat_f(ninodo, &st)== -1) {
                fprintf(stderr,"Error en mi_stat_f.\n");
                return EXIT_FAILURE;
            }
            ts = localtime(&st.atime);
            strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
            ts = localtime(&st.mtime);
            strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
            ts = localtime(&st.ctime);
            strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

            //Imprimimos resultados
            printf("Bytes escritos: %d\n", escritos);
            printf("\nDATOS INODO %d:\n", ninodo);
            printf("Tipo=%c\n", st.tipo);
            printf("Permisos=%d\n", st.permisos);
            printf("atime: %s\n", atime);
            printf("mtime: %s\n", mtime);
            printf("ctime: %s\n", ctime);
            printf("nlinks=%d\n", st.nlinks);
            printf("TamBytesLogicos: %d\n", st.tamEnBytesLog);
            printf("NumBloquesOcupados: %d\n\n", st.numBloquesOcupados);
        }
    } else if(MODO[1]=='t'){
        long tam = strlen(FICHERO);
        char buffer[tam];
        memset(buffer, 0, tam);
        strcpy(buffer, FICHERO);

        //Iniciamos la escritura
        printf("Offsets:");
        printf(" %d",offsets[0]);
        for(int j = 1; j<(sizeof(offsets)/sizeof(offsets[0])); j++){
            printf(", %d",offsets[j]);
        }
        printf("\nLongitud texto: %ld\n\n", tam);

        //Un mismo inodo
        int diferentes_inodos = INODOS;

        for(int i = 0; i<(sizeof(offsets)/sizeof(offsets[0])); i++){
            //Si diferentes_inodo vale 1, reservaremos un nuevo inodo en cada iteracion
            if (diferentes_inodos || i == 0){
                ninodo = reservar_inodo('f',6);
            }
            
            printf("Nº Inodo reservado: %d\n", ninodo);
            printf("Offset: %d\n", offsets[i]);

            escritos = mi_write_f(ninodo, buffer, offsets[i], tam);
            if(escritos==-1) {
                fprintf(stderr,"Error durante la escritura.\n");
                return EXIT_FAILURE;
            }
            if(mi_stat_f(ninodo, &st)== -1) {
                fprintf(stderr,"Error en mi_stat_f.\n");
                return EXIT_FAILURE;
            }
            ts = localtime(&st.atime);
            strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
            ts = localtime(&st.mtime);
            strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
            ts = localtime(&st.ctime);
            strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

            //Imprimimos resultados
            printf("Bytes escritos: %d\n", escritos);
            printf("\nDATOS INODO %d:\n", ninodo);
            printf("Tipo=%c\n", st.tipo);
            printf("Permisos=%d\n", st.permisos);
            printf("atime: %s\n", atime);
            printf("mtime: %s\n", mtime);
            printf("ctime: %s\n", ctime);
            printf("nlinks=%d\n", st.nlinks);
            printf("TamBytesLogicos: %d\n", st.tamEnBytesLog);
            printf("NumBloquesOcupados: %d\n\n", st.numBloquesOcupados);
        }
    }
    
    //Desmontamos el disco
    if(bumount(DISCO)==-1) {
        fprintf(stderr, "Error al desmontar el disco.\n");
        return EXIT_FAILURE;
    }

}
