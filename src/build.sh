#!/bin/bash
#compile
#g++ -std=c++17 -o dracovm -I headers/ sources/*.cpp main.cpp
rm dracovm dracovm-debug test

make main
make debug
make test

#make it available as bash command?
cp dracovm /bin/dracovm
cp dracovm-debug /bin/dracovm-debug
