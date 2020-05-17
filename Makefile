CC=gcc
CFLAGS=-c -g -Wall -std=gnu99
#LDFLAGS=-pthread

SOURCES=bloques.c mi_mkfs.c ficheros_basico.c leer_sf.c ficheros.c escribir.c leer.c permitir.c truncar.c directorios.c prueba_nivel_7.c mi_mkdir.c mi_chmod.c mi_stat.c mi_escribir.c mi_ls.c mi_escribir_varios.c mi_link.c #todos los .c
LIBRARIES=bloques.o ficheros_basico.o ficheros.o directorios.o #todos los .o de la biblioteca del SF
INCLUDES=bloques.h ficheros_basico.h ficheros.h directorios.h #todos los .h
PROGRAMS=mi_mkfs leer_sf escribir leer permitir truncar prueba_nivel_7 mi_mkdir mi_chmod mi_stat mi_escribir mi_ls mi_escribir_varios mi_link
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o *~ $(PROGRAMS)
