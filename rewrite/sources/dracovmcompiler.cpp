#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "dracovmcompiler.hpp"
#include "AssemblyCodeGen.hpp"
#include "BuiltinSubroutines.hpp"

using namespace std;

bool compile_main(vector<string> filenames,string exec_filename){
	cout << "compile_main" << endl;

	// from these filenames, check if each has 
	// correct '.subroutine.dracovm' extension
	// (marking them as a .dracovm file which only has 1 subroutine,
	// and where the name of the subroutine is encoded in the filename) 
	// and exists. 
	string correct_ending = ".subroutine.dracovm";

	//https://stackoverflow.com/questions/3578083/what-is-the-best-way-to-use-a-hashmap-in-c
	//contains the lines of the file
	map<string,vector<string>> vm_sources;

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


					//put the contents
					//of the file in a hashmap
					vector<string> lines;
					string line;
					while(getline(file,line)){
						//pass
						lines.push_back(line);
					}

					file.close();
					vm_sources[filename]=lines;
					continue;
				}else{
					file.close();
					cerr << filename
						 << " does not exist or could not be opened."
						 << endl;
					 return false;
				}
			}
		}
		cerr << filename 
			 << " did not have correct extension: " 
			 << correct_ending
			 << endl;

		return false;
	}

	//TODO:
	//then make hashes, compare those to those
	//found in 'dracovm.info' , and if they differ,
	//add them to the list of files to be recompiled.

	//for now, to complete the rewrite quickly, just compile,
	//without incremental compilation

	return compile_main2(vm_sources,exec_filename);
}

bool compile_main2(map<string,vector<string>> vm_sources, string exec_filename){

	//DEBUG
	cerr << "compile_main2" << endl;

	map<string,vector<string>> asm_codes = compile_vmcodes(vm_sources);

	vector<string> obj_files;

	bool success=true;

	//write these asm codes to their respective files
	for(auto const& entry : asm_codes){
		string filename = entry.first;
		vector<string> myasm = entry.second;

		ofstream file;
		file.open(filename+".asm");

		for(string line : myasm){
			file << line << endl;
		}

		file.close();

		//call nasm
		const string call1 = "nasm -f elf "+filename+".asm";
		success &= WEXITSTATUS(system(call1.c_str())) == 0;

		obj_files.push_back(filename+".o");
	}

	//call ld to link all the object files
	const string call2 = "ld -melf_i386 -s -o "+exec_filename+" "+join(obj_files," ");
	success &= WEXITSTATUS(system(call2.c_str()))==0;

	return success;
}

/*
About incremental compilation in dragon:

there are different cases aspects to this:

1:  1 or more.subroutine.dracovm file, no dependencies amongst each other:
	look in the file metadata if the file has changed
	since an earlier compilation, if so,
	that could be from usage of git,
	
	https://git.wiki.kernel.org/index.php/Git_FAQ#Why_isn.27t_Git_preserving_modification_time_on_files.3F

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


Making sure that the subroutines call each other with
the right number and types of arguments is the responsibility
of the dragon compiler, which should also support
incremental compilation in the future.

checking that all calls are valid (at the dracovm level)
is just an added safeguard/convenience.
Also it should be implemented, as dragon and dracovm
are conceptually different, and actually different programs
which should be correct and check for errors on their own,
without relying on the next or previous program to do it.
*/

//Assembly debugging:
//(they are missing a stack display, but it be ok)
//https://carlosrafaelgn.com.br/Asm86/

//general having fun and trying different programming languages:
//https://tio.run/#
//https://www.tutorialspoint.com/codingground.htm

//https://asmtutor.com/
//https://asmtutor.com/#lesson1

//https://www.youtube.com/watch?v=ubXXmQzzNGo

//$ nasm -f elf hello.asm  # this will produce hello.o ELF object file
//$ ld -s -o hello hello.o # this will produce hello executable