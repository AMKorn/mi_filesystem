 #include "ficheros_basico.h"

//static struct superbloque SB;

//Nivel 2
int tamMB(unsigned int nbloques){
    int mod = (nbloques/8)%BLOCKSIZE;
    if (mod!=0) return ((nbloques/8)/BLOCKSIZE)+1;
    return (nbloques/8)/BLOCKSIZE;
}
int tamAI(unsigned int ninodos){
    int tam = (ninodos * INODOSIZE) / BLOCKSIZE;
    int mod = (ninodos * INODOSIZE) % BLOCKSIZE;
    if (mod!=0) tam += 1;
    return tam; //no estoy del todo seguro
}


int initSB(unsigned int nbloques, unsigned int ninodos){
   struct superbloque SB;
   //int posSB = 0; int tamSB = 1;
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

//Hay que implementar Nivel 3 escribir_bit
//Revisar (copiado)
int initMB(){

   struct superbloque SB;
   if(bread(0, &SB) == EXIT_FAILURE) return EXIT_FAILURE;
   unsigned char buffer[BLOCKSIZE]; 
   memset(buffer, 0, BLOCKSIZE);
   int pos = SB.posPrimerBloqueMB;
   while(buffer[pos]){
      bwrite(pos, &buffer[pos]);
      pos++;
   }
   return EXIT_SUCCESS;
}

//VERSION COPIADA N9
int initAI(){
    // Declaración de array de inodos par luego escribirlos.
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    // Declaración del superbloque para coger datos.
    struct superbloque SB;
    // Leemos el superbloque y lo almacenamos en SB
    int r = bread(posSB, &SB);
    // Si ha habido algún error.
    if(r == -1){
        // Imprimimos el error por la salida estándar de errores.
        fprintf(stderr, "Error: Lectura SuperBloque incorrecta.\n");
        return r;
    }
    // Contador de Inodo para hacer el enlace entre Inodos.
    int contInodos = SB.posPrimerInodoLibre + 1;
    // Recorremos los bloques que contienen el Array de Inodos.
    for(int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI;i++)
    {        
        // Recorremos cada Inodo dentro del bloque
        for(int j = 0; j < BLOCKSIZE / INODOSIZE ; j++)
        {
            // Asignamos el tipo 'libre'
           inodos[j].tipo = 'l'; // libre
           // Si no hemos llegado al último inodo
           if(contInodos < SB.totInodos){
                // Lo enlazamos con el siguiente.
			    inodos[j].punterosDirectos[0] = contInodos;
				contInodos++;
           }
           else {
                // El último elemento no se puede enlazar con nada.
				inodos[j].punterosDirectos[0] = UINT_MAX;
                j = BLOCKSIZE/INODOSIZE;
           }
        }
        // Escribimos el grupo de inodos en el Array de Inodos,
        r = bwrite(i, inodos);
        // SI ha habido algun error.
        if (r == -1) {
            // Imprimimos el error por la salida estándar de errores.
            fprintf(stderr, "Error: Escritura Inodos del bloque incorrecta.\n");
            return r;     
        }  
    }
    // Si hemos llegado hasta aquí es que ha ido todo correctamente.
    return 0;
}


//Reviasar (copiado)
/*
int initAI(){
   struct superbloque SB;
   struct inodo inodos[BLOCKSIZE/INODOSIZE];
   bread(SB.posPrimerBloqueAI, inodos);
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
   struct inodo inodos[BLOCKSIZE/INODOSIZE];
   int pos = SB.posPrimerBloqueAI;
   for(int i=0; i<sizeof(inodos)-1; i++){
      inodos[pos].punterosDirectos[0] = i;
      pos++;
   }
   inodos[pos].punterosDirectos[0] = UINT_MAX;
   
  return EXIT_SUCCESS;
}*/

//Nivel 3
/*
Esta función escribe el valor indicado por el parámetro bit: 0 (libre) ó 1 (ocupado) en un determinado bit del MB que representa el bloque nbloque.
Dado un nº de bloque físico, nbloque, del que queremos indicar si está libre o no, primeramente deberemos averiguar donde se ubica su bit
correspondiente en el MB y luego en el dispositivo (nº de bloque físico) para grabarlo cuando le hayamos dado el valor deseado.
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
   unsigned char mascara = 128; // 10000000
   mascara >>= posbit; // desplazamiento de bits a la derecha

   if (bit == 1){
      bufferMB[posbyte] |= mascara; // operador OR para bits
   } else { //Nos hemos asegurado de que solo pueda ser 0 o 1, por lo que la unica otra opcion es el 0;
      bufferMB[posbyte] &= ~mascara; // operadores AND y NOT para bits
   }

   if(bwrite(nbloqueabs,bufferMB)<0){
      return EXIT_FAILURE;
   }

	return EXIT_SUCCESS;

   /* PENDIENTE
   Ahora que ya disponemos de la función escribir_bit() podemos modificar la función initMB() para poner a 1 en el MB los bits que 
   corresponden a los bloques que ocupa el superbloque, el propio MB, y el array de inodos. Eso implicará también actualizar la 
   cantidad de bloques libres en el superbloque y grabarlo.
   */
}

