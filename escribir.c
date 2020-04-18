#include "ficheros.h"
#include<stdio.h>


int main(int argc, char **argv){

    int offsets[5]={9000,209000,30725000,409605000,480000000};

    //Comprobamos la sintaxis
    if(argc != 4){
		fprintf(stderr, "Argumentos esperados: <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>\n");
        fprintf(stdout,"Offsets:");
        for(int j = 0; j<(sizeof(offsets)/sizeof(offsets[0])); j++){
            fprintf(stdout," %d,",offsets[j]);
        }
        fprintf(stdout,"\nSi diferentes_inodos=0 se reserva un solo inodo para todos los offsets\n");
		return -1;
 	}

    //Montamos el disco
    if(bmount(argv[1])==-1) return EXIT_FAILURE;

    //Inicializamos las variables
    int tam = strlen(argv[2]);
    char buffer[tam];
    memset(buffer,0,tam);
    strcpy(buffer,argv[3]);
    int escritos;
    int ninodo;
    struct STAT st;
    /*struct tm *ts;   //struct de la libreria <time.h>, nos sirve para guardar la hora        POSIBLE BORRAR
    char atime[80];
    char mtime[80];
    char ctime[80];*/

    //Iniciamos la escritura
    printf("Offsets:");
    for(int j = 0; j<(sizeof(offsets)/sizeof(offsets[0])); j++){
        printf(" %d,",offsets[j]);
    }
    
    printf("\nLongitud texto: %d\n\n", tam);

    //Un mismo inodo
    if(atoi(argv[3])==0){
        ninodo= reservar_inodo('f',6);
    }
    for(int i = 0; i<(sizeof(offsets)/sizeof(offsets[0])); i++){
        //Si diferentes_inodo vale 1, reservaremos un nuevo inodo en cada iteracion
        if(atoi(argv[3])==1){
            ninodo= reservar_inodo('f',6);
        }
        escritos = mi_write_f(ninodo, buffer, offsets[i], tam);
        if(escritos==-1) {
            fprintf(stderr,"Error durante la escritura.\n");
            return EXIT_FAILURE;
        }
        if(mi_stat_f(ninodo, &st)== -1) {
            fprintf(stderr,"Error en mi_stat_f.\n");
            return EXIT_FAILURE;
        }
        /*ts = localtime(&st.atime);
        strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&st.mtime);
        strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&st.ctime);
        strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);*/

        //Imprimimos resultados
        fprintf(stdout,"Nº Inodo reservado: %d\n\n", ninodo);
        fprintf(stdout,"Offset: %d\n", offsets[i]);
        fprintf(stdout,"Bytes escritos: %d\n", escritos);
        fprintf(stdout,"\nDATOS INODO %d:\n...\n", ninodo);
        /*fprintf(stdout,"tipo=%c\n", st.tipo);
        fprintf(stdout,"permisos=%d\n", st.permisos);
        fprintf(stdout,"atime: %s\n", atime);
        fprintf(stdout,"mtime: %s\n", mtime);
        fprintf(stdout,"ctime: %s\n", ctime);
        fprintf(stdout,"nlinks=%d\n", st.nlinks);*/
        fprintf(stdout,"stat.tamBytesLogicos: %d\n", st.tamEnBytesLog);
        fprintf(stdout,"stat.numBloquesOcupados: %d\n\n\n", st.numBloquesOcupados);
    }
    
    //Desmontamos el disco
    if(bumount(argv[1])==-1) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}