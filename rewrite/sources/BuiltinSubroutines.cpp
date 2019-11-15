#include <map>
#include <iostream>
#include <vector>

#include "BuiltinSubroutines.hpp"
#include "AssemblyCodeGen.hpp"
#include "VMInstr.hpp"

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
	//TODO
	return {};
}
vector<string> _putchar(){
	//TODO
	return {};
}
vector<string> _putdigit(){
	
	vector<string> res = subroutine(VMInstr(
		"subroutine Builtin_putdigit 0 args 0 locals"
	));

	const vector<string> push1 = push(VMInstr("push ARG 0"));

	const vector<string> body = {
		//prints the Int on top of stack as char to stdout

	    //access our argument , ARG 0, by pushing it onto the stack
	    //push1

	    "mov eax,"+SYS_WRITE,	//putdigit: sys_write syscall
	    "mov ebx,1",	//putdigit: stdout

	    //duplicate the value on stack, add offset to make it a char
	    "pop ecx",
	    "push ecx",
	    "add ecx,48", //putdigit: add offset to make it char
	    "push ecx",

	    "mov ecx,esp", //print the Int on the stack

	    //val length
	    "mov edx,1",  	//putdigit: value length
	    "int 0x80",		//call kernel

	    //pop that value which we pushed
	    "pop ecx",

	    //pop ARG 0 which we pushed
	    "pop ecx",

	    //push return value
	    "mov edx,0",	//putdigit: push return value
	    "push edx",		//putdigit: push return value

	    //we must swap return value with the return address in order to return
	    //(i am so dumb. took me so long to find this.)
	    //swap
	    "pop eax",
	    "pop ebx",
	    "push eax",
	    "push ebx",

	    "ret"
	};


	res.insert(res.end(),push1.begin(),push1.end());
	res.insert(res.end(),body.begin(),body.end());

	return res;
}
vector<string> _new(){
	//TODO
	return {};
}
vector<string> _free(){
	//TODO
	return {};
}
vector<string> _len(){
	//TODO
	return {};
}
vector<string> _abs(){
	//TODO
	return {};
}
vector<string> _time(){
	//TODO
	return {};
}
vector<string> _fopen(){
	//TODO
	return {};
}
vector<string> _fputs(){
	//https://www.tutorialspoint.com/c_standard_library/c_function_fputs.htm

    //([Char] str, Pint filedescriptor)~>PInt
    //arg 0: string to write
    //arg 1: filedescriptor of the file to write to
    //returns: 0
    const int byte_offset_32_bit = 4;

    //prints a string to a file
    vector<string> res=subroutine(VMInstr("subroutine Builtin_fputs 2 args 0 locals"));

    //pop pointer to string buffer to write (Arg 0)
    const vector<string> push1 = push(VMInstr("push ARG 0"));

    //pop file descriptor (Arg 1)
    const vector<string> push2 = push(VMInstr("push ARG 1"));

    res.insert(res.end(),push1.begin(),push1.end());
    res.insert(res.end(),push2.begin(),push2.end());

    const vector<string> body = {

	    "mov eax,"+SYS_WRITE,
	    "pop ebx",//pop our filedescriptor argument

	    //print the char on stack
	    "pop ecx", //pop our string to write argument

	    "mov edx,ecx", //how many bytes to write
	    "mov edx,[edx]", //how many bytes to write

	    "add ecx,"+byte_offset_32_bit,
	    "int 0x80",

	    "push 0",

	    //we must swap return value with the return address in order to return
	    //(i am so dumb. took me so long to find this.) 
	    //swap
	    "pop eax",
	    "pop ebx",
	    "push eax",
	    "push ebx",

	    //return from subroutine
	    "ret"
    };

    res.insert(res.end(),body.begin(),body.end());

    return body;
}