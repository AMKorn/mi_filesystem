/******************************************************************************
*                                ESCRIBIR.C 
* PROYECTO: ENTREGA 1 - SISTEMAS OPERACTIVOS II - GRADO ING. INFORMATICA - UIB
*
* DESCRIPCIÓN: ESCRIBIRÁ TEXTO EN UNO O VARIOS INODOS HACIENDO USO DE 
*              RESERVAR_INODO('F', 6) PARA OBTENER UN NÚMERO DE INODO QUE 
*              MOSTRAREMOS POR PANTALLA YA DEMÁS UTILIZAREMOS COMO PARÁMETRO 
*              PARA EL MI_WRITE_F().
*
* AUTORES: JUAN CARLOS BOO CRUGEIRAS
*          HÉCTOR MARIO MEDINA CABANELAS
*
* FECHA: 03 DE ABRIL DE 2019.
******************************************************************************/

#include<stdio.h>
#include "ficheros.h"

int main(int argc, char **argv){
    /**************************************************************************
    *                              DECLARACIONES      
    *************************************************************************/
    // Declaramos un array de offsets para utilizar los distintos tipos de punteros
    int offsets[5] = {0, 5120, 256000, 30720000, 71680000};
    // Declaraciones que nos servirán posteriormente
    int longitud, diferentes_inodos, ninodo;
    // Struc STAT simpifica la estructura 'inodo'
    struct STAT stat;
    /**************************************************************************
     *                 COMPROBAMOS SI LA SINTAXIS ES CORRECTA
     *************************************************************************/
    // Comprobamos numero correcto de argumentos
    if(argc != 4){
        printf("Sintaxis: escribir <nombre_dispositivo> <\"$(cat fichero)\"> <diferentes_inodos>\n");
        printf("Offsets: %d, %d, %d, %d, %d\n", offsets[0], offsets[1], offsets[2],offsets[3],offsets[4]);
        printf("Si diferentes_inodos=0 se reserva un solo inodo para todos los offsets\n");
        exit(EXIT_FAILURE);
    }
    // Comprobamos el rango del argumento 3
    if(!(atoi(argv[3]) == 0 || atoi(argv[3]) == 1)){
        printf("Sintaxis: argumento 3 solo puede ser 1 o 0.\n");
        exit(EXIT_FAILURE);
    }
    /**************************************************************************
     *                    MONTAMOS EL DISPOSITIVO VIRTUAL
     *************************************************************************/
    int r = bmount(argv[1]);
    if(r == -1){
        fprintf(stderr,"Error al montar el disco.\n");
        exit(EXIT_FAILURE);
    }
    /**************************************************************************
     *                    MOSTRAMOS LA LONGITUD DE FICHERO
     *************************************************************************/
    longitud = strlen(argv[2]);
    printf("longitud texto: %d\n\n", longitud);
    /**************************************************************************
     *                    MOSTRAMOS INFORMACIÓN INODOS
     *************************************************************************/
    // Imprimimos la cantidad de inodos en los que guardaremos los datos
    diferentes_inodos = atoi(argv[3]);
    // Calculamos la estructura del array que nos ayudará para iterar
    int lenght = sizeof(offsets)/ sizeof(offsets[0]);
    // creamos un buffer
    char buffer[longitud];
    // Copiamos los datos que se han de introducir en el fichero en el buffer
    strcpy(buffer, argv[2]);
    // Reservamos un inodo y guardamos su posición
    ninodo = reservar_inodo('f',6);
    // Creamos un struct tm que nos ayuda a guardar la hora ( librería <time.h>>)
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];
    /**************************************************************************
     *                  BUCLE DONDE REALIZAMOS LA ESCRITURA
     *************************************************************************/
    for(int i = 0; i < lenght; i++){
        // Si el tercer argumento es 1, reservamos un nuevo inodo en cada bucle,
        // si es 0, todo en el mismo inodo.
        if(diferentes_inodos != 0 && i > 0){
            ninodo = reservar_inodo('f',6);
        }
        // Imprimimos información sobre el inodo reservado
        printf("\nNodo reservado: %d\n", ninodo );
        printf("Offset: %d\n", offsets[i]);
        // Escribimos en el inodo
        int bytesEscritos = mi_write_f(ninodo, buffer, offsets[i], longitud);
        if(bytesEscritos == -1){
            fprintf(stderr,"Error al escribir.\n");
            exit(EXIT_FAILURE);
        }
        // Imprimimos por pantalla el número de bytes escritos
        printf("Bytes escritos: %d\n\n", bytesEscritos);
        // Guardamos la metainformación del directorio en el struct stat
        if(mi_stat_f(ninodo, &stat) == -1){ // y comprobamos error
            fprintf(stderr,"Error al escribir.\n");
            exit(EXIT_FAILURE);            
        }
        // Imprimimos los datos del inodo
        printf("DATOS INODO %d\n", ninodo);
        printf("tipo=%c\n", stat.tipo);
        printf("permisos=%d\n", stat.permisos);
        // Introducimos los tiempos de modificación, acceso y creación
        ts = localtime(&stat.atime);
        strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&stat.mtime);
        strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&stat.ctime);
        strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
        // Y los imprimimos, además del resto de información del inodo
        printf("atime: %s\n", atime);
        printf("mtime: %s\n", mtime);
        printf("ctime: %s\n", ctime);
        printf("nlinks=%d\n", stat.nlinks);
        printf("tamEnBytesLog=%d\n", stat.tamEnBytesLog);
        printf("numBloquesOcupados=%d\n", stat.numBloquesOcupados);

    }
    // Desmontamos el fichero y comprobamos si hay algún error en el proceso
    if(bumount() == -1){
        fprintf(stderr,"Error al desmontar el disco.\n");
        exit(EXIT_FAILURE);
    }
    // Finalizamos
    exit(EXIT_SUCCESS);
}