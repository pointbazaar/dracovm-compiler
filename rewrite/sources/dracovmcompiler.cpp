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
	string correct_ending = ".subroutine.dracovm";

	for(auto const& filename : filenames){
		if(filename.size()>=correct_ending.size()){
			string ending = 
				filename
				.substr(filename.size()-correct_ending.size(),filename.size());
			if(ending.compare(correct_ending)==0){
				continue;
			}
		}
		cerr << filename 
			 << " did not have correct extension: " 
			 << correct_ending
			 << endl;

		return false;
	}

	//then make hashes, compare those to those
	//found in 'dracovm.info' , and if they differ,
	//add them to the list of files to be recompiled.

	//TODO

	return false;
}