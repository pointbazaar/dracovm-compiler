#compile
g++ -std=c++17 -o dracovm -I headers/ sources/*.cpp main.cpp

#make it available as bash command?
cp dracovm /bin/dracovm
