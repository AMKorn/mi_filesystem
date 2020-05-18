echo ""
echo "Test nivel 9e2"
echo "# Comprobación de la caché de directorios"
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/fic2"
./mi_mkdir disco 6 /dir1/fic2
echo "################################################################################"
echo "$ ./mi_escribir disco /dir1/fic2 "$(cat texto2.txt)" 1000"
./mi_escribir disco /dir1/fic2 "$(cat texto2.txt)" 1000
echo "################################################################################"
echo "$ ./mi_cat disco /dir1/fic2 #tambuffer=BLOCKSIZE * 4"
./mi_cat disco /dir1/fic2
echo "################################################################################"
echo "$ ./mi_escribir disco /dir1/fic2 “******************************” 10000"
./mi_escribir disco /dir1/fic2 "******************************" 10000 
echo "################################################################################"
echo "$ ./mi_ls disco /dir1/"
./mi_ls disco /dir1/
echo "################################################################################"
echo "$ ./mi_cat disco /dir1/fic2"
./mi_cat disco /dir1/fic2