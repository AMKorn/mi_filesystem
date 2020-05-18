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
    if((inodo.permisos & MASC_WRTE) != MASC_WRTE){
        fprintf(stderr, "El inodo no tiene permisos de escritura. Permisos: %d\n", inodo.permisos);
        return 0;
    }

    unsigned int primerBLogico = offset/BLOCKSIZE;
    unsigned int ultimoBLogico = (offset + nbytes - 1)/BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1)%BLOCKSIZE;

    int bytes = offset;

    // Preparación1 para la escritura de los bloques
    int nbloque = traducir_bloque_inodo(ninodo, primerBLogico, 1);
    if(nbloque == -1) return -1;
    unsigned char buf_bloque[BLOCKSIZE];
    if(bread(nbloque, buf_bloque) == -1){
        fprintf(stderr, "mi_write_f(): Error de lectura de bloque.\n");
        return -1;
    }

    // Un único bloque
    if(primerBLogico == ultimoBLogico){
        memcpy(buf_bloque+desp1, buf_original, desp2-desp1+1);
        if(bwrite(nbloque, buf_bloque) == -1){
            fprintf(stderr, "mi_write_f(): Error de escritura de bloque. Único bloque.\n");
            return -1;
        }
        bytes += desp2-desp1+1;
    } else {
        // Primer bloque
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
        if(bwrite(nbloque, buf_bloque) == -1){
            fprintf(stderr, "mi_write_f(): Error de escritura de bloque. Primer bloque.\n");
            return -1;
        }
        bytes += BLOCKSIZE - desp1;
        
        // Bloques intermedios
        for(int i = primerBLogico+1; i < ultimoBLogico; i++){
            nbloque = traducir_bloque_inodo(ninodo, i, 1);
            if(bwrite(nbloque, buf_original + (BLOCKSIZE - desp1) + (i - primerBLogico - 1) * BLOCKSIZE) == -1){
                fprintf(stderr, "mi_write_f(): Error de escritura de bloque. Bloques intermedios.\n");
                return -1;
            }
            bytes += BLOCKSIZE;
        }

        // Último bloque
        nbloque = traducir_bloque_inodo(ninodo, ultimoBLogico, 1);
        if(bread(nbloque, buf_bloque) == -1){
            fprintf(stderr, "mi_write_f(): Error de lectura del último bloque.\n");
            return -1;
        }
        memcpy (buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
        if(bwrite(nbloque, buf_bloque) == -1){
            fprintf(stderr, "mi_write_f(): Error de escritura de bloque. Último bloque.\n");
            return -1;
        }        
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

    return bytes - offset;
}

/**
 * Lee información de un fichero/directorio correspondiente al nº de inodo pasado como argumento y la almacena en un buffer de memoria.
 * @param ninodo        - unsigned int en el que se indica el número de inodo a leer
 * @param buf_original  - void[] en el que almacenar la información a leer
 * @param offset        - unsigned int que indica el offset en número de bytes a partir del cual leer.
 * @param nbytes        - unsigned int que indica la cantidad de bytes a leer.
 * @return el número de bytes leídos, o -1 si hay un error.
 */
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){
    struct inodo inodo;
    if(leer_inodo(ninodo, &inodo) < 0){
        fprintf(stderr, "Error en mi_read_f: Error al leer el inodo.\n");
        return -1;
    }
    if((inodo.permisos & MASC_READ) != MASC_READ){
        fprintf(stderr, "Error en mi_read_f: El fichero o directorio no tiene permisos de lectura.\n");
        return -1;
    }

    if(offset >= inodo.tamEnBytesLog){
        return 0;
    }

    if(offset + nbytes >= inodo.tamEnBytesLog){
        nbytes = inodo.tamEnBytesLog - offset;
    }

    unsigned int primerBLogico = offset/BLOCKSIZE;
    unsigned int ultimoBLogico = (offset + nbytes - 1)/BLOCKSIZE;
    unsigned int desp1 = offset % BLOCKSIZE;
    unsigned int desp2 = (offset + nbytes - 1)%BLOCKSIZE;
    
    unsigned char buf_bloque[BLOCKSIZE];
    int leidos = 0;
    int nbfisico = traducir_bloque_inodo(ninodo, primerBLogico, 0);
    
    // Un único bloque
    if(primerBLogico == ultimoBLogico){
        if(nbfisico != -1){
            if(bread(nbfisico, buf_bloque) < 0){
				return -1;
			}
            memcpy(buf_original,buf_bloque + desp1, nbytes);
        }
        // Actualizamos la variable que controla el número de bytes escritos.
        leidos = nbytes;
    } else {
        // Varios bloques
        // Primer bloque
        if(nbfisico != -1){
            if(bread(nbfisico, buf_bloque) < 0){
				return -1;
			}
            memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
        }
        leidos = BLOCKSIZE - desp1;
        
        // Bloques intermedios
        for(int i = primerBLogico+1; i < ultimoBLogico; i++){
            nbfisico = traducir_bloque_inodo(ninodo, i, 0);
            if(nbfisico != -1){
                if(bread(nbfisico, buf_bloque) < 0){
				    return -1;
			    }
                memcpy(buf_original + (BLOCKSIZE - desp1) + (i - primerBLogico - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);
            }
            leidos += BLOCKSIZE;
        }
        
        // Último bloque
        nbfisico = traducir_bloque_inodo(ninodo, ultimoBLogico, 0);
        if(nbfisico != -1){
            if(bread(nbfisico, buf_bloque) < 0){
				return EXIT_FAILURE;
			}
            memcpy (buf_original+(nbytes-desp2-1), buf_bloque, desp2+1);
        }
        leidos += desp2+1;
    }

    

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
    struct inodo inodo;
    if(leer_inodo(ninodo, &inodo) == EXIT_FAILURE){
        return EXIT_FAILURE;
    }

    /*
    //Comprobar que tiene permisos de escritura
    if((inodo.permisos & 2) != 2){
        fprintf(stderr, "El inodo no tiene permisos de escritura.\n");
        return EXIT_FAILURE;
    }*/

    inodo.permisos = permisos;

    inodo.ctime = time(NULL);

    if(escribir_inodo(ninodo, inodo) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Nivel 6

/**
 * Trunca un fichero/directorio a los bytes indicados y liberando los bloques necesarios.
 * @param ninodo - El número del inodo que se quiere truncar.
 * @param nbytes - El número de bytes que se quieren trucar.
 * @return el número de bloques liberados, o -1 si hay un error
 */
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
    inodo.numBloquesOcupados -= bloquesLiberados;
    
    escribir_inodo(ninodo, inodo);

    printf("DATOS INODO %d:\n", ninodo);
    printf("tipo=%c\n", inodo.tipo);
    printf("...\n");
    printf("tamEnBytesLog=%d\n", inodo.tamEnBytesLog);
    printf("numBloquesOcupados=%d\n", inodo.numBloquesOcupados);


    return bloquesLiberados;
}
