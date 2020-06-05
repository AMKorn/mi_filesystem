// * bloques.c *
#include "bloques.h"
#include "semaforo_mutex_posix.h"


static int descriptor = 0;
static sem_t *mutex;
static unsigned int inside_sc = 0; //Para evitar que un wait se haga dos veces


/**
 * Abre el archivo pasado por parámetro o lo crea si es necesario.
 * @param camino - string con la dirección al camino que abrir
 * @return el descriptor de fichero del fichero abierto.
 */
int bmount(const char *camino){
    if(descriptor>0){
        close(descriptor);
    }
    umask(000);
    if((descriptor = open(camino, O_RDWR|O_CREAT, 0666)) == -1){
        fprintf(stderr, "Error: bloques.c -> bmount() -> open()\n");    
    }
    if (!mutex) { //mutex == 0
    //El semáforo es único y sólo se ha de inicializar una vez en nuestro sistema (lo hace el padre)
        mutex = initSem();  //Inicializar semaforo
		if (mutex == SEM_FAILED) {
           return -1;
        }
	}
    return descriptor;
}

/**
 * Cierra el fichero.
 * @return 0 en éxito, -1 en error.
 */
int bumount(){
    descriptor = close(descriptor); 
    // Hay que asignar el resultado de la operación a la variable ya que bmount() la utiliza
    if (descriptor == -1) {
        fprintf(stderr, "Error: bloques.c → bumount() → close(): %d: %s\n", errno, strerror(errno));
    return -1;
    }
   deleteSem(); // borramos semaforo 
   return 0;

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


void mi_waitSem(){
    if (!inside_sc) {
        waitSem(mutex);
    }
    inside_sc++;
}

void mi_signalSem() {
    inside_sc--;
    if (!inside_sc) {
        signalSem(mutex);
    }
}