/*
Lee un determinado bit del MB y devuelve el valor del bit leído.
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
   unsigned char mascara = 128;     // 10000000
   mascara >>= posbit;         // desplazamiento de bits a la derecha
   mascara &= bufferMB[posbyte];     // operador AND para bits
   mascara >>= (7-posbit);         // desplazamiento de bits a la derecha

	return mascara;
}

/*
Encuentra el primer bloque libre, consultando el MB, lo ocupa (con la ayuda de la función escribir_bit()) y devuelve su posición.
*/
int reservar_bloque(){
	struct superbloque SB;
   //Comprobamos si hay algun error en la lectura
   if(bread(posSB,&SB)<0){
      return EXIT_FAILURE;
   }
   //Comprobamos que queden bloques libres
   if(SB.cantBloquesLibres<=0){              //PODRIA PONERSE == 0
      //No quedan bloques libres.
      return EXIT_FAILURE;
   }

   int posBloqueMB = SB.posPrimerBloqueMB;
   unsigned char bufferMB[BLOCKSIZE];

   //Buscar el primer bloque libre
   unsigned char bufferAux[BLOCKSIZE];
   memset(bufferAux,255,BLOCKSIZE);
   if(bread(posBloqueMB, bufferMB)<0){
		return EXIT_FAILURE;
   }
   while(memcmp(bufferMB, bufferAux,BLOCKSIZE)!=0){   //Que tengan el mismo tamaño implica que tiene todos los bits a 1
      posBloqueMB++;
      //Comprobamos si nos hemos pasado del rango
      if (posBloqueMB>SB.posUltimoBloqueMB) return EXIT_FAILURE; 
      if(bread(posBloqueMB, bufferMB)<0){
		   return EXIT_FAILURE;
      }
	}

   //Localizar el primer Byte libre
   int posbyte=0;
   while(bufferMB[posbyte]!=255){
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

   int nbloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE+ posbyte) * 8 + posbit;
   //Indicar que esta reservado
   escribir_bit(nbloque, 1);
   
   //Decrementamos los bloques libres
   SB.cantBloquesLibres--;
   if(bwrite(posSB,&SB)<0){
		return EXIT_FAILURE;
   }

   //Limpiamos la el bloque reservado
   memset(bufferAux, 0, BLOCKSIZE);
   if(bwrite(nbloque,bufferAux)<0){
		return EXIT_FAILURE;
   }

	return nbloque;
}
/*
Libera un bloque determinado (con la ayuda de la función escribir_bit()).
*/
int liberar_bloque(unsigned int nbloque){
   struct superbloque SB;
   //Comprobamos si hay algun error en la lectura
   if(bread(posSB,&SB)<0){
      return EXIT_FAILURE;
   }

   //Marcamos el bloque como libre
   if(escribir_bit(nbloque,0)<0){
      //Error al escribir el bit
      return EXIT_FAILURE;
   }

   //Incrementamos los bloques libres
   SB.cantBloquesLibres++;
   if(bwrite(posSB,&SB)<0){
		return EXIT_FAILURE;
   }

   /*
   unsigned char bufferAux[BLOCKSIZE];
   memset(bufferAux, 0, BLOCKSIZE);
*/

	return nbloque;
}

/*
Lee un determinado inodo del array de inodos para volcarlo en una variable de tipo struct inodo pasada por referencia.
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

/*
Lee un determinado inodo del array de inodos para volcarlo en una variable de tipo struct inodo pasada por referencia.
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


/*
Encuentra el primer inodo libre (dato almacenado en el superbloque), lo reserva (con la ayuda de la función escribir_inodo()), 
devuelve su número y actualiza la lista enlazada de inodos libres.
*/
int reservar_inodo(unsigned char tipo, unsigned char permisos){
   struct superbloque SB;
   //Comprobamos si hay algun error en la lectura
   if(bread(posSB,&SB)<0){
      return EXIT_FAILURE;
   }
   //Comprobamos si hay inodos libres
   if(SB.cantInodosLibres<=0){
      //No hay inodos libres
      return EXIT_FAILURE;
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
      return EXIT_FAILURE;
   }
   //Actualizamos el superbloque
   SB.cantInodosLibres--;
   if(bwrite(posSB, &SB)<0){
      return EXIT_FAILURE;
   }

   return posInodoReservado;
}
