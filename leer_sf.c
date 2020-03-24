#include "ficheros_basico.h"

int main(int argc, char **argv){
	if(!argv[1]){
        fprintf(stderr, "Argumentos esperados: <disco>\n");
        return EXIT_FAILURE;
    }
    bmount(argv[1]);
    unsigned int posSB=0;
	struct superbloque SB;
	bread(posSB, &SB);

	printf("DATOS DEL SUPERBLOQUE:\n");
    printf("posPrimerBloqueMB = %u\n", SB.posPrimerBloqueMB);
    printf("posUltimoBloqueMB = %u\n", SB.posUltimoBloqueMB);
    printf("posPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI);
    printf("posUltimoBloqueAI = %d\n", SB.posUltimoBloqueAI);
    printf("posPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);
    printf("posUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);
    printf("posInodoRaiz = %d\n", SB.posInodoRaiz);
    printf("posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);
    printf("cantBloquesLibres = %d\n", SB.cantBloquesLibres);
    printf("canInodosLibres = %d\n", SB.cantInodosLibres);
    printf("totBloques = %d\n", SB.totBloques);
    printf("totInodos = %d\n\n", SB.totInodos);
	
    printf("sizeof struct superbloque: %lu\n", sizeof(struct superbloque));
    printf("sizeof struct inodo: %lu\n", sizeof(struct inodo));

    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    bread(SB.posPrimerInodoLibre+1, &inodos);

/*
    for(unsigned int i = 0; i < SB.cantInodosLibres; i++){
        printf("%u ", inodos->punterosDirectos[0]);
    }
*/

	bumount();
    return EXIT_SUCCESS;
}  