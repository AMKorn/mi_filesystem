#include "ficheros.h"
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes){
    struct inodo inodo;                     // Estructura inodo
    int r = leer_inodo(ninodo, &inodo);     // Leemos el inodo ninodo
    // Si ha habido algún error.
    if(r == -1){
        // Imprimimos el error por la salida estándar de errores.
        fprintf(stderr, "Error my_writef(): Lectura inodo incorrecta.\n");
        return -1; 
    }
    // Si no tenemos los permisos de escritura.
    if((inodo.permisos & 2) != 2){
        // Imprimimos el error por la salida estándar de errores.
        fprintf(stderr, "Error my_writef(): No tiene permisos necesarios.\n");
        return -1;       
    }
    int bytesEscritos = 0;                          // Número de bytes escritos
    int firstblogico = offset/BLOCKSIZE;            // Primer bloque lógico
    int lastblogico = (offset+nbytes-1)/BLOCKSIZE;  // Último bloque lógico
    unsigned char buffer[BLOCKSIZE];                // Buffer auxiliar
    int bfisico;                                    // Bloque físico
    /**************************************************************************
     *                    SI SÓLO SE VA A ESCRIBIR UN BLOQUE 
     *************************************************************************/
    if(firstblogico == lastblogico){
        int desp1 = offset % BLOCKSIZE;             // Desplazamiento en bytes.
        // Calculamos bloque físico en el que trabajaremos.
        bfisico = traducir_bloque_inodo(ninodo,firstblogico,1);
        // Si ha habido algún error.
        if(bfisico == -1){
            // Imprimimos el error por la salida estándar de errores.
            fprintf(stderr, "Error my_writef(): Error al traducir bloque inodo.\n");
            return -1;
        }
        // Leemos el bloque físico
        r = bread(bfisico, buffer);
        // Si ha habido algún error.
        if(r == -1){
            // Imprimimos el error por la salida estándar de errores.
            fprintf(stderr, "Error my_writef(): Error al leer el bloque.\n");
            return -1;
        }
        // Compiamos los datos en el bloque con offset
        memcpy(buffer + desp1, buf_original, nbytes);
        // Escribimos el buffer en el bloque físico correspondiente.
        r = bwrite(bfisico, buffer);
        if(r == -1){
            // Imprimimos el error por la salida estándar de errores.
            fprintf(stderr, "Error my_writef(): Error al escribir el bloque.\n");
            return -1;
        }
        // Actualizamos la variable que controla el número de bytes escritos.
        bytesEscritos = nbytes;
    }
    /**************************************************************************
     *                SI SÓLO SE VA A ESCRIBIR MÁS DE UN BLOQUE 
     *************************************************************************/
    if(firstblogico != lastblogico){
        // Calculamos el primer bloque físico.
        int primerBF = traducir_bloque_inodo(ninodo, firstblogico, 1);
        /**********************************************************************
         *                     PRIMER BLOQUE A ESCRIBIR
         *********************************************************************/
        r = bread(primerBF, buffer);
        // Si ha habido algún error.
        if(r == -1){
            // Imprimimos el error por la salida estándar de errores.
            fprintf(stderr, "Error my_writef(): Error al leer el bloque.\n");
            return -1;
        }
        int desp1 = offset % BLOCKSIZE;              // Desplazamiento en bytes
        // Copiamos la primera parte de los datos en el primer bloque con offset
        memcpy(buffer + desp1, buf_original, BLOCKSIZE-desp1);  
        // Escribimos el bloque 
        r = bwrite(primerBF, buffer);
        // Si ha habido algún error.
        if(r == -1){
            // Imprimimos el error por la salida estándar de errores.
            fprintf(stderr, "Error my_writef(): Error al escribir el bloque.\n");
            return -1;
        }
        // Actualizamos los bytes escritos.
        bytesEscritos = BLOCKSIZE - desp1;
        /**********************************************************************
         *                  BLOQUES INTERMEDIOS A ESCRIBIR
         *********************************************************************/
        // Recorremos los bloques lógicos.
        for(int i = firstblogico+1; i < lastblogico; i++){
            // Calculamos el bloque físico.
            bfisico = traducir_bloque_inodo(ninodo,i,1);
            // Escribimos todos los datos, ya que no debemos preservar nada.
            r = bwrite(bfisico, buf_original + (BLOCKSIZE - desp1) + (i - firstblogico - 1)*BLOCKSIZE);
            if(r == -1){
                // Imprimimos el error por la salida estándar de errores.
                fprintf(stderr, "Error my_writef(): Error al escribir el bloque.\n");
                return -1;
            }
            // Actualizamos los bytes leídos.
            bytesEscritos += BLOCKSIZE;
        }
        /**********************************************************************
         *                       ÚLTIMO BLOQUE A ESCRIBIR
         *********************************************************************/
        // Calculamos último bloque físico.
        int ultimoBF = traducir_bloque_inodo(ninodo,lastblogico,1);
        r = bread(ultimoBF, buffer);
        if(r == -1){
            // Imprimimos el error por la salida estándar de errores.
            fprintf(stderr, "Error my_writef(): Error al leer el bloque.\n");
            return -1;
        }
        // Calculamos los bytes que hemos escrito.
        int bytesUltimos = (offset + nbytes -1)%BLOCKSIZE;
        // Actualizamos el buffer con los datos a escribir.
        memcpy(buffer, 
               buf_original+(BLOCKSIZE - desp1)+(lastblogico-firstblogico-1)*BLOCKSIZE,
               bytesUltimos+1);
        // Escribimos el bloque en disco.
        r = bwrite(ultimoBF, buffer);
        if(r == -1){
            // Imprimimos el error por la salida estándar de errores.
            fprintf(stderr, "Error my_writef(): Error al escribir el bloque.\n");
            return -1;
        }
        // Actualizamos los bloques escritos.
        bytesEscritos += bytesUltimos+1;
    }
    // leemos el inodo
    r = leer_inodo(ninodo, &inodo);
    if(r == -1){
        // Imprimimos el error por la salida estándar de errores.
        fprintf(stderr, "Error my_writef(): Error al leer el inodo.\n");
        return -1;
    }
    // Si hemos añadido bytes al fichero
    if(offset + nbytes > inodo.tamEnBytesLog){
        // Actualizamos el valor en el inodo
        inodo.tamEnBytesLog = offset + nbytes;
    }
    // Actualizamos fecha de modificación.
    inodo.mtime = time(NULL);
    // Actualizamos fecha de creación.
	inodo.ctime = time(NULL);
    // Escribimos el inodo.
    r = escribir_inodo(ninodo, inodo);
    if(r == -1){
        // Imprimimos el error por la salida estándar de errores.
        fprintf(stderr, "Error my_writef(): Error al escribir el inodo.\n");
        return -1;
    }
    // Devolvemos los bytes escritos.
    return bytesEscritos;
}

