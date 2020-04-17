#include "ficheros.h"

/**
 * Escribe el contenido procedente de un buffer de memoria, buf_original, de tamaño nbytes, en un fichero/directorio
 * @param ninodo - el número del inodo en el que escribir
 * @param buf_original - puntero que contiene la información a escribir
 * @param nbytes - el tamaño de la información a escribir
 * @return el número de bytes escritos, o -1 si hay un error.
 */
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes){
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    if((inodo.permisos & 2) != 2){
        fprintf(stderr, "El inodo no tiene permisos de escritura.\n");
        return -1;
    }

    unsigned int primerBLogico = offset/BLOCKSIZE;
    unsigned int ultimoBLogico = (offset + nbytes - 1)/BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1)%BLOCKSIZE;

    int bytes = offset;

    // Preparación para la escritura de los bloques
    int nbloque = traducir_bloque_inodo(ninodo, primerBLogico, 1);
    if(nbloque == -1) return -1;
    unsigned char buf_bloque[BLOCKSIZE];
    bread(nbloque, buf_bloque);
    fprintf(stdout, "desp1: %d\n", desp1);

    // Un único bloque
    if(primerBLogico == ultimoBLogico){
        memcpy(buf_bloque+desp1, buf_original, desp2-desp1+1);
        bwrite(nbloque, buf_bloque);

        bytes += desp2-desp1+1;
    } else {
        // Primer bloque
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
        bwrite(nbloque, buf_bloque);
        bytes += BLOCKSIZE - desp1;
        
        // Bloques intermedios
        for(int i = primerBLogico+1; i < ultimoBLogico; i++){
            nbloque = traducir_bloque_inodo(ninodo, i, 1);
            bytes += bwrite(nbloque, buf_original + (BLOCKSIZE - desp1) + (i - primerBLogico - 1) * BLOCKSIZE);
        }

        // Último bloque
        nbloque = traducir_bloque_inodo(ninodo, ultimoBLogico, 1);
        bread(nbloque, buf_bloque);
        memcpy (buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
        bwrite(nbloque, buf_bloque);
        bytes += desp2+1;
    }
    // Actualización de metainformación
    leer_inodo(ninodo, &inodo);
    if(inodo.tamEnBytesLog < bytes) {
        inodo.tamEnBytesLog = bytes;
        inodo.ctime = time(NULL);
    }
    inodo.mtime = time(NULL);
    escribir_inodo(ninodo, inodo);

    return bytes-offset;
}

/**
 * Lee información de un fichero/directorio correspondiente al nº de inodo pasado como argumento y la almacena en un buffer de memoria.
 */
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    int leidos = 0;
    if((inodo.permisos & 4) != 4){
        fprintf(stderr, "El inodo no tiene permisos de lectura.\n");
        return -1;
    }

    if(offset >= inodo.tamEnBytesLog){
        leidos = 0;
        //fprintf(stderr, "offset >= inodo.tamEnBytesLog\n");
        return leidos;
    }

    if(offset + nbytes >= inodo.tamEnBytesLog){
        nbytes = inodo.tamEnBytesLog - offset;
    }

    unsigned int primerBLogico = offset/BLOCKSIZE;
    unsigned int ultimoBLogico = (offset + nbytes - 1)/BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1)%BLOCKSIZE;
    int nbloque;
    unsigned char buf_bloque[BLOCKSIZE];
    memset(buf_bloque, 0, BLOCKSIZE);

    // Preparación para la escritura de los bloques
    //fprintf(stderr, "TADRUCIENDO BLOQUE:\nbloque: %d \nninodo: %d \nprimerBLogico: %d \n",nbloque, ninodo, primerBLogico);
    nbloque = traducir_bloque_inodo(ninodo,primerBLogico,0);
    if(nbloque == -1){									

        leidos = nbytes;
        return leidos;
    }
    if(bread(nbloque, buf_bloque)== -1) return -1;


    // Un único bloque
    if(primerBLogico == ultimoBLogico){
        memcpy(buf_original,buf_bloque + desp1, nbytes);
        // Actualizamos la variable que controla el número de bytes escritos.
        leidos = nbytes;
        //fprintf(stderr, "P2: %d\n",leidos);
        return leidos;      //[Ruben]: si retornas ahora no actualizaras los inodo.times, creo
    }

    // Varios bloques
    // Primer bloque
    memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
    leidos = BLOCKSIZE - desp1;
    
    // Bloques intermedios
    for(int i = primerBLogico+1; i < ultimoBLogico; i++){
        nbloque = traducir_bloque_inodo(ninodo, i, 0);
        if(nbloque==-1){ 
            //fprintf(stderr, "P3: %d\n",leidos);
            return leidos;
        }
        if(bread(nbloque, buf_bloque)== -1) return -1;
        memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
        leidos += BLOCKSIZE;
    }
    
    // Último bloque
    nbloque = traducir_bloque_inodo(ninodo, ultimoBLogico, 0);
    if(nbloque==-1)return leidos;
    if(bread(nbloque, buf_bloque)== -1) return -1;
    memcpy (buf_original+(nbytes-desp2-1), buf_bloque, desp2+1);
    leidos += desp2+1;

    // Actualización de metainformación
    leer_inodo(ninodo, &inodo);
    inodo.atime = time(NULL);
    if(escribir_inodo(ninodo, inodo) < 0) return -1;

    //fprintf(stderr, "P4: %d\n",leidos);    
    return leidos;
}

