#compile
g++ -o dracovm -I headers/ sources/*.cpp main.cpp

#make it available as bash command?
cp dracovm /bin/dracovm