/******************************************************************************
* Método: mi_read_f()
* Descripción:  Lee información de un fichero/directorio correspondiente al 
*               número de inodo pasado como argumento y lo almacena en el buffer
*               de memoria, buf_original.
* Parámetros:   ninodo        : Número de inodo correspondiente.
*               *buf_original : Buffer donde se almacenarán los datos.
*               offset        : Posición de lectura inicial.
*               nbytes        : Número de bytes que se deben leer.
* Devuelve:     Número de bytes leídos.
*              -1 si ha habido algún problema.
******************************************************************************/
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){
    int bytesLeidos = 0;                // Número de bytes leídos
    struct inodo inodo;                 // Estructura inodo
    int r = leer_inodo(ninodo, &inodo); // Leer inodo
    int bytesALeer = nbytes;            // Número de bytes a leer.
    // SI ha habido algún error.
    if(r == -1){
        // Imprimimos el error por la salida estándar de errores.
        fprintf(stderr, "Error my_read_f(): Error al leer el inodo.\n");
        return -1;
    }
    // Si no tenemos los permisos de lectura
    if((inodo.permisos & 4) != 4){
        // Imprimimos el error por la salida estándar de errores.
        fprintf(stderr, "Error my_read_f(): No tiene permisos de lectura.\n");
        return -1;
    }
    // Si el offset es mayor al tamaño del fichero leemos 0 bytes.
    if(offset >= inodo.tamEnBytesLog){
        bytesLeidos = 0;
        return bytesLeidos;
    }
    // Si nos indican leer más de los bytes del fichero, sólo leeremos hasta EOF
    if(offset + nbytes >= inodo.tamEnBytesLog){
        bytesALeer = inodo.tamEnBytesLog - offset;
    } else {
        bytesALeer = nbytes;
    }
    unsigned char buffer[BLOCKSIZE];                // Buffer auxiliar
    memset(buffer, 0, BLOCKSIZE);
    int bfisico;                                    // Bloque físico
    int firstblogico = offset/BLOCKSIZE;            // Primer bloque lógico
    int lastblogico = (offset+bytesALeer-1)/BLOCKSIZE;  // Último bloque lógico
    /**************************************************************************
     *                    SI SÓLO SE VA A LEER UN BLOQUE 
     *************************************************************************/
    if(firstblogico == lastblogico){
        int desp1 = offset % BLOCKSIZE;             // Desplazamiento en bytes.
        // Calculamos bloque físico en el que trabajaremos.
        bfisico = traducir_bloque_inodo(ninodo,firstblogico,0);
        // Si ha habido algún error.
        if(bfisico == -1){
            // Imprimimos el error por la salida estándar de errores.
            //fprintf(stderr, "Error my_read_f(): Error al traducir bloque inodo.\n");
            //return -1;
            bytesLeidos = bytesALeer;
            return bytesLeidos;
        }
        // Leemos el bloque físico
        r = bread(bfisico, buffer);
        // Si ha habido algún error.
        if(r == -1){
            // Imprimimos el error por la salida estándar de errores.
            fprintf(stderr, "Error my_read_f(): Error al leer el bloque.\n");
            return -1;
        }
        // Compiamos los datos en el bloque con offset
        memcpy(buf_original,buffer + desp1, bytesALeer);
        // Actualizamos la variable que controla el número de bytes escritos.
        bytesLeidos = bytesALeer;
    }
    /**************************************************************************
     *                SI SÓLO SE VA A LEER MÁS DE UN BLOQUE 
     *************************************************************************/
    if(firstblogico != lastblogico){
        // Calculamos el primer bloque físico.
        int primerBF = traducir_bloque_inodo(ninodo, firstblogico, 0);
        /**********************************************************************
         *                      PRIMER BLOQUE A LEER
         *********************************************************************/
        int desp1 = offset % BLOCKSIZE;              // Desplazamiento en bytes
        if(primerBF != -1){
            r = bread(primerBF, buffer);
            // Si ha habido algún error.
            if(r == -1){
                // Imprimimos el error por la salida estándar de errores.
                fprintf(stderr, "Error my_read_f(): Error al leer el bloque.\n");
                return -1;
            }
            // Copiamos la primera parte de los datos en el primer bloque con offset
            memcpy(buf_original,buffer + desp1, BLOCKSIZE-desp1);  
            // Actualizamos los bytes leídos.
        }
        bytesLeidos = BLOCKSIZE - desp1;
        /**********************************************************************
         *                    BLOQUES INTERMEDIOS A LEER
         *********************************************************************/
        // Recorremos los bloques lógicos.
        for(int i = firstblogico+1; i < lastblogico; i++){
            // Calculamos el bloque físico.
            bfisico = traducir_bloque_inodo(ninodo,i,0);
            if(bfisico != -1){
                // Leemos todos los datos, ya que no debemos preservar nada.
                r = bread(bfisico, buf_original + (BLOCKSIZE - desp1) + (i - firstblogico - 1)*BLOCKSIZE);
                if(r == -1){
                    // Imprimimos el error por la salida estándar de errores.
                    fprintf(stderr, "Error my_read_f(): Error al leer el bloque.\n");
                    return -1;
                }
            } 
            // Actualizamos los bytes leídos.
            bytesLeidos += BLOCKSIZE;
        }
        /**********************************************************************
         *                       ÚLTIMO BLOQUE A LEER
         *********************************************************************/
        // Calculamos último bloque físico.
        int ultimoBF = traducir_bloque_inodo(ninodo,lastblogico,0);
        r = bread(ultimoBF, buffer);
        if(r == -1){
            // Imprimimos el error por la salida estándar de errores.
            fprintf(stderr, "Error my_read_f(): Error al leer el bloque.\n");
            return -1;
        }
        // Calculamos los bytes que hemos escrito.
        int bytesUltimos = (offset + bytesALeer -1)%BLOCKSIZE;
        // Actualizamos el buffer con los datos a leer.
        memcpy( buf_original+(BLOCKSIZE - desp1)+(lastblogico-firstblogico-1)*BLOCKSIZE,
                buffer,
                bytesUltimos+1);
        // Actualizamos los bloques escritos.
        bytesLeidos += bytesUltimos+1;
    }
    // Actualizamos fecha de acceso.
    inodo.atime = time(NULL);
    // Escribimos el inodo.
    r = escribir_inodo(ninodo, inodo);
    if(r == -1){
        // Imprimimos el error por la salida estándar de errores.
        fprintf(stderr, "Error my_writef(): Error al escribir el inodo.\n");
        return -1;
    }
    // Devolvemos número de bytes leídos.
    return bytesLeidos;
}

