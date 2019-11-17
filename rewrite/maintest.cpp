#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

#include "headers/dracovmcompiler.hpp"
#include "headers/AssemblyCodeGen.hpp"

using namespace std;

int testrun(string name,vector<string> vmcodes);

//arithmetic tests (all from ArithmeticFocusedAssemblyGeneratorTest)
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

//array related tests (all from ArrayFocusedAssemblyGeneratorTest)
bool test_arraystore1();
bool test_arrayread_arraystore();
bool test_arrayread_arraystore2();
bool test_arraystore_len();
bool test_len_ok_with_stack();
bool test_new_ok_with_stack();

//logic focused tests (all from LogicFocusedAssemblyCodeGeneratorTest)
bool test_ieq_false();
bool test_ieq_true();
bool test_ineq_true();
bool test_ineq_false();
bool test_igt_true();
bool test_igt_false();
bool test_ilt_true();
bool test_ilt_false();
bool test_swap();
bool test_and();
bool test_and2();
bool test_or();
bool test_or2();
bool test_feq1();
bool test_feq2();


int main(int argc, char* argv[]){
	
	cout << "START TESTS" << endl;
	//TODO:

	vector<bool> test_results = {
		//arithmetic related
		test_fadd1(),	//0
		test_fadd2(),	//1
		test_fsub(),	//2
		test_fsub2(),	//3
		test_iadd(),	//4
		test_isub(),	//5
		test_imod(),	//6
		test_imod2(),	//7
		test_idiv(),	//8
		test_idiv_positive_by_negative(),
		test_idiv_negative_by_positive(),	//10


		//array related
		test_arraystore1(),
		test_arrayread_arraystore(),	//12
		test_arrayread_arraystore2(),
		test_arraystore_len(),	//14
		
		test_len_ok_with_stack(),
		test_new_ok_with_stack(),	//16

		
		//logic related
		test_ieq_false(), 	//17	//one of these is failing
		test_ieq_true(),	//18	//

		
		test_ineq_true(),	//19
		test_ineq_false(),	//20
		
		
		test_igt_true(),	//21
		test_igt_false(),	//22

		test_ilt_true(),	//23
		test_ilt_false(),	//24
		

		test_swap(),		//25

		test_and(),			//26
		test_and2(),		//27

		test_or(),			//28
		test_or2(),			//29

		test_feq1(),		//30
		test_feq2()			//31
		
	};
	int i=0;
	for(bool x : test_results){
		cout << setw(3) << i << ": ";
		if(x){
			cout << "PASS " << endl;
		}else{
			cout << "FAIL " << endl;
		}
		i++;
	}
	
	return 0;
}

