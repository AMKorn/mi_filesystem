#include "directorios.h"

int main(int argc, char **argv){
	if(!argv[1]){
        fprintf(stderr, "Argumentos esperados: <disco>\n");
        return EXIT_FAILURE;
    }
    bmount(argv[1]);
    //unsigned int posSB=0;
	struct superbloque SB;
	bread(posSB, &SB);

    printf("\nDATOS DEL SUPERBLOQUE:\n");
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
	
    /*
    printf("sizeof struct superbloque: %lu\n", sizeof(struct superbloque));
    printf("sizeof struct inodo: %lu\n", sizeof(struct inodo));
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    printf("RECORRIDO LISTA ENLAZADA DE INODOS LIBRES\n");
    //El inodo 0 vale 0 en lugar de 1, no se por que, de momento dejo constancia en este comentario
    //Es posible que pueda derivar un error.
    for (int i=SB.posPrimerBloqueAI; i<=SB.posUltimoBloqueAI;i++) {
        if(bread(i, &inodos)==-1){
            return EXIT_FAILURE;
        } 
        for (int j=0; j<BLOCKSIZE / INODOSIZE; j++) {
            printf("%d ",inodos[j].punterosDirectos[0]);
        }  
    }
    */
    
    /*
    printf("RESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS\n");
    int r = reservar_bloque();
    bread(posSB,&SB);
    printf("Se ha reservado el bloque físico nº %u que era el 1º libre indicado por el MB\n", r);
    printf ("SB.cantBloquesLibres = %u\n",SB.cantBloquesLibres);
    liberar_bloque(r);
    bread(posSB,&SB);
    printf("Liberamos ese bloque y después SB.cantBloquesLibres = %u.\n",SB.cantBloquesLibres);

    printf("\n\n");

    printf("MAPA DE BITS CON BLOQUES DE METADATOS OCUPADOS\n");
    unsigned char p;
    printf("leer_bit(%u)→ ",posSB);
    p=leer_bit(posSB);
    printf("valor del bit correspondiente a posSB (o sea al BF nº %u) = %u\n\n",posSB,p);

    printf("leer_bit(%u)→ ",SB.posPrimerBloqueMB);
    p=leer_bit(SB.posPrimerBloqueMB);
    printf("valor del bit correspondiente a posPrimerBloqueMB (o sea al BF nº %u) = %u\n\n",SB.posPrimerBloqueMB,p);

    printf("leer_bit(%u)→ ",SB.posUltimoBloqueMB);
    p=leer_bit(SB.posUltimoBloqueMB);
    printf("valor del bit correspondiente a posUltimoBloqueMB (o sea al BF nº %u) = %u\n\n",SB.posUltimoBloqueMB,p);

    printf("leer_bit(%u)→ ",SB.posPrimerBloqueAI);
    p=leer_bit(SB.posPrimerBloqueAI);
    printf("valor del bit correspondiente a posPrimerBloqueAI (o sea al BF nº %u) = %u\n\n",SB.posPrimerBloqueAI,p);

    printf("leer_bit(%u)→ ",SB.posUltimoBloqueAI);
    p=leer_bit(SB.posUltimoBloqueAI);
    printf("valor del bit correspondiente a posUltimoBloqueAI (o sea al BF nº %u) = %u\n\n",SB.posUltimoBloqueAI,p);

    printf("leer_bit(%u)→ ",SB.posPrimerBloqueDatos);
    p=leer_bit(SB.posPrimerBloqueDatos);
    printf("valor del bit correspondiente a posPrimerBloqueDatos (o sea al BF nº %u) = %u\n\n",SB.posPrimerBloqueDatos,p);

    printf("leer_bit(%u)→ ",SB.posUltimoBloqueDatos);
    p=leer_bit(SB.posUltimoBloqueDatos);
    printf("valor del bit correspondiente a posUltimoBloqueDatos (o sea al BF nº %u) = %u\n\n",SB.posUltimoBloqueDatos,p);

    printf("DATOS DEL DIRECTORIO RAIZ\n");
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];

    struct inodo ino;
    int ninodo = SB.posInodoRaiz;
    leer_inodo(ninodo, &ino);
    printf("tipo: %c\n", ino.tipo);
    printf("permisos: %d\n", ino.permisos);
    ts = localtime(&ino.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&ino.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&ino.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    printf("ATIME: %s\nMTIME: %s\nCTIME: %s\n",atime,mtime,ctime);
    printf("nlinks: %d\n", ino.nlinks);
    printf("tamEnBytesLog: %d\n", ino.tamEnBytesLog);
    printf("numBloquesOcupados: %d\n", ino.numBloquesOcupados);
    */
	/*
    printf("INODO 1. TRADUCCION DE LOS BLOQUES LOGICOS 8, 204, 30.004, 400.004 y 468.750\n");
    int i = reservar_inodo('f',6);
    traducir_bloque_inodo(i,8,1);
    traducir_bloque_inodo(i,204,1);
    traducir_bloque_inodo(i,30004,1);
    traducir_bloque_inodo(i,400004 ,1);
    traducir_bloque_inodo(i,468750 ,1);

    printf("DATOS DEL DIRECTORIO RESERVADO\n");
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];

    struct inodo ino;
    leer_inodo(i, &ino);
    printf("tipo: %c\n", ino.tipo);
    printf("permisos: %d\n", ino.permisos);
    ts = localtime(&ino.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&ino.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&ino.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    printf("ATIME: %s\nMTIME: %s\nCTIME: %s\n",atime,mtime,ctime);
    printf("nlinks: %d\n", ino.nlinks);
    printf("tamEnBytesLog: %d\n", ino.tamEnBytesLog);
    printf("numBloquesOcupados: %d\n", ino.numBloquesOcupados);
    if(bread(posSB,&SB)==-1){
        return -1;
    }
    printf("SB.posPrimerInodoLibre = %d\n",SB.posPrimerInodoLibre);
    */

	bumount();
    return EXIT_SUCCESS;
}  
