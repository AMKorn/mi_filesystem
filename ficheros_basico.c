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

//Hay que implementar Nivel 3 escribir_bit
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
   unsigned int posMB = SB.posPrimerBloqueMB;
   unsigned int posbyte = nbloque/8;
   unsigned int posbit = nbloque%8;
   unsigned int nbloqueMB = posbyte/BLOCKSIZE;
   unsigned int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;

   unsigned int bufferMB [BLOCKSIZE];
   posbyte = posbyte % BLOCKSIZE;
   unsigned char mascara = 128;
   mascara >>=posbit;               //Desplazamiento de bits a la derecha
   if(bit==1){
      bufferMB[posbyte] |= mascara;    //Operador OR para bits
   } else {
      bufferMB[posbyte] &= ~mascara;   //Operador AND y NOT para bits
   }
   if(bwrite(nbloqueabs, bufferMB[posbyte])){
      return EXIT_SUCCESS;
   } else {
      return EXIT_FAILURE;
   }

}

unsigned char leer_bit(unsigned int nbloque){
   unsigned int posMB = SB.posPrimerBloqueMB;
   unsigned int posbyte = nbloque/8;
   unsigned int postbit = nbloque%8;
   unsigned int nbloqueMB = posbyte/BLOCKSIZE;
   unsigned int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;
   unsigned int bufferMB [BLOCKSIZE];
   posbyte = posbyte % BLOCKSIZE;

   unsigned char mascara = 128;     // 10000000
   mascara >>= posbit;              // desplazamiento de bits a la derecha
   mascara &= bufferMB[posbyte];    // operador AND para bits
   mascara >>= (7-posbit);          // desplazamiento de bits a la derecha

   return EXIT_SUCCESS;
}

int reservar_bloque(){
   if(SB.cantBloquesLibres>0){
      //unsigned int posBloqueMB = SB.posPrimerBloqueMB;
      char bufferAux[BLOCKSIZE];
      char bufferMB[BLOCKSIZE];
      memset(bufferAux, 255, BLOCKSIZE);

      // Finding first MB position with a 0
      char found = false;
      for(int posBloqueMB = SB.posPrimerBloqueMB; posBloqueMB < SB.totBloques && !found; posBloqueMB++){

         bread(posBloqueMB,bufferMB);
         if(memcmp(bufferAux, bufferMB, BLOCKSIZE) != 0) {
            found = true;
         }
      }

      // Finding the byte with a 0
      int posbyte = 0;
      for(int i = 0; i < BLOCKSIZE && !posbyte; i++){
         if(bufferMB[i]!=255){
            posbyte=i;
         }
      }

      // Finding the first bit with a 0
      unsigned char mascara = 128; // 10000000
      int posbit = 0;
      while (bufferMB[posbyte] & mascara) {  
         posbit++;
         bufferMB[posbyte] <<= 1; // desplaz. de bits a la izqda
      }

      nbloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE+ posbyte) * 8 + posbit;
      escribir_bit(nbloque, 1);
      SB.cantBloquesLibres--;
      //y salvamos el superbloque
      // ???
      //Grabamos un buffer de 0s en la posición del nbloque del dispositivo por si había basura (podría tratarse de un bloque reutilizado por el sistema de ficheros)
      // Tengo que mirar mas detalladamente como se hace esto
      //Devolvemos el nº de bloque que hemos reservado, nbloque
      return nbloque; 
   } else {
      return -1; 
   }
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