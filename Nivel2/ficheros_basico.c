#include "bloques.h"
#include "ficheros_basico.h"

int tamMB(unsigned int nbloques){
    int mod = (nbloques/8)/BLOCKSIZE;
    if (mod!=0) return ((nbloques/8)/BLOCKSIZE)+1;
    return (nbloques/8)/BLOCKSIZE;
}

int tamAI(unsigned int ninodos){
    
    int tarryainodos = (ninodos * INODOSIZE) / BLOCKSIZE;
    int mod = (nbloques/8)/BLOCKSIZE;
    if (mod!=0) tarryainodos += 1;
    return tarryainodos; //no estoy del todo seguro
}

int initSB(unsigned int nbloques, unsigned int ninodos){
struct superbloque SB;
int posSB = 0;
int tamSB = 1;
//Posición del primer bloque del mapa de bits  
SB.posPrimerBloqueMB = posSB + tamSB;
//Posición del último bloque del mapa de bits
SB.posUltimoBloqueMB = SB.posPrimerBloqueMB+tamMB(nbloques)-1;
//Posición del primer bloque del array de inodos
SB.posPrimerBloqueAI = SB.posUltimoBloqueMB+1;
//Posición del último bloque del array de inodos
SB.posUltimoBloqueAI = SB.posPrimerBloqueAI+tamAI(ninodos)-1;
//Posición del primer bloque de datos
SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI+1;
//Posición del último bloque de datos 
SB.posUltimoBloqueDatos = nbloques-1;
//Posición del inodo del directorio raíz en el array de inodos
SB.posInodoRaiz = 0 

//Posición del primer inodo libre en el array de inodos
SB.posPrimerInodoLibre = 0; //Inicialmente ​ 
//Cantidad de bloques libres en el SF
SB.cantBloquesLibres = nbloques;    // Inicialmente
//Cantidad de inodos libres en el array de inodos
SB.cantInodosLibres = ninodos;   // Inicialmente
//Cantidad total de bloques
SB.totBloques = nbloques;
//Cantidad total de inodos
SB.totinodos = ninodos;
}

int initMB(){
    struct superbloque SB;
    int posInicial = SB.posPrimerBloqueMB;
    unsigned char buff[BLOCKSIZE];
    memset(buff,0,BLOCKSIZE);
    for(int i = 0; i<BLOCKSIZE; i++){
        bwrite(posInicial+i,buff[i]);
    }
}

int initAI(){
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE]​;
    //unsigned char buff[BLOCKSIZE];
    int contInodos = SB.posPrimerInodoLibre+1;
    int posInicial = SB.posPrimerBloqueAI;

    for (int i=SB.posPrimerBloqueAI; i<=SB.posUltimoBloqueAI;i++) {
        for (int j=0; j<BLOCKSIZE/INODOSIZE; j++) {
            inodos[j].tipo := ‘l’; ​ //libre
            if (contInodos < SB.totInodos) {
                inodos[j].punterosDirectos[0] := contInodos;
                contInodos++;
            }else{ ​ //hemos llegado al último inodo
                inodos[j].punterosDirectos[0] := ​ UINT_MAX​ ;
                //hay que salir del bucle, el último bloque no tiene por qué estar completo
                return;
            }
        }
        bwrite(posInicial+i,inodos);    //revisar
    }

}
