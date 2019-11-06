#include <vector>
#include <string>
#include <iostream>
#include <fstream>

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
				//filename has correct ending

				ifstream file(filename, ios::in);
				if(file.is_open()){
					//file exists and could be opened
					file.close();
					continue;
				}else{
					file.close();
					cerr << filename
						 << " does not exist or could not be opened."
						 << endl;
				}
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

/*
About incremental compilation in dragon:

there are different cases aspects to this:

1:  1 or more.subroutine.dracovm file, no dependencies amongst each other:
	look in the file metadata if the file has changed
	since an earlier compilation, if so,
	that could be from usage of git,
	or any other reason, which might not have
	actually changed the file.
	so a fast hash (idk, maybe md5) is computed and
	compared with the info in 'dracovm.info' file.
	recompilation is started if the hashes differ.

2:	2 or more .subroutine.dracovm files, where one or more 
	.subroutine.dracovm files depend on other .subroutine.dracovm files.

	DracoVM doesn't know about the number of arguments
	that have been put on the stack before a subroutine call,
	it only knows about the subroutine name 
	(which is also part of the filename)
	So, for any file which needs to be recompiled, it is checked
	if the subroutines that it calls 
	(represented by the files, 
	which filenames this program receives as arguments)
	were specified to be compiled alongside it
	(if the were mentioned in the commandline arguments.)


*/