int testrun(string name,vector<string> vmcodes){
	//returns the startus code after running these vm codes
	//the name has to equal the name of the subroutine
	//in the vm code

	map<string,vector<string>> mymap;
	mymap[name]=vmcodes;

	compile_main2(mymap);

	//start a process, and examine its exit value

	int status=47; //random value thats hopefully never tested for
	const string call="./"+name;
	status = WEXITSTATUS(system(call.c_str()));

	//as we are testing, we should remove the files that have been created

	const bool remove_files=true;

	if(remove_files){
		const string call2="rm "+name+".o "+name+".asm "+name+" ";
		system(call2.c_str());
	}

	//https://stackoverflow.com/questions/20193464/how-to-get-the-exit-code-of-program-invoked-by-system-call
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

	int res = (testrun("IADD_main",vmcodes));
	cout << res << endl;
	return res==5;
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

bool test_arraystore1(){
	//tests if arraystore behaves as expected with the stack

	const vector<string> vmcodes={
		"subroutine ARRAYSTORE_main 0 args 0 locals",
		"iconst 3",
		"call Builtin_new",
		"iconst 0", //store at index 0
		"iconst 9", //store a 9
		"arraystore",
		//3 should be on stack
		"exit"
	};

	return 3==testrun("ARRAYSTORE_main",vmcodes);
}

bool test_arrayread_arraystore(){
	const vector<string> vmcodes={
		"subroutine ARRAYREADARRAYSTORE_main 0 args 0 locals",
		"iconst 3",
		"call Builtin_new",
		//return value is on stack
		"swap",
		"pop",
		"dup",
		//store at index 0
		"iconst 0",
		//store a 9
		"iconst 9",
		"arraystore",

		//value stored, pointer to array on stack
		"iconst 0",
		"arrayread",
		//value read is on stack, should be 9

		"exit"
	};

	return 9==testrun("ARRAYREADARRAYSTORE_main",vmcodes);
}

bool test_arrayread_arraystore2(){

	//this test should verify that we are using correct offsetting of 4 bytes.
    //otherwise, the value at index 1 would be overwritten by the max-value int stored at index 0
    //https://en.wikipedia.org/wiki/2,147,483,647


	const vector<string> vmcodes={
		"subroutine ARRAYREADARRAYSTORE2_main 0 args 0 locals",

		"iconst 2",			//+1
		"call Builtin_new",	//+1
		"swap",
		"pop",				//-1
		"dup",				//+1
		"dup",				//+1
		"iconst 1",			//+1
		"iconst 8",			//+1
		"arraystore",		//-3

		"iconst 0",			//+1
		//store the highest int possible
		"iconst 2147483647",//+1
		"arraystore",		//-3

		"iconst 1",			//+1
		"arrayread",		//-1

		"exit"
	};

	return 8==testrun("ARRAYREADARRAYSTORE2_main",vmcodes);
}

bool test_arraystore_len(){
	const vector<string> vmcodes={
		"subroutine ARRAYSTORELEN_main 0 args 0 locals",
		
		"iconst 3",
		"call Builtin_new",
		"swap",
		"pop",
		"dup",

		"iconst 0",
		"iconst 9",
		"arraystore",

		"call Builtin_len",
		"swap",
		"pop",

		//return value (supposed array length) on stack

		"exit"
	};

	return 3==testrun("ARRAYSTORELEN_main",vmcodes);
}

bool test_len_ok_with_stack(){
	const vector<string> vmcodes={
		"subroutine LENOKSTACK_main 0 args 0 locals",

		"iconst 1",
		"iconst 3",
		"call Builtin_new",

		"call Builtin_len",
		"pop",

		"call Builtin_len",
		"pop",

		"call Builtin_len",
		"pop",

		"pop",//pop array pointer
		"pop",//pop the 3

		//1 should be on stack

		"exit"
	};

	return 1==testrun("LENOKSTACK_main",vmcodes);
}

bool test_new_ok_with_stack(){
	const vector<string> vmcodes={
		"subroutine NEWOKSTACK_main 0 args 0 locals",

		"iconst 5",
		"call Builtin_new",
		"pop", //pop the array pointer

		"exit"
	};

	return 5==testrun("NEWOKSTACK_main",vmcodes);
}


bool test_ieq_false(){
	const vector<string> vmcodes={
		"subroutine IEQFALSE_main 0 args 0 locals",

		"iconst 1",
		"iconst 0",

		"ieq",
		"if-goto label1",

		"iconst 45",
		"exit",

		"label label1",
		"iconst 0",

		"exit"
	};

	return 45==testrun("IEQFALSE_main",vmcodes);
}

bool test_ieq_true(){
	const vector<string> vmcodes={
		"subroutine IEQTRUE_main 0 args 0 locals",

		"iconst 11",
		"iconst 11",

		"ieq",
		"if-goto label1",

		"iconst 45",
		"exit",

		"label label1",
		"iconst 3",
		

		"exit"
	};

	return 3==testrun("IEQTRUE_main",vmcodes);
}

bool test_ineq_true(){
	cout << "test ineq_true" << endl;
	const vector<string> vmcodes={
		"subroutine INEQTRUE_main 0 args 0 locals",

		"iconst 1",
		"iconst 0",

		"ineq",
		"if-goto label1",

		"iconst 45",
		"exit",

		"label label1",
		"iconst 0",
		

		"exit"
	};

	return 0==testrun("INEQTRUE_main",vmcodes);
}

bool test_ineq_false(){
	cout << "test ineq_false" << endl;
	const vector<string> vmcodes={
		"subroutine INEQFALSE_main 0 args 0 locals",

		"iconst 11",
		"iconst 11",

		"ineq",
		"if-goto label1",
		"iconst 45",
		"exit",

		"label label1",
		"iconst 3",
		

		"exit"
	};

	return 45==testrun("INEQFALSE_main",vmcodes);
}

bool test_igt_true(){
	const vector<string> vmcodes={
		"subroutine IGTTRUE_main 0 args 0 locals",

		"iconst 11",
		"iconst 10",

		"igt",
		"if-goto label1",

		"iconst 45",
		"exit",

		"label label1",
		"iconst 3",
		

		"exit"
	};

	return 3==testrun("IGTTRUE_main",vmcodes);
}

bool test_igt_false(){
	//TODO
	return false;
}

bool test_ilt_true(){
	//TODO
	return false;
}

bool test_ilt_false(){
	//TODO
	return false;
}

bool test_swap(){
	//TODO
	return false;
}

bool test_and(){
	//TODO
	return false;
}

bool test_and2(){
	//TODO
	return false;
}

bool test_or(){
	//TODO
	return false;
}

bool test_or2(){
	//TODO
	return false;
}

bool test_feq1(){
	//TODO
	return false;
}

bool test_feq2(){
	//TODO
	return false;
}
