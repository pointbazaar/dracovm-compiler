#include <iostream>

using namespace std;

int main(int argc, char* argv[]){

	cout << "DRACOVM Compiler" << endl;

	const unsigned int nSubroutines = argc-1;

	//parse files given as arguments:
	//they should all have ".dracovm-subroutine"
	//extension. and the files should all exist. 
	//so we should check that.
	
	cout << "VM Code Files to compile: " << endl << endl;
	for(int i=1;i<argc;i++){
		char* filename = argv[i];
		
		cout << " - " << filename << endl;
		try{
			
		}catch(const string msg){
			cerr << "error with " << filename << endl;
		}
	}

	cout << endl;

	cout << "successfully compiled " << nSubroutines << " subroutines" << endl;
	
	return 0;
}