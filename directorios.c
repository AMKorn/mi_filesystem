#include "directorios.h"

struct UltimaEntrada UltimaEntradaEscritura;
struct UltimaEntrada UltimaEntradaLectura;

/**
 * Dado un string camino, calcula el directorio inicial, el final y el tipo de directorio.
 * @param camino    - const char[] que incluye el camino a calcular
 * @param inicial   - char[] en el que se almacenará el directorio inicial
 * @param final     - char[] en el que se almacenará el directorio final
 * @param tipo      - puntero a char en el que se almacena si es directorio o fichero.
 * @return 0 si es un fichero o 1 si es un directorio.
 */
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    //Primero comprobamos que empiece por '/'
    if (camino[0] != '/') return -1;

    int i = 1;  //i será 1 ya que sabemos que el primer caracter es '/'
    int j = 0;
    int tam = strlen(camino); 

    while(camino[i]!= '/' && camino[i]!='\0') {
        inicial[j] = camino[i];
        i++;
        j++;
    }
    //caso de un fichero
    if (i==tam){
        final[0]='\0';
        strcpy(tipo, "f");

        return 0;
    } 
    //caso directorio 
    camino++;  
    camino = strchr(camino, '/'); 
    strcpy(final, camino); //Asignamos a final el contenido de camino a partir de su primer '/' 
    strcpy(tipo, "d");
    return 1;
}

/**
 * Busca la entrada dado un camino.
 * @return EXIT_SUCCESS o uno de los tipos de errores definidos para esta función.
 * */
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos){
    if(strcmp(camino_parcial, "/") == 0){    // Se trata del directorio Raiz
        *p_inodo = 0;
        *p_entrada = 0;
        return EXIT_SUCCESS;
    }

    struct entrada entrada;
    struct inodo inodo_dir;
    char tipo;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    memset(inicial, 0, sizeof(inicial)); //previamente inicializar el buffer de lectura con 0s
	memset(final, 0, strlen(camino_parcial));  
     
    memset(entrada.nombre, 0, sizeof(entrada.nombre));
    //int camino;
    if (extraer_camino (camino_parcial, inicial, final, &tipo) ==-1) return ERROR_CAMINO_INCORRECTO; 

    //printf("[buscar_entrada()→ inicial: %s, final: %s, reservar: %d]\n", inicial, final, reservar);

    //buscamos la entrada cuyo nombre se encuentra en inicial
    if (leer_inodo(*p_inodo_dir, &inodo_dir)==-1) return ERROR_LECTURA_ESCRITURA;
    if ((inodo_dir.permisos &4) !=4) {
        //printf("[buscar_entrada()→ El inodo %d no tiene permisos de lectura]\n", *p_inodo_dir);
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

    // Si la entrada no existe
    if ((num_entrada_inodo == cant_entradas_inodo) && strcmp(inicial,entrada.nombre)!=0){
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
                    if (tipo == 'd') {  // si es un directorio
                        if (strcmp(final, "/") == 0) { // Si es el último directorio
                            if ((entrada.ninodo = reservar_inodo('d', permisos))<0) return ERROR_RESERVAR_INODO;
                            //printf("[buscar_entrada()→ reservado inodo %d tipo %c con permisos %d para %s]\n", entrada.ninodo, tipo, permisos, entrada.nombre);
                            //printf("[buscar_entrada()→ creada entrada: %s,  %d]\n",entrada.nombre, entrada.ninodo);
                        } else { // Si cuelgan más diretorios o ficheros
                            return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                        }
                    }else{ // si es un fichero
                        if ((entrada.ninodo = reservar_inodo('f', permisos))<0) return ERROR_RESERVAR_INODO;
                        //printf("[buscar_entrada()→ reservado inodo %d tipo %c con permisos %d para %s]\n", entrada.ninodo, tipo, permisos, entrada.nombre);
                        //printf("[buscar_entrada()→ creada entrada: %s,  %d]\n",entrada.nombre, entrada.ninodo);      
                    }      
                    if(mi_write_f(*p_inodo_dir, &entrada, num_entrada_inodo*sizeof(struct entrada), sizeof(struct entrada)) == -1){
                        if(entrada.ninodo != -1) liberar_inodo(entrada.ninodo);
                        return EXIT_FAILURE;
                    }
                }
                break;
        }
    }

   if((strcmp(final, "/") == 0) || (strcmp(final, "")==0)){
        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar==1)) return ERROR_ENTRADA_YA_EXISTENTE; // si es modo escritura y la entrada ya existe
        // cortamos la recursividad
        *p_inodo = entrada.ninodo;
        *p_entrada = num_entrada_inodo;
    } else {
        *p_inodo_dir = entrada.ninodo;
        return buscar_entrada (final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos); //Llamada recursiva
    }
    return EXIT_SUCCESS;
}

