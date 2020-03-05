#include "bloques.h"
static int descriptor = 0;
/*
Llamar a la función open() del sistema para obtener el descriptor del fichero que se usará como dispositivo virtual.
int open(const char *camino, int oflags, mode_t mode);
El nombre del fichero va en el argumento de la función (nos lo proporcionará el administrador del sistema desde 
consola cuando inicialice el sistema de ficheros a través de mi_mkfs.c). 

Podemos abrirlo como O_RDWR|O_CREAT.
Los permisos se representan en octal, por ej: 0666 significa que damos permiso de lectura (r) y escritura (w) a 
usuario, grupo y otros: rw-rw-rw-.

Un proceso o programa tiene asignada una tabla de descriptores de ficheros (nº entero de 0-19, 0: 
entrada estándar, 1: salida estándar, 2: salida de error estándar). La función open() retornará el descriptor del fichero 
(el más bajo libre en la tabla de descriptores) para ser usado en las siguientes operaciones de E/S.
Abrir un fichero es una llamada al sistema y puede producir errores que hay que gestionar. [+Información sobre llamadas al sistema.]
if (descriptor == -1) {
…  //error
}

bmount() devuelve -1 si ha habido error o el descriptor si ha ido bien.
*/  
int bmount(const char *camino){
    umask(000);
    descriptor = open(camino, O_RDWR|O_CREAT, 0666);
    if (descriptor == -1) {
        return -1;
    }
    return 0;
//if (open(camino, 0666, O_RDWR|O_CREAT) )
}

/*
Básicamente llama a la función close() para liberar el descriptor de ficheros.
int close(int descriptor);
La función bumount() devuelve 0 si se ha cerrado el fichero correctamente y -1 en caso contrario.
*/
int bumount(){
//return close(descriptor);
    if(close(descriptor) < 0){
        return -1;
    }
    return 0;
}

/*
Escribe el contenido de un buffer de memoria apuntado por *buf (que tendrá el tamaño de un bloque) en el bloque 
del dispositivo virtual especificado por el argumento nbloque.
Hay que utilizar la función write() precedida de la función lseek() (el desplazamiento será el nº de bloque * tamaño del bloque, 
y se comenzará a contar desde el inicio del fichero: SEEK_SET).
off_t lseek(int descriptor, off_t desplazamiento, int punto_de_referencia);
size_t write(int descriptor, const void *buf, size_t nbytes);
La función bwrite() devuelve el nº de bytes que ha podido escribir o -1 si se produce un error.
*/
int bwrite(unsigned int nbloque, const void *buf){
    off_t offset = lseek(descriptor, nbloque*BLOCKSIZE ,SEEK_SET);
    if(offset==-1) return -1; //error moving the pointer.

    size_t nbytes = write(descriptor, buf, BLOCKSIZE);
    if (nbytes <0) return -1; //the system call returned an error.

    return nbytes;
}

/*
Lee del dispositivo virtual el bloque especificado por nbloque. Copia su contenido en un buffer de memoria apuntado por *buf.
Hay que utilizar la función read() precedida de la función lseek().
size_t read(int descriptor, void *buf, size_t nbytes);
El puntero del fichero quedará indicando el siguiente byte a leer.
La función bread() devuelve el nº de bytes leídos o -1 si se produce un error.

SEEK_SET 	Offset is to be measured in absolute terms.
SEEK_CUR 	Offset is to be measured relative to the current location of the pointer.
SEEK_END 	Offset is to be measured relative to the end of the file.
*/
int bread(unsigned int nbloque, void *buf){
    off_t offset = lseek(descriptor, nbloque*BLOCKSIZE,SEEK_SET);
    if(offset==-1) return -1; //error moving the pointer.

    int nB = read(descriptor, buf, nbloque);
    if (nB < 0) return -1; //the system call returned an error.
    return nB;
/*
    if (size_t read(descriptor, buf, nbloque)==-1) return -1;
    return read;
*/
}