#include "directorios.h"

struct UltimaEntrada UltimaEntradaEscritura;
struct UltimaEntrada UltimaEntradaLectura;

/**
 * Retorna
 * 0: Fichero
 * 1: Directorio
 */
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    //Primero comprobamos que empiece por '/'
    if (camino[0] != '/') return -1;

    int i = 1;  //i será 1 ya que sabemos que el primer caracter es '/'
    int j = 0;
    int tam = strlen(camino); 

    while(camino[i]!= '/' && camino[i]!='\0') {       //while(camino[i]!= '/' || i<tam) {
        inicial[j] = camino[i];
        i++;
        j++;
    }
    //caso de un fichero
    if (i==tam){
        final[0]='\0';
        strcpy(tipo, "f");
    //printf("[FICHERO\n");

        return 0;
    } 
    //caso directorio 
    camino++;  
    camino = strchr(camino, '/'); 
    strcpy(final, camino); //Asignamos a final el contenido de camino a partir de su primer '/' 
    strcpy(tipo, "d");
    //printf("[DIRECTORIO\n");
    return 1;
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos){
    if(strcmp(camino_parcial, "/") == 0){    //Directorio Raiz
        *p_inodo = 0;
        *p_entrada = 0;
        return EXIT_SUCCESS;
    }

    struct entrada entrada;
    struct inodo inodo_dir;
    char tipo;
    //int tamNombre = sizeof(entrada.nombre);
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    memset(inicial, 0, sizeof(inicial)); //previamente inicializar el buffer de lectura con 0s
	memset(final, 0, strlen(camino_parcial));  
     
    memset(entrada.nombre, 0, sizeof(entrada.nombre));
    //int camino;
    if (extraer_camino (camino_parcial, inicial, final, &tipo) ==-1) return ERROR_CAMINO_INCORRECTO; 

    printf("[buscar_entrada()→ inicial: %s, final: %s, reservar: %d]\n", inicial, final, reservar);

    //buscamos la entrada cuyo nombre se encuentra en inicial
    if (leer_inodo(*p_inodo_dir, &inodo_dir)==-1) return ERROR_LECTURA_ESCRITURA;
    if ((inodo_dir.permisos &4) !=4) {
        printf("[buscar_entrada()→ El inodo %d no tiene permisos de lectura]\n", *p_inodo_dir);
        return ERROR_PERMISO_LECTURA;
    } 

    //el buffer de lectura puede ser un struct tipo entrada 
    //o bien un array de las entradas que caben en un bloque, para optimizar la lectura en RAM
    int cant_entradas_inodo = inodo_dir.tamEnBytesLog/sizeof(struct entrada); //cantidad de entradas que contiene el inodo
    
    int num_entrada_inodo = 0;  //nº de entrada inicial


    if (cant_entradas_inodo > 0) {
        if(mi_read_f(*p_inodo_dir,&entrada,num_entrada_inodo*sizeof(struct entrada),sizeof(struct entrada)) < 0) return -8; //Leer entrada
        while ((num_entrada_inodo < cant_entradas_inodo) && (strcmp(inicial,entrada.nombre)!= 0)) { 
            num_entrada_inodo++;
            if(mi_read_f(*p_inodo_dir,&entrada,num_entrada_inodo*sizeof(struct entrada),sizeof(struct entrada)) < 0) return -8; //leer siguiente entrada
        }
    }

    if ((num_entrada_inodo == cant_entradas_inodo) && strcmp(inicial,entrada.nombre)!=0){  
    //la entrada no existe
        switch(reservar){
            case 0:  //modo consulta. Como no existe retornamos error
                return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            case 1:  //modo escritura. 
                //Creamos la entrada en el directorio referenciado por *p_inodo_dir
                //si es fichero no permitir escritura 
                //printf("Tipo1:  %c, Tipo2:  %c\n", inodo_dir.tipo,tipo);
                if (inodo_dir.tipo == 'f'){ 
                    return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
                }
                //si es directorio comprobar que tiene permiso de escritura
                if((inodo_dir.permisos & 2) != 2){
                    return -8;
                } else {
                    //int res;
                    strcpy(entrada.nombre, inicial);                
                    if (tipo == 'd') {  //es un directorio
                        if (strcmp(final, "/") == 0) { 
                            if ((entrada.ninodo = reservar_inodo('d', permisos))<0) return ERROR_RESERVAR_INODO;
                                printf("[buscar_entrada()→ reservado inodo %d tipo %c con permisos %d para %s]\n", entrada.ninodo, tipo, permisos, entrada.nombre);
                                printf("[buscar_entrada()→ creada entrada: %s,  %d]\n",entrada.nombre, entrada.ninodo);
                        } else { //cuelgan más diretorios o ficheros
                            return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                        }
                    }else{ //es un fichero
                        if ((entrada.ninodo = reservar_inodo('f', permisos))<0) return ERROR_RESERVAR_INODO;
                        printf("[buscar_entrada()→ reservado inodo %d tipo %c con permisos %d para %s]\n", entrada.ninodo, tipo, permisos, entrada.nombre);
                        printf("[buscar_entrada()→ creada entrada: %s,  %d]\n",entrada.nombre, entrada.ninodo);      
                    }      
                    if(mi_write_f(*p_inodo_dir, &entrada, num_entrada_inodo*sizeof(struct entrada), sizeof(struct entrada)) == -1){
                        if(entrada.ninodo != -1){
                            liberar_inodo(entrada.ninodo);
                        }
                        return EXIT_FAILURE;
                    }
                }  
                break;
        }
    }

   if((strcmp(final, "/") == 0) || (strcmp(final, "")==0)){
        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar==1)) return ERROR_ENTRADA_YA_EXISTENTE; //modo escritura y la entrada ya existe
        // cortamos la recursividad
        *p_inodo = entrada.ninodo;
        *p_entrada = num_entrada_inodo;
    } else {
        *p_inodo_dir = entrada.ninodo;
        return buscar_entrada (final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos); //Llamada recursiva
    }
    return EXIT_SUCCESS;
}

