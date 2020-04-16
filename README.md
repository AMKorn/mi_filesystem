# mi_filesystem

Posibles errores futuros o dudas:
1. En algun momento se hace que el inodo 0 valga 0 en lugar de valer 1, no estoy seguro de si es un error o es normal, hasta que resuelva esta duda lo dejaré como un posible error.
	SOLUCIONADO: La funcion reservar_inodo lo modifica.
2. ficheros_basico.c linea 550 no estoy completamente seguro de que esa sea la manera correcta de hacerlo.
	** Incluir el inodo que queremos liberar en la lista de inodos libres (por el principio), actualizando el superbloque para que éste sea ahora el primero de la lista. El inodo liberado apuntará donde antes apuntaba el campo del superbloque. **
3. liberar_bloques_inodo es muy ineficiente, habria que mirar la manera de mejorarlo para la entrega final