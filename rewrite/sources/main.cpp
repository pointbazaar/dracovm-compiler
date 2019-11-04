#include <iostream>

using namespace std;

int main(int argc, char* argv[]){

	cout << "DRACOVM Compiler" << endl;

	//TODO: parse files given as arguments
	cout << "printing arguments: " << endl;
	for(int i=1;i<argc;i++){

		cout << argv[i] << endl;
	}
	
	return 0;
}