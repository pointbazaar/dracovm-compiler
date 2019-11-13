#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "headers/dracovmcompiler.hpp"

using namespace std;

int main(int argc, char* argv[]){

	cout << "DRACOVM Compiler" << endl;
	const unsigned int nSubroutines = argc-1;

	//parse files given as arguments:
	//they should all have ".dracovm-subroutine"
	//extension. and the files should all exist. 
	//so we should check that.
	
	if(argc==1){
		cerr << " no options or files supplied. exiting " << endl;
		exit(1);
	}
	
	string help_option="-help";
	
	//compare to see if -help 
	if(help_option.compare(argv[1])==0){
		// print help and genereal info
	cout << "USAGE: dracovm (FILE.subroutine.dracovm)*" << endl;
	cout << "USAGE: dracovm -help " << endl;
	cout << "AUTHOR: alex23667@gmail.com" << endl;
	cout << "REPO: https://github.com/pointbazaar/dracovm-compiler" << endl;
		exit(0);
	}

	if(nSubroutines==0){
		cerr << "no .subroutine.dracovm files supplied. exiting." << endl;
		exit(1);
	}

	//TODO: open our dracovm.info file, which should tell us all about
	//what we actually need to compile, also,
	// the subroutine files and their hashes.

	ifstream info_file ("dracovm.info", ios::out | ios::app | ios::binary);
	//this call also creates the file if it does not exist

	if(info_file.good()){
		cout << "dracovm.info exists" << endl;
		//TODO: read the information about the subroutines
	}else{
		cout << "dracovm.info does not exist. exiting" << endl;
		exit(1);
	}

	cout << "VM Code Files to compile: " << endl << endl;
	
	vector <string> filenames;

	for(int i=1;i<argc;i++){
		string filename = argv[i];
		filenames.push_back(filename);
		cout << " - " << filename << endl;
	}

	cout << endl;

	bool success = compile_main(filenames);

	if(success){
		cout << "BUILD SUCCESSFUL" << endl;
	}else{
		cout << "BUILD FAILED" << endl;
	}

	return 0;
}
