
#include <string.h>
#include "ficheros.h"


#define ERROR_EXTRAER_CAMINO  -10 // (en negrita)  identificador del error en extraer_camino
#define ERROR_PERMISO_LECTURA -11 // (en negrita)
#define ERROR_NO_EXISTE_ENTRADA_CONSULTA -12
#define ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO -13 //(en negrita)
#define ERROR_PERMISO_ESCRITURA -14 // (en negrita)
#define ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO -15 //
#define ERROR_ENTRADA_YA_EXISTENTE -16
#define ERROR_LIBERANDO_INODO -17
#define ERROR_LECTURA_ENTRADA -18

struct entrada {
    char nombre[60]; 
    //En el SF ext2 la longitud del nombre es 255
    unsigned int ninodo;
    };

int extraer_camino(const char *camino, char *inicial, char *final);
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar,      unsigned char permisos);
int mi_creat(const char *camino, unsigned char permisos);
int mi_dir(const char *camino, char *buffer);
int mi_chmod(const char *camino, unsigned char permisos);
int mi_link(const char *camino1, const char *camino2);
int mi_unlink(const char *camino);