/**
 * Función de impresión de los posibles errores causados por buscar_entrada().
 * */
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

/**
 * Función para crear un directorio o fichero dados unos permisos
 * @param camino    - const char[] que contiene el directorio a crear.
 * @param permisos  - unsigned char que contiene la información en formato rwx en octal.
 * @return          - EXIT_SUCCESS o EXIT_FAILURE
 * */
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

/**
 * Función que imprime los archivos y subdirectorios dentro de un directorio o los datos del fichero.
 * @param camino - const char[] que contiene el directorio o fichero a consultar.
 * @param buffer - char[] donde guardar la información
 * @param tipo   - char que representa si es un directorio o un fichero.
 * @return       - El número de elementos en el directorio, o -1 si hay error.
 * */
int mi_dir(const char *camino, char *buffer, char tipo){
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    int e;

    char longitud[TAMFILA];
    struct entrada entrada;

    if ((e = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 6)) < 0){
        mostrar_error_buscar_entrada(e);
        return -1;
    }

    struct inodo inodo;
    if (leer_inodo(p_inodo, &inodo) == -1){
        fprintf(stderr, "Error al leer_inodo en mi_dir\n");
        return -1;
    }
    if ((inodo.permisos & MASC_READ) != MASC_READ){
        fprintf(stderr, "Sin permisos de lectura\n");
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
                fprintf(stderr, "Error mi_read_f en mi_dir");
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
                    sprintf(longitud, "%d", inodoAux.tamEnBytesLog);
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
            fprintf(stderr, "Error mi_read_f en mi_dir");
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
        sprintf(longitud, "%d", inodo.tamEnBytesLog);
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

/**
 * Función que cambia los permisos de un directorio o fichero.
 * @param camino    - const char[] que incluye el camino cuyos permisos se desean cambiar.
 * @param permisos  - unsigned char que incluye los permisos en formato rwx
 * @return          - EXIT_SUCCESS o EXIT_FAILURE.
 * */
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

/**
 * Función que muestra los datos de un fichero o directorio.
 * @param camino    - const char[] que incluye el camino cuyos datos se quieren mostrar.
 * @param p_stat    - puntero a un struct STAT donde almacenar la información.
 * @return          - EXIT_SUCCESS o EXIT_FAILURE
 * */
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

/**
 * Función que escribe datos en un camino
 * @param camino    - const char[] que incluye la dirección en la que escribir
 * @param buf       - const void[] en el que se almacena la información a escribir.
 * @param offset    - unsigned int que indica el offset con el que escribir.
 * @param nbytes    - unsigned int que indica la cantidad de bytes a escribir.
 * @return          - el número de bytes escritos, o -1 si hay un error.
 * */
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

/**
 * Función de directorios.c para leer los nbytes del fichero indicado por camino, a partir del offset pasado por parámetro y copiarlos en un buffer. 
 * @param camino    - const char[] que incluye la dirección a leer
 * @param buf       - const void[] en el que se almacenará la información leída.
 * @param offset    - unsigned int que indica el offset a partir del cual se ha de leer.
 * @param nbytes    - unsigned int que indica la cantidad de bytes a leer.
 * @return          - el número de bytes leídos, o -1 si hay un error.
 * */
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
        return -1;
    }
    leidos = mi_read_f(p_inodo, buf, offset, nbytes);
    if(leidos==-1){
        fprintf(stderr,"ERROR: MI_READ_F\n");
        return -1;
    }
    return leidos; 
}

/**
 * Función que enlaza dos caminos.
 * @param camino1   - const char[] que incluye el camino original
 * @param camino2   - const char[] que incluye el camino a crear.
 * @return          - EXIT_SUCCESS o EXIT_FAILURE
 * */