/**
 * Devuelve la metainformación de un fichero/directorio utilizando un struct STAT.
 * @param ninodo - El número del inodo cuya metainformación se quiere consultar.
 * @param p_stat - Puntero al struct STAT en el que se almacena la metainformación.
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){
    struct inodo inodo;
    if(leer_inodo(ninodo, &inodo) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->atime = inodo.atime;
    p_stat->ctime = inodo.ctime;
    p_stat->mtime = inodo.mtime;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
    return EXIT_SUCCESS;
}

/**
 * Cambia los derechos de un fichero
 * @param ninodo - El número del inodo cuya metainformación se quiere consultar.
 * @param permisos - Nuevos permisos del fichero
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */
int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
    //Comprobar que tiene permisos de escritura
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    if((inodo.permisos & 2) != 2){
        fprintf(stderr, "El inodo no tiene permisos de escritura.\n");
        return EXIT_FAILURE;
    }

    inodo.permisos = permisos;

    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);
    return EXIT_SUCCESS;
}

/**
 * Actualiza los permisos de un fichero/directorio.
 * @param ninodo - Número del inodo cuyos permisos se quieren modificar.
 * @param permisos - Los permisos que se quieren establecer.
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */
 int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
    struct inodo inodo;
    if(leer_inodo(ninodo, &inodo) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    inodo.permisos = permisos;
    inodo.ctime = time(NULL);
    escribir_inodo(ninodo, inodo);
    return EXIT_SUCCESS;
 }

 // Nivel 6

 /**
  * Trunca un fichero/directorio a los bytes indicados y liberando los bloques necesarios.
  * @param ninodo - El número del inodo que se quiere truncar.
  * @param nbytes - El número de bytes que se quieren trucar.
  * @return el número de bloques liberados, o -1 si hay un error
  * */
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){
    //Comprobar que tiene permisos de escritura y que nbytes <= tamEnBytesLog
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    if((inodo.permisos & 2) != 2){
        fprintf(stderr, "El inodo no tiene permisos de escritura.\n");
        return -1;
    }
    if(inodo.tamEnBytesLog < nbytes){
        fprintf(stderr, "No se puede truncar más allá del tamaño en bytes lógicos del fichero/directorio.\n");
        return -1;
    }

    int primerBL;
    if(nbytes%BLOCKSIZE == 0){
        primerBL = nbytes/BLOCKSIZE;
    } else {
        primerBL = nbytes/BLOCKSIZE+1;
    }

    int bloquesLiberados = liberar_bloques_inodo(primerBL, &inodo);
    if(bloquesLiberados==-1){
        fprintf(stderr, "Error en liberar_bloques_inodo() desde mi_truncar_f().\n");
        return -1;
    }

    inodo.mtime = time(NULL);
    inodo.ctime = time(NULL);
    inodo.tamEnBytesLog = nbytes;

    return bloquesLiberados;
}
