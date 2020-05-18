# mi_filesystem

**GRUPO:** 
Ino2

**MIEMBROS:**
Andreas Manuel Korn
Sergio Vega García
Rubén López Babón

**Posibles errores futuros o dudas:**
1. ficheros_basico.c linea 550 no estoy completamente seguro de que esa sea la manera correcta de hacerlo.
	** Incluir el inodo que queremos liberar en la lista de inodos libres (por el principio), actualizando el superbloque para que éste sea ahora el primero de la lista. El inodo liberado apuntará donde antes apuntaba el campo del superbloque. 
	
//Creo que ya no persiste:
Después de  ./truncar disco 1 409605001 hacer ./leer_sf disco 
	Debería:
		cantBloquesLibres = 96851
	En cambio:
		cantBloquesLibres = 96825
		
**Mejoras futuras:**
1. Buscar_camino()[directorios.c], pasarlo a iterativo
2. mi_touch
3. Extra nivel 10
4. liberar_bloques_inodo es muy ineficiente, habria que mirar la manera de mejorarlo para la entrega final
