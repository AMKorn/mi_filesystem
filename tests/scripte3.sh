make
echo "$ ./mi_mkfs disco 100000"
./mi_mkfs disco 100000
echo "$ time ./simulacion disco"
time ./simulacion disco
echo "$ time ./verificacion disco $(cat dir.txt)"
time ./verificacion disco "$(cat dir.txt)"
echo "$ ./mi_cat disco $(cat dir.txt)informe.txt > resultado.txt"
./mi_cat disco "$(cat dir.txt)"informe.txt > resultado.txt
echo "$ ls -l resultado.txt"
ls -l resultado.txt
echo "$ cat resultado.txt"
cat resultado.txt
echo "$ ./leer_sf disco"
./leer_sf disco
make clean