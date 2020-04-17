#include "ficheros.h"
#include<stdio.h>
int main(int argc, char **argv){

    int offsets[5]={9000,209000,30725000,409605000,480000000};

    //Comprobamos la sintaxis
    if(argc != 4){
		fprintf(stderr, "Argumentos esperados: <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>\n");
        fprintf(stderr,"Offsets:");
        for(int j = 0; j<(sizeof(offsets)/sizeof(offsets[0])); j++){
            fprintf(stderr," %d,",offsets[j]);
        }
        fprintf(stderr,"\nSi diferentes_inodos=0 se reserva un solo inodo para todos los offsets\n");
		return -1;
 	}

    //Montamos el disco
    if(bmount(argv[1])==-1) return EXIT_FAILURE;

    //Inicializamos las variables
    int tam = strlen(argv[2]);
    char buffer[tam];
    memset(buffer,0,tam);
    strcpy(buffer,argv[2]);
    int escritos;
    int ninodo;
    //if(ninodo==-1) return EXIT_FAILURE;
    //struct inodo ino;
    struct STAT st;
    struct tm *ts;   //struct de la libreria <time.h>, nos sirve para guardar la hora        POSIBLE BORRAR
    char atime[80];
    char mtime[80];
    char ctime[80];

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
        ts = localtime(&st.atime);
        strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&st.mtime);
        strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&st.ctime);
        strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

        //Imprimimos resultados
        fprintf(stderr,"Nº Inodo reservado: %d\n", ninodo);
        fprintf(stderr,"Offset: %d\n", offsets[i]);
        fprintf(stderr,"Bytes escritos: %d\n", escritos);
        fprintf(stderr,"DATOS INODO %d:\n", ninodo);
        fprintf(stderr,"Tipo=%c\n", st.tipo);
        fprintf(stderr,"Permisos=%d\n", st.permisos);
        fprintf(stderr,"Atime: %s\n", atime);
        fprintf(stderr,"Mtime: %s\n", mtime);
        fprintf(stderr,"Ctime: %s\n", ctime);
        fprintf(stderr,"nlinks=%d\n", st.nlinks);
        fprintf(stderr,"TamBytesLogicos: %d\n", st.tamEnBytesLog);
        fprintf(stderr,"NumBloquesOcupados: %d\n\n\n", st.numBloquesOcupados);
    }
    
    //Desmontamos el disco
    if(bumount(argv[1])==-1) return EXIT_FAILURE;

}