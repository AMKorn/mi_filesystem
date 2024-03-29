/* Autores:
 * Andreas Manuel Korn
 * Sergio Vega García
 * Rubén López Babón
 */

#include "ficheros_basico.h"
//Nivel 2

/**
 * Calcula el tamaño en bloques necesario para el mapa de bits.
 * @param nbloques - el número de bloques.
 * @return el tamaño del mapa de bits.
 */
int tamMB(unsigned int nbloques){
   int tam = (nbloques/8) / BLOCKSIZE;
   int mod = (nbloques/8) % BLOCKSIZE;
   if (mod!=0) tam += 1;
   return tam;
}

/**
 * Calcula el tamaño del array de inodos.
 * @param ninodos - el número de inodos.
 * @return el tamaño del array de inodos.
 */
int tamAI(unsigned int ninodos){
   int tam = (ninodos * INODOSIZE) / BLOCKSIZE;
   int mod = (ninodos * INODOSIZE) % BLOCKSIZE;
   if (mod!=0) tam += 1;
   return tam;
}

/**
 * Inicializa el superbloque.
 * @param nbloques - el número de bloques
 * @param ninodos - el número de inodos
 * @return EXIT_SUCCESS
 */
int initSB(unsigned int nbloques, unsigned int ninodos){
   struct superbloque SB;
   SB.posPrimerBloqueMB = tamSB + posSB;
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

/**
 * Inicializa el mapa de bits
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */
int initMB(){
   struct superbloque SB;
   if(bread(0, &SB) == -1){
      return EXIT_FAILURE;
   } 
   unsigned char buffer[BLOCKSIZE]; 
   memset(buffer, 0, BLOCKSIZE);
   for(int i = SB.posPrimerBloqueMB; i <= SB.posUltimoBloqueMB; i++){
      if(bwrite(i, buffer) == -1){
         return EXIT_FAILURE;
      }
   }
   // Actualizamos el MB con los datos que ocupan SB, MB y AI
   int i = posSB;
   for(; i <= SB.posUltimoBloqueAI; i++){
      escribir_bit(i,1);          
   }
   SB.cantBloquesLibres = SB.cantBloquesLibres-i;
   //Lo sobreescribimos
   if(bwrite(posSB, &SB) == -1){
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
}

/**
 * Inicializa el array de inodos
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */
int initAI() {
   struct superbloque SB;
   struct inodo inodos[BLOCKSIZE/INODOSIZE];
   if(bread(posSB, &SB)==-1){
      return EXIT_FAILURE;
   }
   int contInodos = SB.posPrimerInodoLibre+1;
   
   //Si hemos inicializado SB.posPrimerInodoLibre = 0
   for (int i=SB.posPrimerBloqueAI; i<=SB.posUltimoBloqueAI; i++) {
      for (int j=0; j<BLOCKSIZE / INODOSIZE; j++) {
         inodos[j].tipo = 'l';  //libre
         if (contInodos < SB.totInodos) {
            inodos[j].punterosDirectos[0] = contInodos;
            contInodos++;
         }else{ //Hemos llegado al último inodo
            inodos[j].punterosDirectos[0] = UINT_MAX;
            //Hay que salir del bucle, el último bloque no tiene por qué estar completo
            j = BLOCKSIZE/INODOSIZE; //Forzamos el cierre del for
         }
      }
      if(bwrite(i, &inodos)==-1){
         return EXIT_FAILURE;
      }   
   }
   return EXIT_SUCCESS;
}

//Nivel 3

/**
 * Esta función escribe el valor indicado por el parámetro bit: 0 (libre) ó 1 (ocupado) en un determinado bit del MB que representa el bloque nbloque.
 * Dado un nº de bloque físico, nbloque, del que queremos indicar si está libre o no, primeramente deberemos averiguar donde se ubica su bit
 * correspondiente en el MB y luego en el dispositivo (nº de bloque físico) para grabarlo cuando le hayamos dado el valor deseado.
 * @param nbloque - número del bloque que representa el bit a cambiar
 * @param bit - o (libre) ó 1 (ocupado)
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */
int escribir_bit(unsigned int nbloque, unsigned int bit){
   if ((bit!=0) && (bit!=1)) {
      //Error bit no valido
      return EXIT_FAILURE; 
   }
   struct superbloque SB;

   //Leer el superbloque para obtener la localización del MB
   if(bread(posSB, &SB)<0){
      return EXIT_FAILURE;
   }
	
   int posbyte = nbloque/8;
   int posbit = nbloque%8;
   int nbloqueMB = posbyte/BLOCKSIZE;
   int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;

   unsigned char bufferMB[BLOCKSIZE];  
   if(bread(nbloqueabs, bufferMB)<0){
		return EXIT_FAILURE;
	}

   posbyte = posbyte % BLOCKSIZE;
   unsigned char mascara = 128;  // 10000000
   mascara >>= posbit;           // Desplazamiento de bits a la derecha

   if (bit == 1){
      bufferMB[posbyte] |= mascara;    // Operador OR para bits
   } else {                            // Nos hemos asegurado de que solo pueda ser 0 o 1, por lo que la unica otra opcion es el 0;
      bufferMB[posbyte] &= ~mascara;   // Operadores AND y NOT para bits
   }

   if(bwrite(nbloqueabs,bufferMB)<0){
      return EXIT_FAILURE;
   }

	return EXIT_SUCCESS;
}

/**
 * Lee un determinado bit del MB y devuelve el valor del bit leído.
 * @param nbloque - número del bloque al que representa el bit a leer
 * @return 0 (libre) ó 1 (ocupado)
 */
unsigned char leer_bit(unsigned int nbloque){
	struct superbloque SB;

   //Leer el superbloque para obtener la localización del MB
   if(bread(posSB, &SB)<0){
      return EXIT_FAILURE;
   }
	
   int posbyte = nbloque/8;
   int posbit = nbloque%8;
   int nbloqueMB = posbyte/BLOCKSIZE;
   int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;

   unsigned char bufferMB[BLOCKSIZE];  
   if(bread(nbloqueabs, bufferMB)<0){
		return EXIT_FAILURE;
	}

   posbyte = posbyte % BLOCKSIZE;
   unsigned char mascara = 128;  // 10000000
   mascara >>= posbit;           // Desplazamiento de bits a la derecha
   mascara &= bufferMB[posbyte]; // Operador AND para bits
   mascara >>= (7-posbit);       // Desplazamiento de bits a la derecha
   printf("nbloque:%u, posbyte:%u, posbit:%u, nbloqueMB:%u, nbloqueabs:%u]\n",nbloque,posbyte,posbit,nbloqueMB,nbloqueabs);

	return mascara;
}

/**
 * Encuentra el primer bloque libre, consultando el MB, lo ocupa (con la ayuda de la función escribir_bit()) y devuelve su posición.
 * @return la posición del primer bloque libre
 * */
int reservar_bloque(){
	struct superbloque SB;
   //Comprobamos si hay algun error en la lectura
   if(bread(posSB,&SB)<0){
      fprintf(stderr, "Error: Lectura SB.\n");
      return EXIT_FAILURE;
   }
   //Comprobamos que queden bloques libres
   if(SB.cantBloquesLibres<=0){              //PODRIA PONERSE == 0
      //No quedan bloques libres.
      fprintf(stderr, "Error: No quedan bloques libres.\n");
      return EXIT_FAILURE;
   }

   int posBloqueMB = SB.posPrimerBloqueMB;
   unsigned char bufferMB[BLOCKSIZE];

   //Buscar el primer bloque libre
   unsigned char bufferAux[BLOCKSIZE];
   memset(bufferAux,255,BLOCKSIZE);
   if(bread(posBloqueMB, bufferMB)<0){
      fprintf(stderr, "Error: Lectura del primer bloque libre.\n");
		return EXIT_FAILURE;
   }
   while(memcmp(bufferAux, bufferMB, BLOCKSIZE) <=0){   //Que tengan el mismo tamaño implica que tiene todos los bits a 1
      posBloqueMB++;
      //Comprobamos si nos hemos pasado del rango
      if (posBloqueMB>SB.posUltimoBloqueMB) {
         fprintf(stderr, "Error: Salida de rango.\n");
         return EXIT_FAILURE; 
      }
      if(bread(posBloqueMB, bufferMB)<0){
         fprintf(stderr, "Error: Lectura bloque.\n");
		   return EXIT_FAILURE;
      }
	}


   //Localizar el primer Byte libre
   int posbyte=0;
   while(bufferMB[posbyte]>=255){
      posbyte++;
      //Comprobamos si nos hemos pasado del rango
      if(posbyte>BLOCKSIZE) return EXIT_FAILURE;
   }

   //Localizar el primer bit libre
   unsigned char mascara = 128; // 10000000
   int posbit = 0;
   while (bufferMB[posbyte] & mascara) {     //No me convence mucho esta sentencia quizas mejor asi: while((bufferMB[posbyte] & mascara) != 0){  //De momento lo dejaré como esta en la Documentacion
      posbit++;
      bufferMB[posbyte] <<= 1; // desplaz. de bits a la izqda
   }

   int nbloque = ((posBloqueMB - SB.posPrimerBloqueMB)*BLOCKSIZE + posbyte)*8 + posbit;
   //Indicar que esta reservado
   escribir_bit(nbloque, 1);
   
   //Decrementamos los bloques libres
   SB.cantBloquesLibres--;
   if(bwrite(posSB,&SB)<0){
      fprintf(stderr, "Error: Escritura SB.\n");
		return EXIT_FAILURE;
   }

   //Limpiamos la el bloque reservado
   memset(bufferAux, 0, BLOCKSIZE);
   if(bwrite(nbloque,bufferAux)<0){
      fprintf(stderr, "Error: Escritura Bloque.\n");
		return EXIT_FAILURE;
   }

	return nbloque;
}

/**
 * Libera un bloque determinado (con la ayuda de la función escribir_bit()).
 * @return el número del bloque liberado, o -1 si ha habido un error. 
 */ 
int liberar_bloque(unsigned int nbloque){
   struct superbloque SB;
   //Comprobamos si hay algun error en la lectura
   if(bread(posSB,&SB)<0){
      return -1;
   }

   //Marcamos el bloque como libre
   if(escribir_bit(nbloque,0)<0){
      //Error al escribir el bit
      return -1;
   }

   //Incrementamos los bloques libres
   SB.cantBloquesLibres++;
   if(bwrite(posSB,&SB)<0){
		return -1;
   }

	return nbloque;
}

/**
 * Escribe el contenido de una variable de tipo struct inodo en un determinado inodo del array de inodos.
 * @param ninodo - el número de inodo en el que escribir
 * @param inodo - el inodo a escribir en el array de inodos
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */
int escribir_inodo(unsigned int ninodo, struct inodo inodo){
   struct superbloque SB;
   //Comprobamos si hay algun error en la lectura
   if(bread(posSB,&SB)<0){
      return EXIT_FAILURE;
   }
   //Obtenemos el nº de bloque del array de inodos que tiene el inodo solicitado
   int posbloqueinodo = SB.posPrimerBloqueAI + (ninodo/(BLOCKSIZE/INODOSIZE)); //No estoy seguro, quizas sea así:     int nBloque = (ninodo / BLOCKSIZE) + SB.posPrimerBloqueAI;
   struct inodo inodos[BLOCKSIZE/INODOSIZE];
   
   //Leemos el contenido del bloque inodo
   if(bread(posbloqueinodo,&inodos)<0){
      return EXIT_FAILURE;
   }
   //Escribimos el inodo
   inodos[ninodo%(BLOCKSIZE/INODOSIZE)] = inodo;
   if(bwrite(posbloqueinodo,&inodos)<0){
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

/**
 * Lee un determinado inodo del array de inodos para volcarlo en una variable de tipo struct inodo pasada por referencia.
 * @param ninodo - el número de inodo a leer
 * @param inodo - inodo vacío en el que volcar la información
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */
int leer_inodo(unsigned int ninodo, struct inodo *inodo){
   struct superbloque SB;
   //Comprobamos si hay algun error en la lectura
   if(bread(posSB,&SB)<0){
      return EXIT_FAILURE;
   }
   //Obtenemos el nº de bloque del array de inodos que tiene el inodo solicitado
   int posbloqueinodo = SB.posPrimerBloqueAI + (ninodo/(BLOCKSIZE/INODOSIZE)); //No estoy seguro, quizas sea así:     int nBloque = (ninodo / BLOCKSIZE) + SB.posPrimerBloqueAI;
   struct inodo inodos[BLOCKSIZE/INODOSIZE];
   //Leemos el contenido del bloque inodo
   if(bread(posbloqueinodo,&inodos)<0){
      return EXIT_FAILURE;
   }
   *inodo = inodos[ninodo%(BLOCKSIZE/INODOSIZE)];
   return EXIT_SUCCESS;
}

/**
 * Encuentra el primer inodo libre (dato almacenado en el superbloque), lo reserva (con la ayuda de la función escribir_inodo()), 
 * devuelve su número y actualiza la lista enlazada de inodos libres.
 * @param tipo - tipo al que establecer el inodo reservado
 * @param permisos - permisos a establecer en el inodo, con formato rwx
 * return posición del inodo reservado, o -1 si hay error
 */
int reservar_inodo(unsigned char tipo, unsigned char permisos){
   struct superbloque SB;
   //Comprobamos si hay algun error en la lectura
   if(bread(posSB,&SB)<0){
      return -1;
   }
   //Comprobamos si hay inodos libres
   if(SB.cantInodosLibres<=0){
      //No hay inodos libres
      return -1;
   }
   
   //Guardamos el primer inodo libre
   int posInodoReservado = SB.posPrimerInodoLibre;
   //Avanzamos el la posicion del siguiente inodo libre
   SB.posPrimerInodoLibre++;     //REVISAR

   struct inodo ino;
   ino.tipo = tipo;
   ino.permisos = permisos;
   ino.nlinks = 1;
   ino.tamEnBytesLog = 0;
   ino.atime = time(NULL);
   ino.mtime = time(NULL);
   ino.ctime = time(NULL);
   ino.numBloquesOcupados = 0;
   //punteros a bloques directos: 0 
   for(int i = 0; i < 12; i++){
      ino.punterosDirectos[i] = 0;
   }
   //punteros a bloques indirectos: 0 
   for (int i=0; i< 3; i++){
		ino.punterosIndirectos[i]=0;
	}

   //Escribimos el nuevo inodo
   if (escribir_inodo(posInodoReservado,ino)<0){
      return -1;
   }
   //Actualizamos el superbloque
   SB.cantInodosLibres--;
   if(bwrite(posSB, &SB)<0){
      return -1;
   }
   
   return posInodoReservado;
}

//Nivel4

/**
 * Función que obtiene el rango de punteros en el que se sitúa el bloque lógico.
 * @param inodo      - struct inodo cuyo rango se quiere obtener
 * @param nblogico   - unsigned int que indica el bloque lógico
 * @param ptr        - unsigned int[] que indicará los punteros indirectos del inodo.
 * @return           - el rango del bloque lógico, o -1 si hay un error.
 */
int obtener_nrangoBL (struct inodo inodo,  unsigned int nblogico, unsigned int *ptr) { //devolver nrangoBL:ent 
   if (nblogico<DIRECTOS){
      *ptr=inodo.punterosDirectos[nblogico];      
      return 0;
   }                
   else if (nblogico<INDIRECTOS0){
      *ptr=inodo.punterosIndirectos[0];         
      return 1; 
   }                     
   else if (nblogico<INDIRECTOS1){            
      *ptr=inodo.punterosIndirectos[1];             
      return 2;         
   }
   else if (nblogico<INDIRECTOS2){              
      *ptr=inodo.punterosIndirectos[2];                
      return 3;   
   }         
   else {
      *ptr=0;               
      perror("Bloque lógico fuera de rango");         
      return -1; 
   } 
}

/**
 * Función que obtiene los índices de los bloques de punteros.
 * @param nblogico         - unsigned int que indica el número del bloque lógico cuyo índice se quiere obtener.
 * @param nivel_punteros   - unsigned int que ineica el nivel de los punteros.
 * @return                 - los índices de los bloques de punteros, o -1 si hay error.
 */
int obtener_indice(unsigned int nblogico, unsigned int nivel_punteros){
   //Caso 1: Dentro de punteros directos
   if (nblogico<DIRECTOS){    //ej nblogico=8
      return  nblogico; 
   }  

   //Caso 2: Dentro de punteros indirectos 0
   else if (nblogico<INDIRECTOS0){  //ej nblogico=204 
      return nblogico-DIRECTOS;   
   }   

   //Caso 3: Dentro de punteros indirectos 1
   else if (nblogico<INDIRECTOS1){     //ej nblogico=30.004   
      if (nivel_punteros==2){ 
         return (nblogico-INDIRECTOS0) / NPUNTEROS;
      } else if (nivel_punteros==1){ 
         return (nblogico-INDIRECTOS0) % NPUNTEROS; 
      } 
   }

   //Caso 4: Dentro de punteros indirectos 2
   else if (nblogico<INDIRECTOS2) {//ej nblogico=400.004            
      if (nivel_punteros==3){ 
         return (nblogico-INDIRECTOS1)/(NPUNTEROS*NPUNTEROS);               
      }else if (nivel_punteros==2){        
         return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS)) / NPUNTEROS;               
      }else if (nivel_punteros==1){      
         return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS)) % NPUNTEROS;  
      }  
   }
   //ERROR
   return -1;             
}

