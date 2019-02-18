#!/bin/bash
counter=1

g++ -std=c++14 -Wall -Wextra -w SrcAssgn2-tas-ES16BTECH11027.cpp -o tas
g++ -std=c++14 -Wall -Wextra -w SrcAssgn2-cas-ES16BTECH11027.cpp -o cas
g++ -std=c++14 -Wall -Wextra -w SrcAssgn2-cas-bounded-ES16BTECH11027.cpp -o casb
while [ $counter -le 5 ]
do
	./tas
	((counter++))
done

counter=1

while [ $counter -le 5 ]
do
	./cas
	((counter++))
done

counter=1

while [ $counter -le 5 ]
do
	./casb
	((counter++))
done