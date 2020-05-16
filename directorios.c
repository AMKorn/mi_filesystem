#include "directorios.h"

/*
Retorna
0: Fichero
1: Directorio
*/
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo){
    //Primero comprobamos que empiece por '/'
    if (camino[0] != '/') return -1;

    int i = 1;  //i será 1 ya que sabemos que el primer caracter es '/'
    int j = 0;
    int tam = strlen(camino);
    
    while(camino[i]!= '/' && camino[i] != '\0'){
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

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos){
       if(strcmp(camino_parcial, "/") == 0){    //Directorio Raiz
        p_inodo = 0;
        p_entrada = 0;
        return EXIT_SUCCESS;
    }

    struct entrada entrada;
    struct inodo inodo_dir;
    char tipo;
    //int tamNombre = sizeof(entrada.nombre);
    char inicial[sizeof(entrada.nombre)];
    char final[sizeof(strlen(camino_parcial))];
    memset(inicial, 0, sizeof(entrada.nombre)); //previamente inicializar el buffer de lectura con 0s
	memset(final, 0, strlen(camino_parcial));   //previamente inicializar el buffer de lectura con 0s
    //int camino;

    if (extraer_camino (camino_parcial, inicial, final, &tipo) ==-1) return ERROR_EXTRAER_CAMINO; 
   
    //buscamos la entrada cuyo nombre se encuentra en inicial
    if (leer_inodo( *p_inodo_dir, &inodo_dir)==-1) return -1;
    if ((inodo_dir.permisos &4) !=4) return ERROR_PERMISO_LECTURA;

    //el buffer de lectura puede ser un struct tipo entrada 
    //o bien un array de las entradas que caben en un bloque, para optimizar la lectura en RAM
    int cant_entradas_inodo = inodo_dir.tamEnBytesLog/sizeof(struct entrada); //cantidad de entradas que contiene el inodo
    int num_entrada_inodo = 0;  //nº de entrada inicial
    if (cant_entradas_inodo > 0) {
        if(mi_read_f(*p_inodo_dir,&entrada,cant_entradas_inodo*sizeof(struct entrada),sizeof(struct entrada)) < 0) return -1; //Leer entrada
        while ((num_entrada_inodo < cant_entradas_inodo) & (inicial != entrada.nombre)) { 
        num_entrada_inodo++;
        if(mi_read_f(*p_inodo_dir,&entrada,cant_entradas_inodo*sizeof(struct entrada),sizeof(struct entrada)) < 0) return -1; //leer siguiente entrada
        }
    }

    if ((num_entrada_inodo == cant_entradas_inodo) && (inicial != entrada.nombre)){  
    //la entrada no existe
        switch(reservar){
            case 0:  //modo consulta. Como no existe retornamos error
                return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            case 1:  //modo escritura. 
                //Creamos la entrada en el directorio referenciado por *p_inodo_dir
                //si es fichero no permitir escritura 
                if (inodo_dir.tipo == 'f'){ 
                    return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
                }
                //si es directorio comprobar que tiene permiso de escritura
                if((inodo_dir.permisos & 2) != 2){
                    return -1;
                } else {
                    strcpy(entrada.nombre, inicial);                
                    if (tipo == 'd') {  //es un directorio
                        if (strcmp(final, "/") == 0) { 
                            entrada.ninodo = reservar_inodo(tipo, permisos);
                        } else { //cuelgan más diretorios o ficheros
                            return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                        }
                    }else{ //es un fichero
                        entrada.ninodo = reservar_inodo('f', permisos);      
                    }      
                    if(mi_write_f(*p_inodo_dir, &entrada, 0, sizeof(struct entrada)) == -1){    //quizas quitar "&"
                        if(entrada.ninodo != -1){
                            liberar_inodo(entrada.ninodo);
                        }
                        return EXIT_FAILURE;
                    }
                }  
                break;
        }
    }

   if(strcmp(final, "/") == 0){
        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar=1)) return ERROR_ENTRADA_YA_EXISTENTE; //modo escritura y la entrada ya existe
        // cortamos la recursividad
        *p_inodo = entrada.ninodo;
        *p_entrada = num_entrada_inodo;
        return EXIT_SUCCESS;
    }else {
        *p_inodo_dir = entrada.ninodo;
        return buscar_entrada (final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos); //Llamada recursiva
    }
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
   case -8: fprintf(stderr, "Error: Error al extraer el camino.\n"); break;
   }
}

int mi_dir(const char *camino, char *buffer){
    return EXIT_SUCCESS; // TO-DO
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
    printf("Número de Inodo: %d\n", p_inodo);
    return mi_stat_f(p_inodo, p_stat);
}