/**
 * Esta función se encarga de obtener el nº  de bloque físico correspondiente a un bloque lógico determinado del inodo indicado. 
 *
 * reservar = 0 ->   unicamente consulta
 *     Si existe bloque físico de datos, la función devolverá su posición.
 *     Si no existe bloque físico de datos, dará error.
 *
 * reservar = 1 ->   consultar y, si no existe bloque físico, también para reservar
 *     Si existe bloque físico de datos, la función devolverá su posición.
 *     Si no existe bloque físico de datos, lo reservará y se devolverá su posición
 */
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar){
   struct inodo inodo;
   unsigned int ptr, ptr_ant, salvar_inodo, nRangoBL, nivel_punteros, indice;
   int buffer[NPUNTEROS];
   
   if (leer_inodo(ninodo, &inodo)==-1){
      return -1;
   }
   ptr = 0; 
   ptr_ant = 0; 
   salvar_inodo = 0;
   nRangoBL = obtener_nrangoBL(inodo, nblogico, &ptr);   //0:D, 1:I0, 2:I1, 3:I2

   nivel_punteros = nRangoBL; //El nivel_punteros +alto es el que cuelga del inodo
   while (nivel_punteros>0){  //Iterar para cada nivel de indirectos
      if (ptr==0) {  //No cuelgan bloques de punteros
         if (reservar ==0) return -1;  //Error lectura bloque inexistente

         else { //Reservar bloques punteros y crear enlaces desde inodo hasta datos
            salvar_inodo = 1;
            ptr = reservar_bloque();   //De punteros          
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);  //Fecha actual
            if (nivel_punteros == nRangoBL) { 
               //El bloque cuelga directamente del inodo
               inodo.punterosIndirectos[nRangoBL-1] = ptr;

            } else { //El bloque cuelga de otro bloque de punteros
               buffer[indice] = ptr;
               bwrite(ptr_ant, buffer);                
         	}
		   } 
      }
      bread(ptr, buffer);
      indice = obtener_indice(nblogico, nivel_punteros);
      ptr_ant = ptr; //Guardamos el puntero
      ptr = buffer[indice]; // y lo desplazamos al siguiente nivel
      nivel_punteros--;    
   }  //Al salir de este bucle ya estamos al nivel de datos

   if (ptr==0) {  //No existe bloque de datos
      if (reservar==0) return -1;  //Error lectura ∄ bloque   
      else {
         salvar_inodo = 1;
         ptr = reservar_bloque();   //De datos
         inodo.numBloquesOcupados++;
         inodo.ctime = time(NULL);
         if (nRangoBL==0){
         inodo.punterosDirectos[nblogico] = ptr;

         }else {
            buffer[indice] = ptr; 
            bwrite(ptr_ant, buffer);
         }
      }
   }

   if (salvar_inodo == 1) {
      escribir_inodo (ninodo, inodo);  //Sólo si lo hemos actualizado
   }
   return ptr; //nbfisico
}

