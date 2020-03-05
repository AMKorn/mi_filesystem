 #include "ficheros_basico.h"

static struct superbloque SB;

//Nivel 2
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

//Nivel 3
int escribir_bit(unsigned int nbloque, unsigned int bit){
<<<<<<< HEAD
   int posMB = SB.posPrimerBloqueMB;
   int posbyte = nbloque/8;
   int posbit = nbloque%8;
   int nbloqueMB = posbyte/BLOCKSIZE;
   int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;
=======
   unsigned int posMB = SB.posPrimerBloqueMB;
   unsigned int posbyte = nbloque/8;
   unsigned int postbit = nbloque%8;
   unsigned int nbloqueMB = posbyte/BLOCKSIZE;
   unsigned int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;
>>>>>>> d450985c67d25fad1073f120421143abe283ba3c

   int bufferMB [BLOCKSIZE];
   posbyte = posbyte % BLOCKSIZE;
   unsigned char mascara = 128;
   mascara >>=posbit;               //Desplazamiento de bits a la derecha
   bufferMB[posbyte] |= mascara;    //Operador OR para bits
   bufferMB[posbyte] &= ~mascara;   //Operador AND y NOT para bits
   bwrite(nbloqueabs, bufferMB[posbyte]);
   return EXIT_SUCCESS;
}

unsigned char leer_bit(unsigned int nbloque){
   unsigned char mascara = 128;     // 10000000
   mascara >>= posbit;              // desplazamiento de bits a la derecha
   mascara &= bufferMB[posbyte];    // operador AND para bits
   mascara >>= (7-posbit);          // desplazamiento de bits a la derecha

   return EXIT_SUCCESS;
}

int reservar_bloque(){
   if(SB.cantBloquesLibres>0){
      unsigned int posBloqueMB = SB.posPrimerBloqueMB;
      bread(posBloqueMB,bufferMB) 
      int bufferAux [BLOCKSIZE];
      memset (bufferAux, 255, BLOCKSIZE)
      if(posBloqueMB){
         
      }
   } else {

   }

   return EXIT_SUCCESS;
}

int liberar_bloque(unsigned int nbloque){
   return EXIT_SUCCESS;
}

int escribir_inodo(unsigned int ninodo, struct inodo inodo){
   return EXIT_SUCCESS;
}

int leer_inodo(unsigned int ninodo, struct inodo *inodo){
   return EXIT_SUCCESS;
}

int reservar_inodo(unsigned char tipo, unsigned char permisos){
   return EXIT_SUCCESS;
}