void mostrar_error_buscar_entrada(int error) {
   // fprintf(stderr, "Error: %d\n", error);
   switch (error) {
   case -1: fprintf(stderr, "Error: Camino incorrecto.\n"); break;
   case -2: fprintf(stderr, "Error: Permiso denegado de lectura.\n"); break;
   case -3: fprintf(stderr, "Error: No existe el archivo o el directorio.\n"); break;
   case -4: fprintf(stderr, "Error: No existe algún directorio intermedio.\n"); break;
   case -5: fprintf(stderr, "Error: Permiso denegado de escritura.\n"); break;
   case -6: fprintf(stderr, "Error: El archivo ya existe.\n"); break;
   case -7: fprintf(stderr, "Error: No es un directorio.\n"); break;
   case -8: fprintf(stderr, "Error: Error en lectura/escritura.\n"); break;
   case -9: fprintf(stderr, "Error: Error reserva inodo.\n"); break;
   }
}

int mi_creat(const char *camino, unsigned char permisos){
    unsigned int p_inodo_dir, p_inodo, p_entrada;
    p_inodo_dir=0;
    char reservar = 1;
    int e = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, permisos);
    if(e != EXIT_SUCCESS){
        mostrar_error_buscar_entrada(e);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int mi_dir(const char *camino, char *buffer, char tipo){
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    char longitud[TAMFILA];
    struct entrada entrada;

    if (buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 6) < 0){
        return -1;
    }

    struct inodo inodo;
    if (leer_inodo(p_inodo, &inodo) == -1){
        perror("Error al leer_inodo en mi_dir");
        return -1;
    }
    if ((inodo.permisos & 4) != 4){
        perror("Sin permisos de lectura");
        return -1;
    }

    int nentrada;
    //Si el inodo es un directorio miramos su contenido
    if (tipo == 'd'){
        nentrada = 0;
        int maxEntradas = inodo.tamEnBytesLog / sizeof(entrada);;
        while (nentrada < maxEntradas){
            //leemos la entrada
            if (mi_read_f(p_inodo, &entrada, nentrada * sizeof(entrada), sizeof(entrada)) < 0){
                perror("Error mi_read_f en mi_dir");
                return EXIT_FAILURE;
            }

            if (entrada.ninodo >= 0){

                struct inodo inodoAux;
                if (leer_inodo(entrada.ninodo, &inodoAux) == -1){
                    return EXIT_FAILURE;
                }

                if (inodoAux.tipo != 'l'){
                    //Tipo
                    if (inodoAux.tipo == 'd'){
                        strcat(buffer, "\x1b[91md\t");
                    } else if (inodoAux.tipo == 'f'){
                        strcat(buffer, "\x1b[92mf\t");
                    }

                    //Permisos
                    if (inodoAux.permisos & 4){
                        strcat(buffer, "r");
                    } else {
                        strcat(buffer, "-");
                    }
                    if (inodoAux.permisos & 2){
                        strcat(buffer, "w");
                    } else {
                        strcat(buffer, "-");
                    }
                    if (inodoAux.permisos & 1){
                        strcat(buffer, "x");
                    } else {
                        strcat(buffer, "-");
                    }
                    strcat(buffer, "\t\t");

                    //mTime
                    struct tm *ts;
                    char mtime[TAMFILA];
                    ts = localtime(&inodoAux.mtime);
                    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
                    strcat(buffer, mtime);
                    strcat(buffer, "\t");

                    //Tamaño
                    memset(longitud, '\0', sizeof(longitud));
                    sprintf(longitud, "%d bytes", inodoAux.tamEnBytesLog);
                    strcat(buffer, longitud);
                    strcat(buffer, "\t\t");

                    //Nombre
                    strcat(buffer, entrada.nombre);
                    strcat(buffer, "\n\x1b[0m");

                    nentrada++;
                }
            }
        }
    } else if (tipo == 'f') {
        nentrada = 1;
        strcat(buffer, "\x1b[92mf\t");

        if (mi_read_f(p_inodo, &entrada, nentrada*sizeof(entrada), sizeof(entrada)) < 0) {
            perror("Error mi_read_f en mi_dir");
            return EXIT_FAILURE;
        }

        //Permisos
        if (inodo.permisos & 4){
            strcat(buffer, "r");
        } else {
            strcat(buffer, "-");
        }
        if (inodo.permisos & 2){
            strcat(buffer, "w");
        } else {
            strcat(buffer, "-");
        }
        if (inodo.permisos & 1){
            strcat(buffer, "x");
        } else {
            strcat(buffer, "-");
        }
        strcat(buffer, "\t\t");

        //mTime
        struct tm *ts;
        char mtime[TAMFILA];
        ts = localtime(&inodo.mtime);
        strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
        strcat(buffer, mtime);
        strcat(buffer, "\t");

        //Tamaño
        memset(longitud, '\0', sizeof(longitud));
        sprintf(longitud, "%d bytes", inodo.tamEnBytesLog);
        strcat(buffer, longitud);
        strcat(buffer, "\t\t");
        
        //Nombre
        char tipo, inicial[strlen(camino)], final[strlen(camino)];
        extraer_camino(camino, inicial, final, &tipo);
        while(tipo!='f'){
            extraer_camino(final, inicial, final, &tipo);
        }
        strcat(buffer, inicial);
        strcat(buffer, "\n\x1b[0m");
    }
    return nentrada;
}

