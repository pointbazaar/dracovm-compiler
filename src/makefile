main:
	g++ -D DEBUG=false -std=c++17 -o dracovm -I headers/ sources/*.cpp main.cpp


#build the lexer for debugging (includes debug statements)
debug: 
	g++ -D DEBUG=true -std=c++17 -o dracovm-debug -I headers/ sources/*.cpp main.cpp

#build the tests
test: 
	g++ -D DEBUG=true -std=c++17 -o test  -I headers/ sources/*.cpp maintest.cpp
