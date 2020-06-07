#include "simulacion.h"
#include <stdio.h>
#include <stdlib.h>

#define DISCO argv[1]

static int acabados = 0;

void reaper(){
  pid_t ended;
  signal(SIGCHLD, reaper);
  while ((ended=waitpid(-1, NULL, WNOHANG))>0) {
    acabados++;
    //Podemos testear qué procesos van acabando:
    //fprintf(stderr, "acabado: %d total acabados: %d\n", ended, acabados);
  }

}

int main(int argc, char const *argv[]) {
    //Comrpobamos sintaxis
    if(argc!=2){
        printf("Sintaxis : ./simulacion <disco>\n");
        return -1;
    }
    
    //Montamos el disco
    if(bmount(DISCO)==-1) {
        fprintf(stderr, "Error de montaje de disco.\n");
        return EXIT_FAILURE;
    }

    struct tm *tm;
    time_t tiempo;
    char dir[100];

    //Creamos el directorio de simulación en la raíz
    time(&tiempo);
    tm = localtime(&tiempo);
    strftime(dir, sizeof(dir), "/simul_%Y%m%d%H%M%S/", tm);
    if(mi_creat(dir,6)<0){
        bumount();
        fprintf(stderr, "Error al montar el disco padre");
        return -1;
    }

    //Asignamos reaper a SIGCHLD
    signal(SIGCHLD, reaper);

    printf("***Simulación de %d procesos realizando cada uno %d escrituras***\n", NUMPROCESOS, NUMESCRITURAS);
    printf("Directorio simulación: %s\n", dir);

    // Guardamos el directorio en un fichero auxiliar 
    FILE *f;
    if ((f = fopen("dir.txt", "w")) == NULL) {
        fprintf(stderr, "Error al guardar el directorio\n");
        exit(1);
    }
    fprintf(f, "%s", dir);
    fclose(f);

    for(int proceso = 1; proceso<=NUMPROCESOS; proceso++){
        if(fork() == 0){

            char dir_pid[100];  

            // Montamos el disco
            bmount(DISCO);
            // Añadimos PID del proceso al nombre del directorio
            char spid[10];
            int pid = getpid();
            sprintf(spid,"%d",pid);
            strcpy(dir_pid,dir);
            strcat(dir_pid,"proceso_");
            strcat(dir_pid,spid);
            strcat(dir_pid, "/");
            if(mi_creat(dir_pid,6)<0){
                bumount();
                perror("Error al montar el disco hijo");
                return -1;
            }
            
            //Creamos el fichero prueba.dat
            strcat(dir_pid,"prueba.dat");
            if(mi_creat(dir_pid,6)<0){
                bumount();
                return -1;
            }

            // Inicializamos la semilla para los números aleatorios
            srand(time(NULL)+getpid());

            for(int i=0; i<NUMESCRITURAS;i++){
                // Inicializamos el registro
                struct REGISTRO registro;
                registro.fecha=time(NULL); 
                registro.pid = getpid(); 
                registro.nEscritura = i+1; 
                registro.nRegistro =  rand() % REGMAX;  

                if(mi_write(dir_pid, &registro, registro.nRegistro*sizeof(struct REGISTRO), sizeof(struct REGISTRO)) < 0){
                    perror("Error al hacer mi_write en simulacion");
                    return -1;
                }

                // Esperamos 0,05s para la siguiente escritura
                usleep(50000);
                //printf("[simulacion.c -> Escritura %d en %s]\n", i, dir_pid);
            }
            printf("Proceso %d: Completadas %d escrituras en %s\n", proceso, NUMESCRITURAS, dir_pid);
            //printf("[simulacion.c -> Acabado proceso con PID %s, total acabados %d]\n", spid, proceso);
            if(proceso==NUMPROCESOS){
                printf("Total de procesos terminados: %d\n", proceso);
            }/* else {
                printf("[simulacion.c -> acabados: %d]\n", proceso);
            }*/
            
            bumount();
            exit(0);
        }
        // Esperamos 0,2s para lanzar otro proceso
        usleep(200000);
    }
    
    //Permitimos que el padre espere por todos los hijos
    while (acabados < NUMPROCESOS){
        pause();
    }
    // Desmontamos el disco
    bumount(); //proceso padre
    return 0;
}
      

  

