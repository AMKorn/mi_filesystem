#include "ficheros_basico.h"

int tamMB(unsigned int nbloques){
   int tam = (nbloques/8)%BLOCKSIZE;
}

int tamAI(unsigned int ninodos){
   int tam = (ninodos*INODOSIZE)%BLOCKSIZE;
}

int initSB(unsigned int nbloques, unsigned int ninodos){
   struct superbloque SB;
   SB.posPrimerBloqueMB = 1;
   SB.posUltimoBloqueMB = SB.posPrimerBloqueMB+tamMB(nbloques)-1;
   SB.posPrimerBloqueAI = SB.posUltimoBloqueMB+1;
   SB.posUltimoBloqueAI = SB.posPrimerBloqueAI+tamAI(ninodos)-1;
   SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI+1;
   SB.posUltimoBloqueDatos = nbloques-1;
   SB.posInodoRaiz = 0;
   SB.posPrimerInodoLibre = 0;
   SB.cantBloquesLibres = nbloques;
   SB.cantInodosLibres = ninodos;
   SB.totBloques = nbloques;
   SB.totInodos = ninodos;
   bwrite()
}

int initMB(){

}
int initAI(){

}