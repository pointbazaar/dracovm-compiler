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
//all from the .java, except these 4:
bool test_flt1();
bool test_flt2();
bool test_fgt1();
bool test_fgt2();


//subroutine focused (all from SubroutineFocusedAssemblyCodeGeneratedTest)
bool test_readchar();
bool test_putchar();
bool test_putdigit();
bool test_putchar_multiple();
bool test_compile_new_and_len();

//Builtin Subroutine Tests (all from BuiltinSubroutinesInAssemblyTest)
bool test_abs();
bool test_time();
bool test_fopen();
bool test_fopen_fputs();

//general (from AssemblyCodeGeneratorTest)
bool test_correct_return_code();
bool test_goto();
bool test_if_goto();

int main(int argc, char* argv[]){
	
	cout << "START TESTS" << endl;
	//TODO:

	vector<bool> test_results = {

		//arithmetic related (11)
		test_fadd1(),	//0
		test_fadd2(),	//1
		test_fsub(),	//2
		test_fsub2(),	//3
		test_iadd(),	//4
		test_isub(),	//5
		test_imod(),	//6
		test_imod2(),	//7
		test_idiv(),	//8
		test_idiv_positive_by_negative(),	//9
		test_idiv_negative_by_positive(),	//10


		//array related (6)
		test_arraystore1(),				//11
		test_arrayread_arraystore(),	//12
		test_arrayread_arraystore2(),	//13
		test_arraystore_len(),			//14
		
		test_len_ok_with_stack(),	//15
		test_new_ok_with_stack(),	//16

		
		//logic related (15)
		test_ieq_false(), 	//17
		test_ieq_true(),	//18

		
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
		test_feq2(),		//31

		test_flt1(),		//32
		test_flt2(),		//33
		test_fgt1(),		//34
		test_fgt2(),		//35

		//subroutine related (5)
		test_readchar(),
		test_putchar(),
		test_putdigit(),
		test_putchar_multiple(),
		test_compile_new_and_len(),

		//builtin subroutine related
		test_abs(),
		test_time(),
		test_fopen(),
		test_fopen_fputs(),

		//general
		test_correct_return_code(),
		test_goto(),
		test_if_goto()
		
	};
	int i=0;
	int passed = 0;
	int failed = 0;
	for(bool x : test_results){
		cout << setw(3) << i << ": ";
		if(x){
			cout << "PASS " << endl;
			passed++;
		}else{
			cout << "FAIL " << endl;
			failed++;
		}
		i++;
	}

	cout << test_results.size() 
		<< " Tests, " 
		<< passed << " Passed, "
		<< failed << " Failed.";
	
	return 0;
}

int testrun(string name,vector<string> vmcodes){
	//returns the startus code after running these vm codes
	//the name has to equal the name of the subroutine
	//in the vm code

	map<string,vector<string>> mymap;
	mymap[name]=vmcodes;

	compile_main2(mymap,name);

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
	const vector<string> vmcodes={
		"subroutine IGTFALSE_main 0 args 0 locals",

		"iconst 11",
		"iconst 11",

		"igt",
		"if-goto label1",

		"iconst 45",
		"exit",

		"label label1",
		"iconst 3",
		

		"exit"
	};

	return 45==testrun("IGTFALSE_main",vmcodes);
}

bool test_ilt_true(){
	const vector<string> vmcodes={
		"subroutine ILTTRUE_main 0 args 0 locals",

		"iconst 2",
		"iconst 11",

		"ilt",
		"if-goto label1",

		"iconst 45",
		"exit",

		"label label1",
		"iconst 3",
		

		"exit"
	};

	return 3==testrun("ILTTRUE_main",vmcodes);
}

bool test_ilt_false(){
	const vector<string> vmcodes={
		"subroutine ILTFALSE_main 0 args 0 locals",

		"iconst 11",
		"iconst 11",

		"ilt",
		"if-goto label1",

		"iconst 45",
		"exit",

		"label label1",
		"iconst 3",
		

		"exit"
	};

	return 45==testrun("ILTFALSE_main",vmcodes);
}

bool test_swap(){
	const vector<string> vmcodes={
		"subroutine SWAP_main 0 args 0 locals",

		"iconst 2",
		"iconst 11",

		"swap",
		"igt",
		"if-goto label1",

		"iconst 45",
		"exit",

		"label label1",
		"iconst 3",
		

		"exit"
	};

	return 3==testrun("SWAP_main",vmcodes);
}

bool test_and(){
	const vector<string> vmcodes={
		"subroutine AND_main 0 args 0 locals",

		"iconst 1",
		"iconst 1",

		"and",
		"exit"
	};

	return 1==testrun("AND_main",vmcodes);
}

bool test_and2(){
	const vector<string> vmcodes={
		"subroutine AND2_main 0 args 0 locals",

		"iconst 1",
		"iconst 0",

		"and",
		"exit"
	};

	return 0==testrun("AND2_main",vmcodes);
}

bool test_or(){
	const vector<string> vmcodes={
		"subroutine OR_main 0 args 0 locals",

		"iconst 0",
		"iconst 0",

		"or",
		"exit"
	};

	return 0==testrun("OR_main",vmcodes);
}

