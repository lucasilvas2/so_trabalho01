#! /bin/bash
echo "Digite o nome o programa que sera executado repetidas vezes:"
read file
echo "Quantas vezes:"
read x
echo "Quantidade de elementos:"
read p
for i in $x; do
	./$file 'matriz_1.txt' 'matriz_2.txt' $p
done