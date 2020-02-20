// * bloques.c *
#include "bloques.h"

static int descriptor = 0;

int bmount(const char *camino){
    descriptor = open(camino, O_RDWR|O_CREAT, 0666);
    if(descriptor == -1){
        fprintf(sterr, "Error %d: %s", errno, strerror(errno));
    }
    return descriptor;
}

int bumount(){
    return close(descriptor);
}

int bwrite(unsigned int nbloque, const void *buf){
    lseek(descriptor, nbloque*BLOCKSIZE, seek_set);
    return write(descriptor, buf, BLOCKSIZE);
}

int bread(unsigned int nbloque, void *buf){
    lseek(descriptor, nbloque*BLOCKSIZE, seek_set);
    return read(descriptor, buf, BLOCKSIZE);
}