int mi_chmod(const char *camino, unsigned char permisos){
    unsigned int p_inodo_dir, p_inodo, p_entrada;
    p_inodo_dir=0;
    char reservar = 0;
    int e = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, permisos);
    if(e != EXIT_SUCCESS){
        mostrar_error_buscar_entrada(e);
        return EXIT_FAILURE;
    }
    return mi_chmod_f(p_inodo, permisos);
}

int mi_stat(const char *camino, struct STAT *p_stat){
    unsigned int p_inodo_dir, p_inodo, p_entrada;
    p_inodo_dir=0;
    char reservar = 0;
    char permisos = 7;
    int e = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, permisos);
    if(e != EXIT_SUCCESS){
        mostrar_error_buscar_entrada(e);
        return EXIT_FAILURE;
    }

    if(mi_stat_f(p_inodo, p_stat)==EXIT_FAILURE){
        return EXIT_FAILURE;
    }    

    char atime[80];
    char mtime[80];
    char ctime[80];
    struct tm *ts;

    ts = localtime(&p_stat->atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat->mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat->ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    printf("Número de Inodo: %d\n", p_inodo);
    printf("Tipo: %c\n", p_stat->tipo);
    printf("Permisos %d\n", p_stat->permisos);
    printf("atime: %s\n", atime);
    printf("ctime: %s\n", ctime);
    printf("mtime: %s\n", mtime);
    printf("nlinks: %d\n", p_stat->nlinks);
    printf("tamEnBytesLog: %d\n", p_stat->tamEnBytesLog);
    printf("numBloquesOcupados: %d\n", p_stat->numBloquesOcupados);

    return EXIT_SUCCESS;
}

