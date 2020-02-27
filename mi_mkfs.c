#include "bloques.h"

#define DISCO argv[1]
#define NUM_BLOQUES atoi(argv[2])

int main(int argc, char **argv){
    if(!argv[1] || !argv[2] || argv[3]){    // If not enough arguments or too many,
        fprintf(stderr, "Argumentos esperados: <nombre_dispositivo> <nbloques>\n"); 
                                            // an error is printed asking for the correct command format
        return EXIT_FAILURE;                // and we finish the program
    }

    if(!bmount(DISCO)){             // We start by mounting the disk.
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));  // if error, we print it
        return EXIT_FAILURE;        // and exit the program.
    }
    unsigned char buf[BLOCKSIZE];   // We prepare space for the information
    memset(buf, 0, BLOCKSIZE);      // and start it full of zeros
    for(unsigned int i=0; i<NUM_BLOQUES; i++){
        if(bwrite(i, buf) == -1) {  // we write each block one by one
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));  // Error treatment
            bumount();
            return EXIT_FAILURE; 
        }
    }
    bumount();  // last we close the disk
    return EXIT_SUCCESS;    // and exit the program
}