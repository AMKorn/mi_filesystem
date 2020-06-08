make clean
make
echo "$ rm disco"
rm disco
echo "$ ./mi_mkfs disco 100000"
./mi_mkfs disco 100000
echo "################################################################################"
echo "$ ./leer_sf disco"
./leer_sf disco
echo "################################################################################"
echo "$./escribir"
./escribir
echo "################################################################################"
echo "$ ./escribir disco "$(cat texto2.txt)" 0 -t"
echo "#escribimos el texto2.txt en los offsets  9000, 209000, 30725000, "
echo "#409605000 y 480000000 de un mismo inodo"
./escribir disco "$(cat texto2.txt)" 0 -t
echo "################################################################################"
echo "$ ./permitir disco 1 0"
echo "#cambiamos permisos del inodo 1 a 0"
./permitir disco 1 0
echo "################################################################################"
echo "$ ./leer disco 1"
echo "#intentamos leer inodo 1 con permisos=0"
./leer disco 1
echo "################################################################################"
echo "$ ./permitir disco 1 6"
echo "#cambiamos permisos del inodo 1 a 6"
./permitir disco 1 6
echo "################################################################################"
echo "$ ./leer disco 1 > ext1.txt"
./leer disco 1 > ext1.txt
echo "################################################################################"
echo "$ ls -l ext1.txt"
ls -l ext1.txt
echo "################################################################################"
echo "$ ./leer_sf disco"
./leer_sf disco
echo "################################################################################"
echo "$ ./truncar disco 1 409605001"
./truncar disco 1 409605001
echo "################################################################################"
echo "$ ./leer_sf disco"
./leer_sf disco
echo "################################################################################"
echo "$ ./truncar disco 1 30725003"
./truncar disco 1 30725003
echo "################################################################################"
echo "$ ./leer_sf disco"
./leer_sf disco
echo "################################################################################"
echo "$ ./truncar disco 1 209008"
./truncar disco 1 209008
echo "################################################################################"
echo "$ ./leer_sf disco"
./leer_sf disco
echo "################################################################################"
echo "$ ./truncar disco 1 9005"
./truncar disco 1 9005
echo "################################################################################"
echo "$ ./leer disco 1 > ext1.txt"
./leer disco 1 > ext1.txt
echo "################################################################################"
echo "$ ls -l ext1.txt"
ls -l ext1.txt
echo "################################################################################"
echo "$ cat ext1.txt"
cat ext1.txt
echo ""
echo "################################################################################"
echo "$ ./leer_sf disco"
./leer_sf disco
echo "################################################################################"
echo "$ ./truncar disco 1 0"
./truncar disco 1 0
echo "################################################################################"
echo "$ ./leer_sf disco"
./leer_sf disco
echo "################################################################################"
make clean