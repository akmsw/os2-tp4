#! /bin/bash

make clean && make && clear

echo "nombre de la imagen principal:"
read main

echo "nombre de la imagen patrón:"
read target

echo "cantidad de threads:"
read threads

python3 ./png2pgm.py $main $target && clear && time ./lab4 $main $target $threads && python3 ./drawMatch.py $main && make clean && clear