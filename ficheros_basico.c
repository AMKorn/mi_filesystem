#include "ficheros_basico.h"

static struct superbloque SB;

int tamMB(unsigned int nbloques){
   int tam = (nbloques/8)/BLOCKSIZE;
   if((nbloques/8)%BLOCKSIZE){
      tam++;
   }
   return tam;
}

int tamAI(unsigned int ninodos){
   int tam = (ninodos*INODOSIZE)/BLOCKSIZE;
   if((ninodos*INODOSIZE)%BLOCKSIZE){
      tam++;
   }
   return tam;
}

int initSB(unsigned int nbloques, unsigned int ninodos){
   int posSB = 0; int tamSB = 1;
   SB.posPrimerBloqueMB = posSB + tamSB;
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
   bwrite(posSB, SB);
}

int initMB(){
   unsigned char* buffer = char[BLOCKSIZE]; 
   memset(buffer, 0, BLOCKSIZE);
   int i = tamMB(SB.totBloques);
   for(int j=SB.posPrimerBloqueMB; j<i; j++){
      bwrite(j, buffer);
   }
}
int initAI(){
   struct inodo inodos[BLOCKSIZE/INODOSIZE];
}