// Nivel 6

/**
 * Libera el nodo indicado por parámetro.
 * Devuelve el número del inodo liberado.
 */
int liberar_inodo(unsigned int ninodo){
   struct inodo inodo;
   if(leer_inodo(ninodo, &inodo)==EXIT_FAILURE) return -1;
   int liberados = liberar_bloques_inodo(0, &inodo);
   if(liberados == -1) return -1;
   inodo.numBloquesOcupados -= liberados;
   if(inodo.numBloquesOcupados != 0){
      return -1;
   }
   inodo.tipo = 'l';
   inodo.tamEnBytesLog = 0;

   //Actualizar lista enlazada
   struct superbloque SB;
   if(bread(posSB, &SB)==-1) return -1;
   unsigned int primInLibre = SB.posPrimerInodoLibre;
   inodo.punterosDirectos[0] = primInLibre;
   SB.posPrimerInodoLibre = ninodo;
   SB.cantInodosLibres++;

   if(escribir_inodo(ninodo, inodo)==EXIT_FAILURE) return -1;
   if(bwrite(posSB, &SB)==-1) return -1;

   return ninodo;
}

/**
 * Libera todos los bloques ocupados a partir del bloque lógico indicado por parámetro primerBL.
 * Devuelve la cantidad de bloques liberados.
 * Actualmente es muy ineficiente, es necesario mejorarlo.
 */
