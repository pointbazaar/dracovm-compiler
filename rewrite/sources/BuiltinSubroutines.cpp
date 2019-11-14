#include <map>
#include <iostream>
#include <vector>

#include "BuiltinSubroutines.hpp"

using namespace std;

 //https://www.informatik.htw-dresden.de/~beck/ASM/syscall_list.html

  //eax values (syscall numbers):
  const int SYS_EXIT   = 1;
  const int SYS_FORK   = 2;
  const int SYS_READ   = 3;
  const int SYS_WRITE  = 4;
  const int SYS_OPEN   = 5;
  const int SYS_CLOSE  = 6;
  const int SYS_WAITPID= 7;
  const int SYS_CREAT  = 8;
  //..
  const int SYS_TIME   = 13;
  //..

vector<string> compile_builtin_subroutines(){
	//TODO
	vector<string> asms;

	vector<vector<string>> asmcodes = {
		_readchar(),
		_putchar(),
		_putdigit(),
		_new(),
		_free(),
		_len(),
		_abs(),
		_time(),
		_fopen(),
		_fputs()
	};

	for(vector<string> asmcode : asmcodes){

		asms.insert(asms.end(),asmcode.begin(),asmcode.end());
	}

	return asms;
}




vector<string> _readchar(){

}
vector<string> _putchar(){

}
vector<string> _putdigit(){

}
vector<string> _new(){

}
vector<string> _free(){

}
vector<string> _len(){

}
vector<string> _abs(){

}
vector<string> _time(){

}
vector<string> _fopen(){

}
vector<string> _fputs(){
	//https://www.tutorialspoint.com/c_standard_library/c_function_fputs.htm

    //([Char] str, Pint filedescriptor)~>PInt
    //arg 0: string to write
    //arg 1: filedescriptor of the file to write to
    //returns: 0
    const int byte_offset_32_bit = 4;

    //prints a string to a file
    return {
    	"subroutine Builtin_fputs 2 args 0 locals",

    	//access our argument , ARG 0, by pushing it onto the stack
	    compile_push(VMCompilerPhases.SEGMENT_ARG,0,a);

	    //access our argument , ARG 1, by pushing it onto the stack
	    compile_push(VMCompilerPhases.SEGMENT_ARG,1,a);

	    "mov eax,"+SYS_WRITE,
	    "pop ebx",//pop our filedescriptor argument

	    //print the char on stack
	    "pop ecx", //pop our string to write argument

	    "mov edx,ecx", //how many bytes to write
	    "mov edx,[edx]", //how many bytes to write

	    "add ecx,"+byte_offset_32_bit,
	    "int 0x80",

	    "push 0"
    };

    //we must swap return value with the return address in order to return
    //(i am so dumb. took me so long to find this.)
    compile_swap("swap return address with return value to return",a);

    //return from subroutine
    SubroutineFocusedAssemblyCodeGenerator.compile_return(a);
}