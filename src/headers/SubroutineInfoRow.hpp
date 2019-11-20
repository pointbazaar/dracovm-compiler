#include <string>
#include <vector>

using namespace std;

class SubroutineInfoRow{

	/*
	This class is supposed to represent one row of information
	(about one subroutine) in the .dracovm.info
	file, which is used to support incremental compilation.
	*/
	
	public:
		string filename;
		vector<string> dependencies;
		string filename_asm;
		string hash;
};