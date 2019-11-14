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

bool test_iadd();
bool test_isub();
bool test_imod();

bool test_imod2();

bool test_idiv();

bool test_idiv_positive_by_negative();
bool test_idiv_negative_by_positive();


int main(int argc, char* argv[]){
	
	cout << "START TESTS" << endl;
	//TODO:

	vector<bool> test_results = {
		test_fadd1(),
		test_fadd2(),
		test_fsub(),
		test_fsub2(),
		test_iadd(),
		test_isub(),
		test_imod(),
		test_imod2(),
		test_idiv(),
		test_idiv_positive_by_negative(),
		test_idiv_negative_by_positive()
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
	mymap[name]=vmcodes;

	compile_main2(mymap);

	//start a process, and examine its exit value

	int status=47; //random value thats hopefully never tested for
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

bool test_iadd(){
	const vector<string> vmcodes={
		"subroutine IADD_main 0 args 0 locals",
		"iconst 3",
		"iconst 2",
		"iadd",
		"exit"
	};

	bool res = 5 == (testrun("IADD_main",vmcodes));
	cout << res << endl;
	return res;
}

bool test_isub(){
	const vector<string> vmcodes={
		"subroutine ISUB_main 0 args 0 locals",
		"iconst 3",
		"iconst 2",
		"isub",
		"exit"
	};

	return 1==testrun("ISUB_main",vmcodes);
}


bool test_imod(){
	const vector<string> vmcodes={
		"subroutine IMOD_main 0 args 0 locals",
		"iconst 3",
		"iconst 2",
		"imod",
		"exit"
	};

	return 1==testrun("IMOD_main",vmcodes);
}


bool test_imod2(){
	const vector<string> vmcodes={
		"subroutine IMOD2_main 0 args 0 locals",
		"iconst 23",
		"iconst 6",
		"imod",
		"exit"
	};

	return 5==testrun("IMOD2_main",vmcodes);
}

bool test_idiv(){
	const vector<string> vmcodes={
		"subroutine IDIV_main 0 args 0 locals",
		"iconst 6",
		"iconst 2",
		"idiv",
		"exit"
	};

	return 3==testrun("IDIV_main",vmcodes);
}

bool test_idiv_positive_by_negative(){
	const vector<string> vmcodes={
		"subroutine IDIV2_main 0 args 0 locals",
		"iconst 200",
		"iconst -10",
		"idiv",
		"iconst 20",
		"iadd",
		"exit"
	};

	return 0==testrun("IDIV2_main",vmcodes);

}

bool test_idiv_negative_by_positive(){

	//https://stackoverflow.com/questions/46574893/divide-a-positive-number-by-a-negative-number-in-assembly

    //maybe the problem is that -300 is not sign-extended
    // edx:eax is the dividend. so if edx == 0b0... then maybe there is a problem?


	const vector<string> vmcodes={
		"subroutine IDIV3_main 0 args 0 locals",
		"iconst -300",
		"iconst 10",
		"idiv",
		"iconst 30",
		"iadd",
		"exit"
	};

	return 0==testrun("IDIV3_main",vmcodes);
}
