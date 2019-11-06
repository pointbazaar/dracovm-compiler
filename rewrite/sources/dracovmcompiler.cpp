#include <vector>
#include <string>
#include <iostream>

using namespace std;

bool compile_main(vector<string> filenames){
	cout << "compile_main" << endl;

	//TODO: from these filenames, check if each has 
	//correct '.subroutine.dracovm' extension
	// (marking them as a .dracovm file which only has 1 subroutine,
	// and where the name of the subroutine is encoded in the filename) 
	//and exists. 
	//then make hashes, compare those to those
	//found in 'dracovm.info' , and if they differ,
	//add them to the list of files to be recompiled.

	//TODO

	return true;
}