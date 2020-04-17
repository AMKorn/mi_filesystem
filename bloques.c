// * bloques.c *
#include "bloques.h"

static int descriptor = 0;

/**
 * Abre el archivo pasado por parámetro o lo crea si es necesario.
 * @param camino - string con la dirección al camino que abrir
 * @return el descriptor de fichero del fichero abierto.
 */
int bmount(const char *camino){
    umask(000);
    descriptor = open(camino, O_RDWR|O_CREAT, 0666);
    if(descriptor == -1){
        fprintf(stderr, "Error %d: %s", errno, strerror(errno));
    }
    return descriptor;
}

/**
 * Cierra el fichero.
 * @return 0 en éxito, -1 en error.
 */
int bumount(){
    return close(descriptor);
}

/**
 * Escribe el contenido apuntado por buf en el bloque especificado.
 * @param nbloque - el número de bloque en el que escribir
 * @param buf - el puntero a la información a escribir
 * @return el número de bytes escritos, o -1 si hay error.
 */
int bwrite(unsigned int nbloque, const void *buf){
    lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
    return write(descriptor, buf, BLOCKSIZE);
}

/**
 * Lee el contenido del bloque especificado y lo almacena en buf
 * @param nbloque - el número del bloque a leer
 * @param buf - el puntero a la dirección en la que almacenar la información leída
 * @returns el número de bytes leídos, o -1 si hay error.
 */
int bread(unsigned int nbloque, void *buf){
    lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
    return read(descriptor, buf, BLOCKSIZE);
}