bool test_or2(){
	const vector<string> vmcodes={
		"subroutine OR2_main 0 args 0 locals",

		"iconst 1",
		"iconst 0",

		"or",
		"exit"
	};

	return 1==testrun("OR2_main",vmcodes);
}

bool test_feq1(){
	//TODO: remove it once migration from java is complete
	return false;
}

bool test_feq2(){
	//TODO: remove it once migration from java is complete
	return false;
}

bool test_flt1(){
	const vector<string> vmcodes={
		"subroutine FLT1_main 0 args 0 locals",

		"fconst 1.0",
		"fconst 0.0",

		"flt",
		"exit"
	};

	return 0==testrun("FLT1_main",vmcodes);
}

bool test_flt2(){
	const vector<string> vmcodes={
		"subroutine FLT2_main 0 args 0 locals",

		"fconst 1.0",
		"fconst 2.0",

		"flt",
		"exit"
	};

	return 1==testrun("FLT2_main",vmcodes);
}

bool test_fgt1(){
	const vector<string> vmcodes={
		"subroutine FGT1_main 0 args 0 locals",

		"fconst 1.0",
		"fconst 0.0",

		"fgt",
		"exit"
	};

	return 1==testrun("FGT1_main",vmcodes);
}

bool test_fgt2(){
	const vector<string> vmcodes={
		"subroutine FGT2_main 0 args 0 locals",

		"fconst 1.0",
		"fconst 2.0",

		"fgt",
		"exit"
	};

	return 0==testrun("FGT2_main",vmcodes);
}

bool test_readchar(){
	const vector<string> vmcodes={
		"subroutine READCHAR_main 0 args 0 locals",

		"call Builtin_readchar",
		"call Builtin_putchar",

		"iconst 0",
		"exit"
	};
	
	//TODO: use outputstream, test inputstream
	/*
	pr.getOutputStream().write('c');
    pr.getOutputStream().write('\n');
    pr.getOutputStream().close();

    pr.waitFor();

    assertEquals(0,pr.exitValue());
    assertEquals("c", IOUtils.toString(pr.getInputStream()));
	*/

	//return 0==testrun("READCHAR_main",vmcodes);
	return false;
}

bool test_putchar(){
	const vector<string> vmcodes={
		"subroutine PUTCHAR_main 0 args 0 locals",

		"cconst 'c'",
		"call Builtin_putchar",

		"pop",
		"iconst 0",
		"exit"
	};
	//TODO
	/*
	assertEquals(0,pr.exitValue());
    assertEquals("c", IOUtils.toString(pr.getInputStream()));

	*/

	//return 1==testrun("PUTCHAR_main",vmcodes);
	return false;
}

bool test_putdigit(){
	const vector<string> vmcodes={
		"subroutine PUTDIGIT_main 0 args 0 locals",

		"iconst 3",
		"call Builtin_putdigit",

		"pop",
		"iconst 0",
		"exit"
	};
	//TODO
	/*
		assertEquals(0,pr.exitValue());
    	assertEquals("3", IOUtils.toString(pr.getInputStream()));
	*/

	//return 1==testrun("PUTDIGIT_main",vmcodes);
	return false;
}

bool test_putchar_multiple(){
	const vector<string> vmcodes={
		"subroutine PUTCHARMULT_main 0 args 0 locals",

		"iconst 234",

		"cconst 'a'",
		"call Builtin_putchar",
		"pop",
		"pop",

		"cconst 'b'",
		"call Builtin_putchar",
		"pop",
		"pop",

		"cconst 'c'",
		"call Builtin_putchar",
		"pop",
		"pop",

		"exit"
	};
	//TODO
	/*
		assertEquals(exit_code,pr.exitValue());
    	assertEquals("abc", IOUtils.toString(pr.getInputStream()));
	*/

	//return 1==testrun("PUTCHARMULT_main",vmcodes);
	return false;
}

bool test_compile_new_and_len(){
	const vector<string> vmcodes={
		"subroutine NEWLEN_main 0 args 0 locals",

		"iconst 3",

		"call Builtin_new",
		"swap",
		"pop",
		"call Builtin_len",
		"swap",
		"pop",
		"call Builtin_putdigit",
		"pop",
		"pop",
		"iconst 0",

		"exit"
	};
	//TODO
	/*
		assertEquals(0,pr.exitValue());
    	assertEquals("3", IOUtils.toString(pr.getInputStream()));
	*/

	//return 0==testrun("NEWLEN_main",vmcodes);
	return false;
}

bool test_abs(){
	//TODO: implement
	/*
	 a.subroutine("Main","main",0,0);

        a.iconst(3);//arg
        a.call("Builtin","abs");
        //clear arg
        a.swap();
        a.pop();

        //return value on stack
        a.call("Builtin","putdigit");

        a.pop(); //clear return value
        a.pop(); //clear argument

        a.iconst(-3); //arg
        a.call("Builtin","abs");
        //clear arg
        a.swap();
        a.pop();

        //return value on stack
        a.call("Builtin","putdigit");

        a.pop(); //clear return value
        a.pop(); //clear argument

        a.iconst(0);
        a.exit();

        final List<String> vmcodes=a.getDracoVMCodeInstructions();

        final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "arraystore_testing");

        assertEquals(0,pr.exitValue());
        assertEquals("33", IOUtils.toString(pr.getInputStream()));

	*/
	return false;
}

