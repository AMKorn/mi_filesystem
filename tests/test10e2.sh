echo ""
echo "Test nivel 10e2"
echo "################################################################################"
echo "$ ./mi_rmdir disco /dir2/dir21/"
./mi_rmdir disco /dir2/dir21/
echo "################################################################################"
echo " ./mi_rm disco /dir2/dir21/fic2"
./mi_rm disco /dir2/dir21/fic2
echo "################################################################################"
echo "$ ./mi_stat disco /dir1/dir11/fic1 #Hemos borrado 1 enlace"
./mi_stat disco /dir1/dir11/fic1
echo "################################################################################"
echo "$ ./mi_rm disco /dir2/dir21/fic2"
./mi_rm disco /dir2/dir21/fic2
echo "################################################################################"
echo "$ ./mi_rm disco /dir2/dir21/"
./mi_rmdir disco /dir2/dir21/
echo "################################################################################"
echo "$ ./mi_ls disco /dir2/dir21/"
./mi_ls disco /dir2/dir21/
echo "################################################################################"
echo "$ ./mi_rm disco /dir2/dir21/fic4"
./mi_rm disco /dir2/dir21/fic4
echo "################################################################################"
echo "$ ./mi_rm disco /dir2/dir21/fic5"
./mi_rm disco /dir2/dir21/fic5
echo "################################################################################"
echo "$ ./mi_rmdir disco /dir2/dir21/"
./mi_rmdir disco /dir2/dir21/
echo "################################################################################"
echo "$ ./mi_ls disco /dir2/"
./mi_ls disco /dir2/
