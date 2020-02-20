#include "bloques.h"

#define DISCO argv[1]
#define NUM_BLOQUES atoi(argv[2])

int main(int argc, char **argv){
    if(!argv[1] || !argv[2]){
        fprintf(stderr, "Argumentos esperados: <nombre_dispositivo> <nbloques>");
        return EXIT_FAILURE;
    }
    bmount(DISCO);
    unsigned char buf[BLOCKSIZE];
    memset(buf, 0, BLOCKSIZE);
    for(unsigned int i=0; i<NUM_BLOQUES; i++){
        bwrite(i, buf);
    }
    bumount();
}