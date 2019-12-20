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
  const string SYS_EXIT   = "1";
  const string SYS_FORK   = "2";
  const string SYS_READ   = "3";
  const string SYS_WRITE  = "4";
  const string SYS_OPEN   = "5";
  const string SYS_CLOSE  = "6";
  const string SYS_WAITPID= "7";
  const string SYS_CREAT  = "8";
  //..
  const string SYS_TIME   = "13";
  //..
  const string SYS_MMAP   = "192";

map<string,vector<string>> compile_builtin_subroutines(){
	map<string,vector<string>> asmcodes = {
		{"Builtin_readchar",_readchar()},
		{"Builtin_putchar",_putchar()},
		{"Builtin_putdigit",_putdigit()},
		{"Builtin_new",_new()},
		{"Builtin_free",_free()},
		{"Builtin_len",_len()},
		{"Builtin_abs",_abs()},
		{"Builtin_time",_time()},
		{"Builtin_fopen",_fopen()},
		{"Builtin_fputs",_fputs()}
	};

	return asmcodes;
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
	const vector<string> sub1 = subroutine(VMInstr(
		"subroutine Builtin_putchar 0 args 0 locals"
	));

	const vector<string> push1 = push(VMInstr("push ARG 0"));
	//in the vector literal
	
	const vector<string> res = {
		//prints the Int on top of stack as char to stdout

		join(sub1,"\n"),

	    //access our argument , ARG 0, by pushing it onto the stack
	    //push1
	    join(push1,"\n"),

	    "mov eax,"+SYS_WRITE,	// sys_write syscall
	    "mov ebx,1",	//stdout

	    "mov ecx,esp", //print the Int on the stack

	    //val length
	    "mov edx,1",  	//: value length
	    "int 0x80",		//call kernel

	    

	    //pop ARG 0 which we pushed
	    "pop ecx",

	    //push return value
	    "mov edx,0",	//: push return value
	    "push edx",		//: push return value

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
	const string byte_offset_32bit="4";
	
	const vector<string> res={
		//http://lxr.linux.no/#linux+v3.6/arch/x86/ia32/ia32entry.S#L372
	    //http://man7.org/linux/man-pages/man2/mmap.2.html

	    //TODO: handle the error if memory could not be allocated
	    
	    //malloc receives as an argument the amount of DWORDs to allocate

	    join(subroutine(VMInstr("subroutine Builtin_new 1 args 0 locals")),"\n"),

	    //access our argument, push it onto the stack
	    join(push(VMInstr("push ARG 0")),"\n"),


	    //this is to multiply by 4, so we allocate 32bit units.
	    //pop our argument into ecx
	    //size of segment to be allocated
	    "pop ecx",

	    //push our size for the length-prefix later on
	    //LINKED CODE 2 (they only work together)
	    "push ecx",

	    //increment by 1, as our array should be length-prefixed, and we need a place to store the length
	    "inc ecx",

	    "mov ecx,"+byte_offset_32bit,
	    "imul eax,ecx",
	    "mov ecx,eax",


	    "mov eax,"+SYS_MMAP,
	    "xor ebx,ebx",	//addr=NULL
	    "mov edx,0x7",	//prot = PROT_READ | PROT_WRITE | PROT_EXEC
	    "mov esi,0x22",	//flags=MAP_PRIVATE | MAP_ANONYMOUS
	    "mov edi,-1",	//fd=-1

	    //LINKED CODE 1 (they only work together)
	    "push ebp",	//save ebp as we should not mess with it
	    
	    "mov ebp,0",	//offset=0 (4096*0)
	    "int 0x80",

	     //LINKED CODE 1 (they only work together)
	    "pop ebp",	//restore ebp as we should not mess with it

	    //eax should now contain
	    //the address of the allocated memory segment

	    //put the length-prefix at the start
	    //pop our length-prefix
	    "pop ebx",
	    //LINKED CODE 2 (they only work together)

	    //mov [eax],ebx //store the length at index 0
	    "mov [eax],ebx",

	    //now we push that pointer on the stack
	    "push eax",

	    //we must swap return value with the return address in order to return
	    //(i am so dumb. took me so long to find this.)
	    join(swap(VMInstr("swap")),"\n"),

	    //return from subroutine
	    "ret"
	};

	return res;
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
	//returns the absolute value of an integer
	const vector<string> res={
		//returns the length of a length-prefixed memory segment

		join(subroutine(VMInstr("subroutine Builtin_abs 1 args 0 locals")),"\n"),

	    //access our argument , ARG 0, by pushing it onto the stack
	    join(push(VMInstr("push ARG 0")),"\n"),
	    //valueis now on stack

	    "pop eax",

	    "cmp eax,0",
	    "jl invert",
	    "jmp end",

	    "invert:",
	    "mov ebx,-1",
	    "imul eax,ebx",

	    "end:",

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
	/*
	// ([Char] filename ,PInt accessmode)~>PInt fopen
    //takes 2 arguments: the file name ([Char]), and the access mode (PInt)
    //returns a file descriptor (PInt) which can then be used with fputs to write to a file

    SubroutineFocusedAssemblyCodeGenerator.compile_subroutine("Builtin_fopen",a);

    //access our argument , ARG 1, by pushing it onto the stack
    compile_push(VMCompilerPhases.SEGMENT_ARG,1,a);

    //access our argument , ARG 0, by pushing it onto the stack
    compile_push(VMCompilerPhases.SEGMENT_ARG,0,a);

    //stack:
    //accessmode
    //filename <- esp

    a.mov(eax,SYS_OPEN,"fopen: sys_open");
    a.pop(ebx,"fopen: get the filename [Char] pointer");

    //tood : we should get the syntactic sugar with the strings in dragon
    //to insert a nullbyte at the end. or use a specially prepared string with this syscall
    //because it is a c-style syscall, it expects a '\0' at the end of the filename probably
    //https://stackoverflow.com/questions/8312290/how-to-open-a-file-in-assembler-and-modify-it

    a.add(ebx, byte_offset_32bit,"offset to the start, as arrays are length-prefixed in dragon");
    //ebx now contains the filename argument
    a.pop(ecx,"fopen: access mode");


    a.call_kernel();

    //push return value
    a.push(eax,"fopen: push return value (file descriptor in eax)");

    //we must swap return value with the return address in order to return
    //(i am so dumb. took me so long to find this.)
    compile_swap("swap return address with return value to return",a);

    //return from subroutine
    SubroutineFocusedAssemblyCodeGenerator.compile_return(a);
	*/
	return {};
}
vector<string> _fputs(){
	//https://www.tutorialspoint.com/c_standard_library/c_function_fputs.htm

    //([Char] str, Pint filedescriptor)~>PInt
    //arg 0: string to write
    //arg 1: filedescriptor of the file to write to
    //returns: 0
    const string byte_offset_32_bit = "4";

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
		res << delim;
	}

    return res.str();
}

