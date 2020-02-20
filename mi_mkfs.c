#include "bloques.c"


int main(int argc, char **argv){
    bmount(argcv[1]);
    unsigned int amount = atoi(argv[2]);
    unsigned char[] *buf = new unsigned char[BLOCKSIZE];
    memset(buf);
    for(unsigned int i=0; i<amount; i++){
        bwrite(i, buf[i]);
    }
    bunmount();
}