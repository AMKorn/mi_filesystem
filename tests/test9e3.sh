echo ""
echo "Test nivel 9e3"
echo "################################################################################"
echo "$ ./mi_mkfs disco 100000"
./mi_mkfs disco 100000
echo "################################################################################"
echo " ./mi_mkdir disco 6 /dir1/"
./mi_mkdir disco 6 /dir1/  
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/fic3"
./mi_mkdir disco 6 /dir1/fic3
echo "################################################################################"
echo "$ ./mi_escribir_varios disco /dir1/fic3 “--texto repetido en 10 bloques--” 0"
echo "# bucle llamando a mi_escribir 10 veces con offset desplazado 1 bloque"
./mi_escribir_varios disco /dir1/fic3 "--texto repetido en 10 bloques--" 0
echo "################################################################################"
echo "$ ./mi_stat disco /dir1/fic3"
./mi_stat disco /dir1/fic3
echo "################################################################################"
echo "$ ./mi_cat disco /dir1/fic3 #tambuffer=BLOCKSIZE * 4"
./mi_cat disco /dir1/fic3