bool test_time(){
	//TODO: implement
	/*
	final DracoVMCodeWriter a=new DracoVMCodeWriter();

        a.subroutine("Main","main",0,0);

        a.call("Builtin","time");

        //return value on stack
        a.call("Builtin","putdigit");

        a.pop(); //clear return value
        a.pop(); //clear argument

        a.iconst(0);
        a.exit();

        final List<String> vmcodes=a.getDracoVMCodeInstructions();

        final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "time_testing");

        assertEquals(0,pr.exitValue());

        //running this shows that the value seems to be increasing,
        //as we get successive alphabetic characters when we are in the ascii range
        System.out.println(IOUtils.toString(pr.getInputStream()));


        //toodo: we should really test it, but for that we probably have to have a builtin
        //subroutine in assembly to print an integer to stdout

	*/
	return false;
}

bool test_fopen(){
	//TODO: implement
	/*
	final DracoVMCodeWriter a=new DracoVMCodeWriter();

        a.subroutine("Main","main",0,0);

        a.iconst(2); //we need a string of length 2, 1 char for name, 1 char for nullbyte
        a.call("Builtin","new");

        //pointer to string should be on stack

        //get rid of argument
        a.swap();
        a.pop();

        a.dup();

        //store filename into string
        a.iconst(0); //index
        a.cconst('x'); //our filename
        a.arraystore();

        a.dup();

        //store the nullbyte, as it is a c-style system call
        a.iconst(1);
        a.iconst(0);
        a.arraystore();

        a.iconst(2); //access mode rw

        //subroutine arguments must be on stack in reverse order.
        a.swap();

        a.call("Builtin","fopen");

        a.swap();
        a.pop(); //clear argument

        a.swap();
        a.pop(); //clear argument

        //our file descriptor is the exit code.
        //if there was no error it should be =/= -1
        a.exit();

        final List<String> vmcodes=a.getDracoVMCodeInstructions();

        final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "time_testing");

        System.out.println(pr.exitValue());
        assertNotEquals(-1,pr.exitValue());

        Assert.fail();
        //this test is flawed. it should give a different file descriptor,
        //if we change the filename. but you can see from the exit code that it is not the case

	*/
	return false;
}

bool test_fopen_fputs(){
	//TODO: implement
	/*
	//TODO: think about it: why could this fail?
        // maybe the syscall is expecting our string in bytes, yet we are working with DWORDS
        // let us investigate why this test is not working

        final DracoVMCodeWriter a=new DracoVMCodeWriter();

        a.subroutine("Main","main",0,0);

        a.iconst(2); //we need a string of length 2, 1 char for name, 1 char for nullbyte
        a.call("Builtin","new");

        //pointer to string should be on stack

        //get rid of argument
        a.swap();
        a.pop();

        a.dup();

        //store filename into string
        a.iconst(0); //index
        a.cconst('c'); //our filename
        a.arraystore();

        a.dup();

        //store the nullbyte, as it is a c-style system call
        a.iconst(1);
        a.iconst(0);
        a.arraystore();

        a.iconst(2); //access mode rw

        //subroutine arguments must be on stack in reverse order.
        a.swap();

        a.call("Builtin","fopen");

        a.pop(); //clear return value
        a.pop(); //clear argument
        a.pop(); //clear argument

        a.iconst(0);
        a.exit();

        final List<String> vmcodes=a.getDracoVMCodeInstructions();

        final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "time_testing");

        assertEquals(0,pr.exitValue());

        //see if file exists
        final Path path = Paths.get("c");
        assertTrue(Files.exists(path));

        //https://stackoverflow.com/questions/1390592/check-if-file-is-already-open
        //maybe this whole test is doomed, because the process we are creating
        //is probably a child of this? what does that mean for files open?
        try {
            org.apache.commons.io.FileUtils.touch(path.toFile());
            Assert.fail();
        } catch (IOException e) {
            //pass, we could not touch the file as it is locked and has been opened by someone else
        }

	*/
	return false;
}

bool test_correct_return_code(){
	const vector<string> vmcodes={
		"subroutine RETURNCODE_main 0 args 0 locals",

		"iconst 45",
		"exit"
	};

	return 45==testrun("RETURNCODE_main",vmcodes);
}

bool test_goto(){
	const vector<string> vmcodes={
		"subroutine GOTO_main 0 args 0 locals",

		"goto label1",
		"iconst 45",
		"exit",
		"label label1",
		"iconst 0",
		"exit"
	};

	return 0==testrun("GOTO_main",vmcodes);
}

bool test_if_goto(){
	const vector<string> vmcodes={
		"subroutine IFGOTO_main 0 args 0 locals",

		"iconst 1",
		"if-goto label1",
		"iconst 45",
		"exit",
		"label label1",
		"iconst 0",
		"exit"
	};

	return 0==testrun("IFGOTO_main",vmcodes);
}