int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes){
    unsigned int p_inodo_dir, p_inodo, p_entrada;
    p_inodo_dir=0;
    char reservar=0;
    char permisos=7;
    // Comprobar caché
    if(strcmp(camino, UltimaEntradaEscritura.camino)==0){
        p_inodo = UltimaEntradaEscritura.p_inodo;
    } else {
        int e = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, permisos);
        if(e != EXIT_SUCCESS){
            mostrar_error_buscar_entrada(e);
            return -1;
        }
        // Actualizar caché
        strcpy(UltimaEntradaEscritura.camino, camino);
        UltimaEntradaEscritura.p_inodo=p_inodo;
    }
    return mi_write_f(p_inodo, buf, offset, nbytes);
}

//Función de directorios.c para leer los nbytes del fichero indicado por camino, a partir del offset pasado por parámetro y copiarlos en un buffer. 
int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes){
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    unsigned char reservar = 0;
    unsigned char permisos = 7; //No se que permisos ponerle, le pondré 7 de momento
    int leidos;
    int error;

    if(strcmp(camino, UltimaEntradaLectura.camino)==0){
        p_inodo = UltimaEntradaLectura.p_inodo;
    } else if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, permisos)) < 0) {
        mostrar_error_buscar_entrada(error);
        return EXIT_FAILURE;
    }
    leidos = mi_read_f(p_inodo, buf, offset, nbytes);
    if(leidos==-1){
        fprintf(stderr,"ERRORR; MI_READ_F");
        return -1;
    }
    return leidos; 
}

int mi_link(const char *camino1, const char *camino2){
    unsigned int p_inodo_dir1, p_inodo_dir2, p_inodo1, p_inodo2, p_entrada1, p_entrada2;
    p_inodo_dir1=0;
    char reservar=0;
    char permisos=7;
    int e = buscar_entrada(camino1, &p_inodo_dir1, &p_inodo1, &p_entrada1, reservar, permisos);
    if(e != EXIT_SUCCESS){
        mostrar_error_buscar_entrada(e);
        return EXIT_FAILURE;
    }
    if((permisos & MASC_WRTE) != MASC_WRTE){
        fprintf(stderr, "El fichero/archivo no tiene permisos de lectura\n");
        return EXIT_FAILURE;
    }
    p_inodo_dir2=0;
    permisos=6;
    reservar=1;
    e = buscar_entrada(camino2, &p_inodo_dir2, &p_inodo2, &p_entrada2, reservar, permisos);
    if(e != EXIT_SUCCESS && e != ERROR_ENTRADA_YA_EXISTENTE){
        mostrar_error_buscar_entrada(e);
        return EXIT_FAILURE;
    }
    // ????
    return 0;
}
