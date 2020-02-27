// * bloques.c *
#include "bloques.h"

static int descriptor = 0;

/**
 * Opens the file passed by parameter or creates it if necessary.
 * @param camino - string with the direction of the path to open.
 * @return the file descriptor of the opened file.
 * */
int bmount(const char *camino){
    umask(000);
    descriptor = open(camino, O_RDWR|O_CREAT, 110);
    if(descriptor == -1){
        fprintf(stderr, "Error %d: %s", errno, strerror(errno));
    }
    return descriptor;
}

/**
 * Closes the file.
 * @return 0 on success, -1 on failure.
 * */
int bumount(){
    return close(descriptor);
}

/**
 * Writes the content pointed to by buf in the specified block.
 * @param nbloque - the block number in which to write.
 * @param buf - the pointer of the information to write
 * @return the number of written bytes, or -1 if error.
 * */
int bwrite(unsigned int nbloque, const void *buf){
    lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
    return write(descriptor, buf, BLOCKSIZE);
}

/**
 * Reads the content in the specified block and stores it in buf.
 * @param nbloque - the block number to read
 * @param buf - the pointer in which to store the information read.
 * @returns the number of read bytes, or -1 if error.
 * */
int bread(unsigned int nbloque, void *buf){
    lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
    return read(descriptor, buf, BLOCKSIZE);
}