int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo){
   unsigned int liberados = 0; // Parámetro a devolver
   unsigned int indice;
   int bloques_punteros[3][NPUNTEROS];
   int ptr_nivel[3];
   int indices[3];
   
   // Inicialización de un buffer auxiliar vacío para ayudar a comparar.
   unsigned char buffVacio[BLOCKSIZE];
   memset(buffVacio, 0, BLOCKSIZE);

   if(inodo->tamEnBytesLog==0) return 0;

   int ultimoBL;
   if((inodo->tamEnBytesLog % BLOCKSIZE) == 0){
      ultimoBL = inodo->tamEnBytesLog/BLOCKSIZE-1;
   } else {
      ultimoBL = inodo->tamEnBytesLog/BLOCKSIZE;
   }

   unsigned int ptr = 0;
   for(int nBL = primerBL; nBL <= ultimoBL; nBL++){
      unsigned int nRangoBL = obtener_nrangoBL(*inodo, nBL, &ptr);
      if(nRangoBL<0) return -1;
      unsigned int nivel_punteros=nRangoBL;

      while(ptr>0 && nivel_punteros>0){
         indice = obtener_indice(nBL, nivel_punteros);
         if(indice == 0 || nBL == primerBL){
            bread(ptr, bloques_punteros[nivel_punteros-1]);
         }
         ptr_nivel[nivel_punteros-1]=ptr;
         indices[nivel_punteros-1]=indice;
         ptr=bloques_punteros[nivel_punteros-1][indice];
         nivel_punteros--;
      }

      if(ptr > 0){ // si existe bloque de datos
         liberar_bloque(ptr);
         liberados++;
         if(nRangoBL == 0){ // es un puntero directo
            inodo->punterosDirectos[nBL]=0;
         } else {
            while(nivel_punteros < nRangoBL){
               indice=indices[nivel_punteros];
               bloques_punteros[nivel_punteros][indice]=0;
               ptr=ptr_nivel[nivel_punteros];
               if(memcmp(bloques_punteros[nivel_punteros], buffVacio, BLOCKSIZE)==0){
                  liberar_bloque(ptr);
                  liberados++;
                  nivel_punteros++;
                  if(nivel_punteros == nRangoBL){
                     inodo->punterosIndirectos[nRangoBL-1]=0;
                  }
               } else {
                  bwrite(ptr, bloques_punteros[nivel_punteros]);
                  nivel_punteros = nRangoBL;
               }
            }
         }
      }
   }

   return liberados;
}