int mi_link(const char *camino1, const char *camino2){
    
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    struct entrada entrada;
    if (buscar_entrada(camino1, &p_inodo_dir, &p_inodo, &p_entrada, 0, 6) == -1) {
        return EXIT_FAILURE;
    }

    int ninodo1 = p_inodo;
    struct inodo inodo;
    if (leer_inodo(ninodo1, &inodo) == -1) {
        printf("Error en mi_link(): No se pudo leer el inodo\n");
        return EXIT_FAILURE;
    }
    if ((inodo.permisos & MASC_READ) != MASC_READ) {
        fprintf(stderr, "El inodo no tiene permisos de lectura.\n");
        return EXIT_FAILURE;
    }

    //Comprobamos que camino1 se refiere a un fichero.
    if (inodo.tipo != 'f') {
        printf("Error en mi_link(): Camino 1 debe ser un fichero existente.\n");
        return EXIT_FAILURE;
    }

    // Volvemos a inicializar los parámetros para buscar caminoAux
    p_inodo_dir = 0;
    p_inodo = 0;
    p_entrada = 0;

    if (buscar_entrada(camino2, &p_inodo_dir, &p_inodo, &p_entrada, 1, 6) == ERROR_ENTRADA_YA_EXISTENTE) {
        printf("Error en mi_link(): La entrada ya existe \n");
        return EXIT_FAILURE;
    }

    //Leemos la entrada creada
    if (mi_read_f(p_inodo_dir, &entrada, (p_entrada) * sizeof(entrada), sizeof(entrada)) == -1) {
        printf("Error en mi_link(): Error al leer la entrada %d\n", p_entrada);
        return EXIT_FAILURE;
    }

    liberar_inodo(p_inodo);

    entrada.ninodo = ninodo1;

    if (mi_write_f(p_inodo_dir, &entrada, (p_entrada) * sizeof(entrada), sizeof(entrada)) < 0) {
        printf("Error en mi_link(): Error al escribir la entrada modificada\n");
        return EXIT_FAILURE;
    }

    inodo.nlinks++;

    inodo.ctime = time(NULL);

    if (escribir_inodo(ninodo1, inodo) == -1) {
        printf("Error en mi_link(): Error al escribir el inodo\n");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

/**
 * Función de la capa de directorios que borra la entrada de directorio especificada (no hay que olvidar actualizar la cantidad de enlaces en el inodo) y, 
 * en caso de que fuera el último enlace existente, borrar el propio fichero/directorio.
 * @param camino    - const char[] que incluye el camino a borrar.
 * @return          - EXIT_SUCCESS o EXIT_FAILURE
 * */
int mi_unlink(const char *camino){

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    unsigned int nentradas;
    char reservar=0;
    char permisos=6;
    struct inodo ino;
    struct entrada entrada;

    int e = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, permisos);
    if(e != EXIT_SUCCESS){
        mostrar_error_buscar_entrada(e);
        return EXIT_FAILURE;
    }

    //Si se trata de un directorio  y no está vacío (inodo.tamEnBytesLog > 0) entonces no se puede borrar y salimos de la función. En caso contrario:
    //Mediante la función leer_inodo() leemos el inodo asociado al directorio que contiene la entrada que queremos eliminar (p_inodo_dir), 
    //y obtenemos el nº de entradas que tiene (inodo_dir.tamEnBytesLog/sizeof(struct entrada))
    if(leer_inodo(p_inodo,&ino));
    if(ino.tipo != 'f') {
        fprintf(stderr,"Camino ha de referirse a un directorio, no a un fichero");
        return EXIT_FAILURE;
    }
    //Si la entrada a eliminar es la última (p_entrada ==nº entradas - 1), basta con truncar el inodo a su tamaño menos el tamaño de una entrada, mediante la función mi_truncar_f()
    nentradas = ino.tamEnBytesLog/sizeof(struct entrada);
    if (p_entrada == nentradas-1){
        if (mi_truncar_f(p_inodo_dir, nentradas-sizeof(struct entrada))){
            fprintf(stderr,"Error en truncar");
            return EXIT_FAILURE;
        }
        //Si no es la última entrada, entonces tenemos que leer la última y colocarla en la posición de la entrada que queremos eliminar (p_entrada), 
        //y después ya podemos truncar el inodo como en el caso anterior. 
        //De esta manera siempre dejaremos las entradas de un directorio consecutivas para cuando tengamos que utilizar la función buscar_entrada()
    } else {
        if(mi_read_f(p_inodo_dir, &entrada, (nentradas-1) * sizeof(struct entrada), sizeof(struct entrada)) == -1) return -1;
		if(mi_write_f(p_inodo_dir,&entrada, p_entrada*sizeof(struct entrada), sizeof(struct entrada)) == -1) return -1;
        if (mi_truncar_f(p_inodo_dir,nentradas - sizeof(struct entrada))){
            fprintf(stderr,"Error en truncar");
            return EXIT_FAILURE;
        }
    }
    //Leemos el inodo asociado a la entrada eliminada para decrementar el nº de enlaces
    leer_inodo(p_inodo,&ino);
        ino.nlinks--;
    //Si no quedan enlaces (nlinks) entonces liberaremos el inodo, en caso contrario actualizamos su ctime y escribimos el inodo.
    if(ino.nlinks < 1){
        liberar_inodo(p_inodo);
    } else {
        ino.ctime = time(NULL);
        if(escribir_inodo(p_inodo, ino) < 0){
            fprintf(stderr,"Error al escribir inodo");
            return EXIT_FAILURE; //Error ESCRIBIR_INODO
        }
    }

    return 0;
}


