echo ""
echo "Test nivel 10e1"
echo "################################################################################"
echo "$ ./mi_mkfs disco 100000"
./mi_mkfs disco 100000
echo "################################################################################"
echo " ./mi_mkdir disco 6 /dir1/"
./mi_mkdir disco 6 /dir1/ 
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir1/dir11"
./mi_mkdir disco 6 /dir1/dir11/
echo "################################################################################"
echo "$ ./mi_touch disco 6 /dir1/dir11/fic1"
./mi_touch disco 6 /dir1/dir11/fic1
echo "################################################################################"
echo "$ ./mi_escribir disco /dir1/dir11/fic1 hellooooooo 0"
./mi_escribir disco /dir1/dir11/fic1 hellooooooo 0
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir2/"
./mi_mkdir disco 6 /dir2/
echo "################################################################################"
echo "$ ./mi_mkdir disco 6 /dir2/dir21/"
./mi_mkdir disco 6 /dir2/dir21/
echo "################################################################################"
echo "$ ./mi_link disco /dir1/dir11/fic1 /dir2/dir21/fic2"
./mi_link disco /dir1/dir11/fic1 /dir2/dir21/fic2
echo "################################################################################"
echo "$ ./mi_cat disco /dir2/dir21/fic2 #ha de mostrar mismo contenido que /dir1/dir11/fic1"
./mi_cat disco /dir2/dir21/fic2
echo "################################################################################"
echo "$ ./mi_stat disco /dir1/dir11/fic1"
./mi_stat disco /dir1/dir11/fic1
echo "################################################################################"
echo "$ ./mi_stat disco /dir2/dir21/fic2"
./mi_stat disco /dir2/dir21/fic2
echo "################################################################################"
echo "$ ./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic4 #camino1 ha de existir"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic4
echo "################################################################################"
echo "$ ./mi_touch disco 6 /dir1/dir11/fic3"
./mi_touch disco 6 /dir1/dir11/fic3
echo "################################################################################"
echo "$ ./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic4"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic4
echo "################################################################################"
echo "$ ./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic5"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic5
echo "################################################################################"
echo "$ ./mi_stat disco /dir1/dir11/fic3"
./mi_stat disco /dir1/dir11/fic3
echo "################################################################################"
echo "$ ./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic2"
./mi_link disco /dir1/dir11/fic3 /dir2/dir21/fic2