/******************************************************************************
* Método: mi_stat_f()
* Descripción:  Devuelve la metainformacióm de un fichero/directorio 
*               correspondiente al número de inodo pasado como argumento.
* Parámetros:   ninodo  : Número de inodo
*               *p_stat : Puntero a struct STAT
* Devuelve:      0 : Ha funcionado bien
*               -1 : Ha habido algún error.
******************************************************************************/
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){
	struct inodo inodo;                     // Estructura inodo auxiliar
    int r = leer_inodo(ninodo, &inodo);     // Leemos el inodo.
    // Si ha habido algún error.
    if(r == -1){
        // Imprimimos el error por la salida estándar de errores.
        fprintf(stderr, "Error my_stat_f(): Error al leer el inodo.\n");
        return -1;
    }
    // Devolvemos meta información por referencia. 
    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->atime = inodo.atime;
    p_stat->mtime = inodo.mtime;
    p_stat->ctime = inodo.ctime;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
    // Todo ha ido bien.
    return 0;
}

/******************************************************************************
* Método:  mi_chmod_f()
* Descripción:  Cambia los permisos de un fichero/directorio correspondiente al 
*               número de inodo pasado como argumento según indique el argumento.
* Parámetros:   ninodo   : Número de inodo
*               permisos : Permisos
* Devuelve:      0 : Ha funcionado bien
*               -1 : Ha habido algún error.
******************************************************************************/
int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
	struct inodo inodo;                     // Estructura inodo auxiliar.
    int r = leer_inodo(ninodo, &inodo);     // Leemos el inodo.
    // Si ha habido algún error.
    if(r == -1){
        // Imprimimos el error por la salida estándar de errores.
        fprintf(stderr, "Error mi_chmod_f(): Error al leer el inodo.\n");
        return -1;
    }
    inodo.permisos = permisos;              // Cambiamos permisos
    inodo.ctime = time(NULL);               // Cambiamos ctime.
    r = escribir_inodo(ninodo, inodo);
    if(r == -1){
        // Imprimimos el error por la salida estándar de errores.
        fprintf(stderr, "Error mi_chmod_f(): Error al escribir el inodo.\n");
        return -1;
    }
    return 0;
}