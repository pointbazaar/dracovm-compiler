#include <map>
#include <iostream>
#include <vector>
#include <numeric>
#include <iterator>
#include <sstream>

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
	
	const vector<string> res={

	    //TODO: handle errors that could occur
		join(subroutine(VMInstr("subroutine Builtin_readchar 0 args 0 locals")),"\n"),

	    //push a placeholder for the value to read on the stack
	    "mov eax,0",
   		"push eax",

	    //SYSCALL START
	    "mov eax,"+SYS_READ,	//sys_read
	    "mov ebx,0",			//stdin

	    //print the char on stack
	    //read into the placeholder DWORD we pushed onto the stack
	    "mov ecx,esp",	
	    


	    //val length
	    "mov edx,1",
	    "int 0x80",
	    //SYSCALL END

	    //push return value: we do not need to push a return value,
	    //as we already pushed a placeholder where our char (which has been read by now)
	    // should have been placed

	    //we must swap return value with the return address in order to return
	    //(i am so dumb. took me so long to find this.)
	    join(swap(VMInstr("swap")),"\n"),

	    //return from subroutine
	    "ret"
	};

	return res;
}
vector<string> _putchar(){
	//TODO
	return {};
}
vector<string> _putdigit(){
	
	const vector<string> sub1 = subroutine(VMInstr(
		"subroutine Builtin_putdigit 0 args 0 locals"
	));

	const vector<string> push1 = push(VMInstr("push ARG 0"));
	//in the vector literal
	
	const vector<string> res = {
		//prints the Int on top of stack as char to stdout

		join(sub1,"\n"),

	    //access our argument , ARG 0, by pushing it onto the stack
	    //push1
	    join(push1,"\n"),

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

	const vector<string> res={
		//returns the length of a length-prefixed memory segment

		join(subroutine(VMInstr("subroutine Builtin_len 1 args 0 locals")),"\n"),

	    //access our argument , ARG 0, by pushing it onto the stack
	    join(push(VMInstr("push ARG 0")),"\n"),

	    //pointer is now on stack
	    "pop eax",

	    //mov eax,[eax] //get the length stored at that location
	    "mov eax,[eax]",

	    //push return value
	    "push eax",

	    //we must swap return value with the return address in order to return
	    //(i am so dumb. took me so long to find this.)
	    join(swap(VMInstr("swap")),"\n"),

	    //return from subroutine
	    "ret"

	};

	return res;
}
vector<string> _abs(){
	//TODO
	return {};
}
vector<string> _time(){
	
	const vector<string> res={
		//time in seconds since the epoch
	    //https://fresh.flatassembler.net/lscr/

	    //takes 0 arguments
	    //returns a PInt

	    join(subroutine(VMInstr("subroutine Builtin_time 0 args 0 locals")),"\n"),

	    "mov eax,"+SYS_TIME,
	    "mov ebx,0",			//return time only in eax
	    "xor ecx,ecx",
	    "xor edx,edx",
	    "int 0x80",


	    //push return value
	    "push eax",		

	    //we must swap return value with the return address in order to return
	    //(i am so dumb. took me so long to find this.)
	    join(swap(VMInstr("swap")),"\n"),

	    //return from subroutine
	    "ret"
	};

	return res;
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
    vector<string> sub1=subroutine(VMInstr("subroutine Builtin_fputs 2 args 0 locals"));

    //pop pointer to string buffer to write (Arg 0)
    const vector<string> push1 = push(VMInstr("push ARG 0"));

    //pop file descriptor (Arg 1)
    const vector<string> push2 = push(VMInstr("push ARG 1"));


    const vector<string> res = {

    	join(sub1,"\n"),

    	join(push1,"\n"),

    	join(push2,"\n"),

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

    return res;
}



string join(vector<string> vec, string delim)
{
	stringstream res;

	for(int i=0;i<vec.size();i++){
		res << vec.at(i);
		if(i<vec.size()-1){
			res << delim;
		}
	}

    return res.str();
}

