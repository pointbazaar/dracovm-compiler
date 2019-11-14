#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "headers/dracovmcompiler.hpp"
#include "headers/AssemblyCodeGen.hpp"

using namespace std;

int testrun(string name,vector<string> vmcodes);

bool test_fadd1();
bool test_fadd2();
bool test_fsub();
bool test_fsub2();
/*
bool test_add();
bool test_sub();
bool test_mod();
bool test_mod2();
bool test_div();
bool test_div_positive_by_negative();
bool test_div_negative_by_positive();
*/

int main(int argc, char* argv[]){
	
	cout << "START TESTS" << endl;
	//TODO:

	vector<bool> test_results = {
		test_fadd1(),
		test_fadd2(),
		test_fsub(),
		test_fsub2()
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

int testrun(string name,vector<string> vmcodes){
	//returns the status code after running these vm codes
	//the name has to equal the name of the subroutine
	//in the vm code

	map<string,vector<string>> mymap;
	mymap["name"]=vmcodes;

	compile_main2(mymap);

	//start a process, and examine its exit value
	int status=1;
	const string call="./"+name;
	status = system(call.c_str());

	//as we are testing, we should remove the files that have been created

	const string call2="rm "+name+".o "+name+".asm "+name+" ";
	system(call2.c_str());

	return status;
}

bool test_fadd1(){

	const vector<string> vmcodes = {
		"subroutine FADD1_main 0 args 0 locals",
		"fconst 3.0",
		"fconst 2.0",
		"fadd",
		"fconst 7.0"
		"flt",
		"if-goto exit0",
		"iconst 1",
		"exit",
		"label exit0",
		"iconst 0",
		"exit"
	};

	return 0==testrun("FADD1_main",vmcodes);
}

bool test_fadd2(){

	const vector<string> vmcodes={
		"subroutine FADD2_main 0 args 0 locals",
        "fconst 3.0",
        "fconst 6.0",
        "fadd",
        "fconst 7.0",
        "flt",
        "if-goto exit0",
        "iconst 1",
        "exit",
        "label exit0",
        "iconst 0",
        "exit"
	};

	return 1==testrun("FADD2_main",vmcodes);
}

bool test_fsub(){
	const vector<string> vmcodes={
			"subroutine FSUB_main 0 args 0 locals",
            "fconst 3.0",
            "fconst 2.0",
            "fsub",
            "fconst 7.0",
            "flt",
            "if-goto exit0",
            "iconst 1",
            "exit",
            "label exit0",
            "iconst 0",
            "exit"
	};

	return 0==testrun("FSUB_main",vmcodes);
}

bool test_fsub2(){
	const vector<string> vmcodes={
		"subroutine FSUB2_main 0 args 0 locals",
        "fconst 30.0",
        "fconst 2.0",
        "fsub",
        "fconst 7.0",
        "flt",
        "if-goto exit0",
        "iconst 1",
        "exit",
        "label exit0",
        "iconst 0",
        "exit"
	};

	return 1==testrun("FSUB2_main",vmcodes);
}