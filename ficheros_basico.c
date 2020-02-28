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
   SB.posPrimerBloqueMB = posSB+tamSB;
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
   bwrite(posSB, &SB);
   return EXIT_SUCCESS;
}

int initMB(){
   unsigned char buffer[BLOCKSIZE]; 
   memset(buffer, 0, BLOCKSIZE);
   int pos = SB.posPrimerBloqueMB;
   while(buffer[pos]){
      bwrite(pos, &buffer[pos]);
      pos++;
   }
   return EXIT_SUCCESS;
}
int initAI(){
   struct inodo inodos[BLOCKSIZE/INODOSIZE];
   int contInodos = SB.posPrimerInodoLibre+1;
   for(int i=SB.posPrimerBloqueAI; i<=SB.posUltimoBloqueAI; i++){
      for(int j=0; j<BLOCKSIZE/INODOSIZE; j++){
         inodos[j].tipo = 'l';
         if(contInodos<SB.totInodos){
            inodos[j].punterosDirectos[0] = contInodos;
            contInodos++;
         } else {
            inodos[j].punterosDirectos[0] = UINT_MAX;
         }
      }
      bwrite(i, &inodos[i]);
   }
   /*
   struct inodo inodos[BLOCKSIZE/INODOSIZE];
   int pos = SB.posPrimerBloqueAI;
   for(int i=0; i<sizeof(inodos)-1; i++){
      inodos[pos].punterosDirectos[0] = i;
      pos++;
   }
   inodos[pos].punterosDirectos[0] = UINT_MAX;
   */
  return EXIT_SUCCESS;
}