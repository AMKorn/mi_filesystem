# mi_filesystem

**GRUPO:** 
Ino2

**MIEMBROS:**
Andreas Manuel Korn
Sergio Vega García
Rubén López Babón

**Mejoras realizadas**
- mi_read y mi_write con cache
- mi_ls: todas las mejoras
- mi_touch
- mi_rmdir
- Lectura de ficheros externos de escribir.c

*Pendientes*
- mejoras liberar_bloques_inodo()
- mejora init_MB()
- SB memoria compartida
- buffer conjunto entradas en buscar_entrada()
- mi_rmdir_r
- mi_rn
- mi_mv
- mi_cp_f
- mi_cp

**Sintaxis**
- escribir.c
	- Argumentos esperados: <nombre_dispositivo> <texto|ruta_fichero> <diferentes_inodos> <modo>
	- <modo> -> 2 modos 
		+ -t, lectura directamente desde consola: escitura directa o cat.
		+ -f, lectura desde fichero.
- mi_ls.c
	- Argumentos esperados: <disco> </ruta> <formato>
	- <modo> -> 2 modos 
		+ -s, formato simple.
		+ -e, formato expandido.
- verificacion.c 
	- Argumentos esperados: ./verificacion <disco> </directorio>
	- En </directorio> podemos usar "$(cat dir.txt)" en lugar de copiar y pegar la ruta, ya que simulacion.c guarda el directorio de trabajo en ese dichero

-----------------------------------------------------------------------------------------------------------
1. Buscar_camino()[directorios.c], pasarlo a iterativo
2. mi_touch
3. Extra nivel 10
4. liberar_bloques_inodo es muy ineficiente, habria que mirar la manera de mejorarlo para la entrega final
-------------------------------------------------------------------------------------------------------------

El script scripte3.sh no se ejecuta correctamente, pero se puede copiar y pegar sus comandos 1 a 1 para una correcta valoracion de la practica