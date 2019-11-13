#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "headers/dracovmcompiler.hpp"
#include "headers/AssemblyCodeGen.hpp"

using namespace std;

bool test_fadd1();

int main(int argc, char* argv[]){
	
	cout << "START TESTS" << endl;
	//TODO:

	vector<bool> test_results = {
		test_fadd1()
	};

	for(bool x : test_results){
		if(x){
			cout << "PASS " << endl;
		}else{
			cout << "FAIL " << endl;
		}
	}
	
	return 0;
}

bool test_fadd1(){

	const vector<string> vmcodes = {
		"subroutine Main_main 0 args 0 locals",
		"fconst 3.0",
		"fconst 2.0",
		"fadd",
		"flt",
		"if-goto exit0",
		"iconst 1",
		"exit",
		"label exit0",
		"iconst 0",
		"exit"
	};

	map<string,vector<string>> mymap;
	mymap["test_fadd1"]=vmcodes;

	compile_main2(mymap);

	//start a process, and examine its exit value
	int status=1;
	status = system("./test_fadd1");

	return status==0;
}