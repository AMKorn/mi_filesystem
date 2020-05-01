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
    
    while(camino[i]!= '/' || i<tam{
        incial[j] = camino[i];
        i++;
        j++;
    }

    //caso de un fichero
    if (i==tam){
        final[0]='\0';
        strcpy(tipo, 'f');
    }
    //caso directorio
    camino++;   //Desplazamos en 1 el puntero caracteres de camino
    final = strchr(camino,'/'); //Asignamos a final el contenido de camino a partir de su primer '/'
    strcpy(tipo,'d');    

}




int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos){

}