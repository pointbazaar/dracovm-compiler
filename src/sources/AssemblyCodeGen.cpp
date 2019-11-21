#include <map>
#include <iostream>
#include <vector>

#include "VMInstr.hpp"
#include "AssemblyCodeGen.hpp"
#include "BuiltinSubroutines.hpp"

using namespace std;

//https://en.wikipedia.org/wiki/X86_instruction_listings
//https://c9x.me/x86/

map<string,vector<string>> compile_vmcodes(map<string,vector<string>> vm_sources){
	
	//DEBUG
	cout << "compile_vmcodes" << endl;

	//parse vm instructions

	map<string,vector<string>> results;

	for(auto const& entry : vm_sources){

		const string filename = entry.first;
		vector<string> vmcodes = entry.second;

		//first vmcode denotes the subroutine,
		//which should be put as the key in the
		//'instrs' map


		const string subr_line = vmcodes.at(0);
		const string tmp = "subroutine ";
		string subr_name = subr_line.substr(tmp.size());
		subr_name = subr_name.substr(0,subr_name.find(" "));
		const string subr_name_without_namespace = 
			subr_name.substr(subr_name.find("_")+1);

		vector<string> asm_cmds;	

		//add startup codes
		asm_cmds.push_back("section .text");
		asm_cmds.push_back("global _start");

		//define our own subroutine label as global,
		//so it can be correctly linked
		asm_cmds.push_back("global "+subr_name);

		//TODO: define the external labels that 
		//might be jumped to, but are not in this file
		for(auto const& vmcmd : vmcodes){
			const VMInstr instr(vmcmd);
			if(
				instr.cmd.compare("call")==0
				|| instr.cmd.compare("pushsubroutine")==0
			){
				asm_cmds.push_back("extern "+instr.arg1);
			}
		}

		//if this is the main subroutine, it should have the start label
		if(subr_name_without_namespace.compare("main")==0){
			asm_cmds.push_back("_start:");
		}

		for(auto const& vmcmd : vmcodes){

			VMInstr instr(vmcmd);
			//compile
			vector<string> asms = compile_vm_instr(instr);
			asm_cmds.insert(asm_cmds.end(),asms.begin(),asms.end());
		}

		//add the builtin subroutines
		//TODO: every subroutine should have its own .asm file,
		//and the builtin subroutines also
		const vector<string> builtins = compile_builtin_subroutines();
		asm_cmds.insert(asm_cmds.end(),builtins.begin(),builtins.end());

		results[subr_name]=asm_cmds;
	}


	return results;
}

vector<string> compile_vm_instr(VMInstr instr){


	//returns the assembly codes generated for a given
	//vm instruction.
	const string cmd=instr.cmd; //short for command

	//DEBUG
	//cout << cmd << endl;

	//create a map of function pointers
	map<string,vector<string> (*)(VMInstr)> func_map;

	//store our subroutines there:

	//constants
	func_map["iconst"]=iconst;
	func_map["fconst"]=fconst;
	func_map["cconst"]=cconst;

		
	//stack related
	func_map["pop"]=pop;
	func_map["push"]=push;
	func_map["swap"]=swap;
	func_map["dup"]=dup;

	//float arithmetic
	func_map["fadd"]=fadd;
	func_map["fsub"]=fsub;

	//float comparisons
	func_map["flt"]=flt;
	func_map["fgt"]=fgt;
	//func_map["feq"]=feq;	//not implemented

	//integer arithmetic
	func_map["iadd"]=iadd;
	func_map["isub"]=isub;
	func_map["imod"]=imod;
	func_map["imul"]=imul;
	func_map["idiv"]=idiv;
	func_map["iexp"]=iexp;

	func_map["dec"]=dec;
	func_map["inc"]=inc;

	func_map["lshiftl"]=lshiftl;
	func_map["lshiftr"]=lshiftr;

	//integer comparisons
	func_map["ieq"]=ieq;
	func_map["ineq"]=ineq;
	func_map["ilt"]=ilt;
	func_map["ileq"]=ileq;
	func_map["igeq"]=igeq;
	func_map["igt"]=igt;

	//subroutine related
	func_map["subroutine"]=subroutine;
	func_map["pushsubroutine"]=pushsubroutine;
	func_map["call"]=call;
	func_map["callfromstack"]=callfromstack;
	func_map["if-goto"]=if_goto;
	func_map["goto"]=_goto;
	func_map["return"]=_return;
	func_map["exit"]=exit;
	func_map["label"]=label;

	//array related
	func_map["arrayread"]=arrayread;
	func_map["arraystore"]=arraystore;

	//logic
	func_map["and"]=_and;
	func_map["or"]=_or;
	func_map["not"]=_not;

	
	if(func_map.count(cmd)==1){
		return func_map[cmd](instr);
	}else{
	
		cerr << "FATAL: (AssemblyCodeGen.cpp) "<< instr.cmd << " not supported" << endl;
		exit(1);
		return {};
	}
}

//			VMInstr Compilation subroutines:

vector<string> iconst(VMInstr instr){
	//cout << "DEBUG " << instr.arg1 << " stoi" << endl << flush;

	const int i = stoi(instr.arg1);
	return {
		"push dword "+to_string(i)+"	; iconst"
	};
}

vector<string> fconst(VMInstr instr){
	//parse arg
	const float f = stof(instr.arg1);

	return {
		"",
		"; fconst:",

		"mov eax, __float32__("+to_string(f)+")	",
		"push eax"
	};
}

vector<string> cconst(VMInstr instr){
	
	return {
		"push dword '"+instr.arg1+"'	; cconst"		
	};
}

vector<string> pop(VMInstr instr){

	const int byte_offset_32_bit=4;

	if(instr.arg1.compare("")==0 && instr.arg2.compare("")==0){
		//simply pop the value to discard it
		return {
			"pop eax"
		};
	}

	const string segment = instr.arg1;
	const int index      = stoi(instr.arg2);

	if(segment.compare("LOCAL")==0){
		//ebp is on the caller's retun address, with the local variable below

		return {
			"",
			"; pop LOCAL "+to_string(index),
			
			"mov eax,ebp",

			//eax-=1
			"mov ebx,"+to_string(byte_offset_32_bit),
			"sub eax,ebx",

			//eax-=index
			"mov ebx,"+to_string(index*byte_offset_32_bit),
			"sub eax,ebx",

			"pop ebx",
			"mov [eax],ebx"
		};
	}else if(segment.compare("ARG")==0){
		//ebp is on the caller's return address, with the arguments above
		
		return {
			"",
			"; pop ARG "+to_string(index),
			
			"mov eax,"+to_string(2*byte_offset_32_bit),

			"add eax,ebp",
			"add eax,"+to_string(index*byte_offset_32_bit),

			"pop ebx",

			"mov [eax],ebx"
		};
	}else{
		cerr << "FATAL (AssemblyCodeGen.cpp) , instr: " << segment << endl;
		exit(1);
	}
}

vector<string> push(VMInstr instr){
	const string segment = instr.arg1;
	const int index = stoi(instr.arg2);
	const int byte_offset_32_bit=4;

	if(segment.compare("LOCAL")==0){
		//locals are on the stack in order
		//ebp is on the caller's return address, with the local variables below

		//eax=ebp-index-1
		return {
			"",
			";	push LOCAL "+to_string(index),
			
			"mov eax,ebp",
			
			"mov ebx,"+to_string(byte_offset_32_bit),
			"sub eax,ebx",

			"mov ebx,"+to_string(index*byte_offset_32_bit),
			"sub eax,ebx",
			
			"mov eax,[eax]",
			"push eax"
		};
		
	}else if(segment.compare("ARG")==0){
		//arguments are on the stack in reverse order
		//ebp is on the caller's return address, with the arguments above
		
		//eax=ebp+index+2*(4 byte)

		return {
			"",
			"; push ARG "+to_string(index),

			"mov eax,"+to_string(2*byte_offset_32_bit),
			"add eax,ebp",
			"add eax,"+to_string(index*byte_offset_32_bit),

			"mov eax,[eax]",
			
			"push eax"
		};
	}else{
		cerr << "FATAL" << endl;
		exit(1);
	}
}

vector<string> dup(VMInstr instr){
	return {
		"",
		"mov eax, [esp]  ;dup",
		"push eax        ;dup"
	};
}

vector<string> swap(VMInstr instr){
	//swaps the 2 values on top of stack
	return {
		"",
		"; swap:",

		"pop eax",
		"pop ebx",
		"push eax",
		"push ebx"
	};
}

vector<string> subroutine(VMInstr instr){
	const string full_name = instr.arg1;
	const string subr_name = full_name.substr(full_name.find("_")+1);
	//   *
	//to provide a base to reference arguments and local variables
    //ebp will point to the return address of the caller
    //above it there are the current subroutines arguments,
    //below it are its local variables

	if(subr_name.compare("main")==0){
		//we need not save ebp of the caller, as there is no caller
      	//our ebp is iconst 0, to reference local variables, we need an ebp
		return {
			"",
			"; subroutine :",

			//jump label for the subroutine
			full_name+":",

			//fake return address for main
			"mov eax,0",
			"push eax",

			//*
			"mov ebp,esp"
		};
	}else{
		//save the ebp of the caller
      	//push ebp
		return {

			"",
			"; subroutine:",
			//jump label for the subroutine
			full_name+":",

			"push ebp",
			//swap
			"pop ebx",
			"pop eax",
			"push ebx",
			"push eax",

			//*
			"mov ebp,esp"
		};
	}
}

vector<string> call(VMInstr instr){
	string subr_name = instr.arg1;
	return {
		"",
		"; call:",

		"call "+subr_name,

		//swap return value with saved ebp
		"pop eax",
		"pop ebx",
		"push eax",
		"push ebx",

		//get our ebp back
		"pop ebp"
	};
}

vector<string> _return(VMInstr instr){
	return {
		"ret"
	};
}

vector<string> exit(VMInstr instr){

	return {
		"",
		"; exit:",

		"mov eax,1",	//system call number: (sys_exit)	
		"pop ebx",		//pop exit code from stack
		"int 0x80"		//interrupt
	};
}

////https://cs.lmu.edu/~ray/notes/nasmtutorial/
////https://stackoverflow.com/questions/8201613/printing-a-character-to-standard-output-in-assembly-x86


vector<string> pushsubroutine(VMInstr instr){
	const string name = instr.arg1;

	return {
		"push "+name+"	;pushsubroutine"
	};
}

vector<string> callfromstack(VMInstr instr){

	return {
		"",
		"; callfromstack :",

		"pop eax",
		"call eax",

		//swap return value with saved ebp	
		"pop ebx",
		"pop eax",
		"push ebx",
		"push eax",

		//get our ebp back
		"pop ebp"	
	};
}

//https://software.intel.com/en-us/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-1-2a-2b-2c-2d-3a-3b-3c-3d-and-4
//<3 data sheets

//https://stackoverflow.com/questions/37775144/how-to-load-the-value-stored-in-extended-registereax-into-st0-of-floating-poin


vector<string> fadd(VMInstr instr){
	//https://stackoverflow.com/questions/11853133/adding-floating-point-double-numbers-in-assembly
	//https://gist.github.com/nikAizuddin/0e307cac142792dcdeba

	return {
		"",
		"; fadd:",

		//init floating point unit
		"finit",

		//load from stack
		"fld dword [esp]",
		"pop eax",
		"fld dword [esp]",
		"pop eax",

		"fadd",

		"push dword 0",			//push unknown value
		"fstp dword [esp]" //fill that value
	};
}

vector<string> fsub(VMInstr instr){
	//https://stackoverflow.com/questions/11853133/adding-floating-point-double-numbers-in-assembly
	//https://gist.github.com/nikAizuddin/0e307cac142792dcdeba

	return {
		"",
		"; fsub:",

		//init floating point unit
		"finit",

		//load from stack
		"fld dword [esp]",
		"pop eax",
		"fld dword [esp]",
		"pop ebx",

		"fsub",
		"push dword 0",			//push unknown value
		"fstp dword [esp]" //fill that value
	};
}

vector<string> fmul(VMInstr instr){
	//https://stackoverflow.com/questions/11853133/adding-floating-point-double-numbers-in-assembly
	return {
		"",
		"; fmul:",

		"finit",

		"fld dword [esp]",
		"pop eax",
		"fld dword [esp]",
		"pop eax",
		"fmul",
		//push temp value
		"push 0", 
		//fill that value
		"fstp dword [esp]"
	};
}

vector<string> fdiv(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> fmod(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> fneg(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> iadd(VMInstr instr){
	//https://c9x.me/x86/html/file_module_x86_id_5.html

	return {
		"",
		";  iadd : ",

		"pop eax",
		"pop ebx",
		"add eax,ebx",
		"push eax"
	};
}

vector<string> isub(VMInstr instr){
	return {
		"",
		";	isub: ",

		"pop ebx",
		"pop eax",
		"sub eax,ebx",
		"push eax"
	};
}

vector<string> imul(VMInstr instr){

	return {
		"",
		"; imul:",

		"pop ebx",
		"pop eax",
		"imul eax,ebx",
		"push eax"		
	};
}

//https://www.cs.virginia.edu/~evans/cs216/guides/x86.html

vector<string> idiv(VMInstr instr){

	return {
		"",
		"; idiv:",

		"pop ecx",	//pop the divisor
		"pop eax",	//pop the dividend

		"xor edx,edx",	//dividend high half := 0

		//sign-extend eax in edx, in case eax is negative
		//https://www.aldeid.com/wiki/X86-assembly/Instructions/cdq
		"cdq",

		//using idiv instead of div, because 
		//idiv should also work for signed values
		"idiv ecx",
		"push eax"
	};
}

vector<string> iexp(VMInstr instr){
	//calculates the powers.
	//this could also be implemented as an assembly subroutine
	//which is just added to every executable,
	//and is just called.

	const string unique = to_string(rand());

	const string label_calc = ".iexp_calc"+unique;
	const string label_end = ".iexp_end"+unique;

	return {
		"",
		"; iexp:",

		"pop ecx",	//pop the power
		"pop eax",	//pop the base

		//we can use power=ecx as a counter
		//we can use edx as accumulator
		"mov edx,1",

		label_calc+":",
		"imul edx,eax",
		"dec ecx",

		"cmp ecx,0",
		"jz "+label_end,
		"jmp "+label_calc,


		label_end+":",
		"push edx"
	};
}

vector<string> imod(VMInstr instr){
	//https://stackoverflow.com/questions/8021772/assembly-language-how-to-do-modulo
	
	return {
		"",
		"; imod: ",

		"pop ebx",
		"pop eax",
		"xor edx,edx",
		//sign-extend eax in edx, in case eax is negative
		//https://www.aldeid.com/wiki/X86-assembly/Instructions/cdq
		"cdq",
		"idiv ebx",
		"push edx"
	};
}

vector<string> ineg(VMInstr instr){
	return {
		"",
		"; ineg:",

		"pop eax",
		"mov ebx,-1",
		"imul eax,ebx",
		"push eax"
	};
}

vector<string> _and(VMInstr instr){

	return {
		"",
		"; and:",

		"pop ebx",
		"pop eax",
		"and eax,ebx",
		"push eax"	
	};
}

vector<string> _not(VMInstr instr){

	//logical not
    //if there is a 1 (true) on the stack,
    // after this vm command, there must be a 0 (false) on the stack
    //if there is a 0 (false) on the stack, 
    //after this vm command, there must be a 1 (true) on the stack

    //https://www.tutorialspoint.com/assembly_programming/assembly_logical_instructions.htm

	const vector<string> res{
		"",
		"; not:",

		"mov ebx,1",
		"pop eax",
		"xor eax, ebx",
		"push eax"
	};
	return res;
}

vector<string> _or(VMInstr instr){
	return {
		"",
		"; or: ",

		"pop eax",
		"pop ebx",
		"or eax,ebx",
		"push eax"		
	};
}

vector<string> ieq(VMInstr instr){
	const string unique = to_string(rand());

	const string label_true = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;

	return {
		"",
		"; ieq:",

		//pop operands
		"pop eax",
		"pop ebx",
		
		"cmp eax,ebx",
		
		"je "+label_true,
		"push 0", 				//push false
		"jmp "+label_end,

		label_true+":",
		"push 1", 				//push true

		label_end+":"		
	};
}


vector<string> igt(VMInstr instr){
	const string unique = to_string(rand());

	const string label_true = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;

	return {
		"",
		"; igt:",

		//pop operands
		"pop ebx",
		"pop eax",
		
		"cmp eax,ebx",
		
		"jg "+label_true,
		"push 0", 				//push false
		"jmp "+label_end,

		label_true+":",
		"push 1", 				//push true

		label_end+":"		
	};
}

vector<string> fgt(VMInstr instr){

	//https://gist.github.com/nikAizuddin/0e307cac142792dcdeba
	const string BITMASK_FLOATING_POINT_ONLY_CONDITION_FLAGS = "0100011100000000B";

    const string BITMASK_FLOAT_GREATER = "0000000000000000B";
	const string unique = to_string(rand());

	const string label_true = ".fgt_push"+unique;
	const string label_end = ".fgt_end"+unique;
	
	return {
		"",
		"; fgt:",


		"finit",

		"fld dword [esp]",
		"pop eax",
		"fld dword [esp]",
		"pop eax",

		"fcomp",

		//store status of comparison
		"fstsw ax",

		"and eax,"+BITMASK_FLOATING_POINT_ONLY_CONDITION_FLAGS,
		"cmp eax,"+BITMASK_FLOAT_GREATER,

		"je "+label_true,
		"push 0",
		"jmp "+label_end,

		label_true+":",
		"push 1",

		label_end+":",
	};
}

//https://stackoverflow.com/questions/8201613/printing-a-character-to-standard-output-in-assembly-x86

vector<string> igeq(VMInstr instr){
	const string unique = to_string(rand());

	const string label_true = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;

	return {
		"",
		"; igeq:",

		//pop operands
		"pop eax",
		"pop ebx",
		
		"cmp eax,ebx",
		
		"jge "+label_true,
		"push 0", //push false
		"jmp "+label_end,

		label_true+":",
		"push 1", //push true

		label_end+":"		
	};
}

vector<string> fgeq(VMInstr instr){
	const string unique = to_string(rand());

	const string label_true = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;
	
	return {
		"",
		"; fgeq:",

		"finit",

		"fld dword [esp]",
		"pop eax",
		"fld dword [esp]",
		"pop eax",

		"fcomp",

		//store status of comparison
		"fstsw ax",

		"jge "+label_true,
		"push 0",
		"jmp "+label_end,

		label_true+":",
		"push 1",

		label_end+":",
	};
}

vector<string> ineq(VMInstr instr){
	const string unique = to_string(rand());

	const string label_true = ".ineq_push"+unique;
	const string label_end = ".ineq_end"+unique;

	return {
		"",
		"; ineq:",

		//pop operands
		"pop eax",
		"pop ebx",
		
		"cmp eax,ebx",
		
		"jne "+label_true,
		"push 0", 				//push false
		"jmp "+label_end,

		label_true+":",
		"push 1", 				//push true

		label_end+":"		
	};
}


vector<string> ilt(VMInstr instr){
	const string unique = to_string(rand());

	const string label_true = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;

	return {
		"",
		"; ilt:",

		//pop operands
		"pop ebx",
		"pop eax",
		
		"cmp eax,ebx",
		
		"jl "+label_true,
		"push 0", //push false
		"jmp "+label_end,

		label_true+":",
		"push 1", //push true

		label_end+":"		
	};
}

vector<string> flt(VMInstr instr){

	//https://gist.github.com/nikAizuddin/0e307cac142792dcdeba

    const string BITMASK_LESS = "0000000100000000B";
	const string BITMASK_FLOATING_POINT_ONLY_CONDITION_FLAGS = "0100011100000000B";

	const int unique = rand();
	const string label_less = ".flt_less"+to_string(unique);
	const string label_end = ".flt_end"+to_string(unique);
	
	/*
	return {
		"",
		"; flt:",

		"finit",

		"fld dword [esp]",
		"pop eax",

		"fld dword [esp]",
		"pop eax",

		"fcom st0,st1",

		//store status of comparison
		"fstsw ax",

		"and eax,"+BITMASK_FLOATING_POINT_ONLY_CONDITION_FLAGS,
		"cmp eax,"+BITMASK_LESS,
		"je "+label_less,

		"push 0",
		"jmp "+label_end,

		label_less+":",
		"push 1",

		label_end+":",
	};*/
	return {
		"finit",
		"movss xmm0, [esp]",
		"pop eax",

		"movss xmm1, [esp]",
		"pop eax",

		"ucomiss xmm0,xmm1",

		"jb "+label_less,
		"push 0",
		"jmp "+label_end,

		label_less+":",
		"push 1",

		label_end+":",
	};
}

vector<string> ileq(VMInstr instr){
	const int unique = rand();

	const string label_true = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;

	return {
		"",
		"; ileq:",

		//pop operands
		"pop eax",
		"pop ebx",
		
		"cmp eax,ebx",
		
		"jle "+label_true,
		"push 0", //push false
		"jmp "+label_end,

		label_true+":",
		"push 1", //push true

		label_end+":"		
	};
}

vector<string> fleq(VMInstr instr){
	const int unique = rand();
	const string label_true = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;
	
	return {
		"",
		"; fleq:",


		"finit",

		"fld dword [esp]",
		"pop eax",
		"fld dword [esp]",
		"pop eax",

		"fcomp",

		//store status of comparison
		"fstsw ax",

		"jle "+label_true,
		"push 0",
		"jmp "+label_end,

		label_true+":",
		"push 1",

		label_end+":",
	};
}

vector<string> inc(const VMInstr instr){
	//increments the value on top of stack
	
	//maybe this can work. maybe it can be done
	//in a single instruction
	return {
		"inc dword [esp] ;	inc"
	};
}

vector<string> dec(VMInstr instr){
	//decrements the value on top of stack

	//maybe this can work. maybe it can be done
	//in a single instruction
	return {
		"dec dword [esp]	;dec"
	};
}

vector<string> _goto(VMInstr instr){
	const string label = instr.arg1;
	return {
		"",
		"; goto:",

		"jmp "+label
	};
}

vector<string> if_goto(VMInstr instr){

	const string subr_name = instr.arg1;
	return {
		"",
		"; if-goto:",

		//pop condition
		"pop eax",
		//ebx=true
		"mov ebx, 1",
		//is the condition true?
		"cmp eax,ebx",
		"je "+subr_name
	};
}

vector<string> label(VMInstr instr){
	const string label=instr.arg1;
	return {
		label+":"
	};
}

vector<string> arraystore(VMInstr instr){

	const int byte_offset_32_bit = 4;
	return {
		"",
		"; arraystore:",
		
		"pop ebx", //value to store
		"pop ecx", //index

		//increment the index, as our arrays are length-prefixed 
		//in dragon
		"inc ecx",
		"lea ecx, [ecx*"+to_string(byte_offset_32_bit)+"]",
		"pop eax", //array address

		//address to store at = array_address + index
		"add eax, ecx",
		"mov [eax], ebx"
	};
}

vector<string> arrayread(VMInstr instr){

	/*
	arrayread, stack looks like:
	|undefined
	|array address
	|array index <- esp

	after execution of this command, stack looks like:
	|undefined
	|array[index] <- esp

	meaning this vm command reads from the array at the specified index,
	and places the value on top of the stack
	*/
	const int byte_offset_32_bit = 4;
	return {
		"",
		";arrayread:",

		"pop ebx",
		//increment the address, as our arrays are length-prefixed in dragon
		"inc ebx",

		
		"lea ebx, [ebx*"+to_string(byte_offset_32_bit)+"]",
		"pop eax",
		"add eax,ebx",

		//dereference eax
		"mov eax,[eax]",
		"push eax"
	};
}

vector<string> lshiftl(VMInstr instr){
	const string comment = "	;lshiftl";
	const vector<string> res{
		"",
		"; lshiftl:",

		"pop ecx"+comment,
		"pop eax"+comment,
		"shl eax, cl"+comment,
		"push eax"+comment
	};
	return res;
}

vector<string> lshiftr(VMInstr instr){
	const string comment = "	;lshiftl";
	const vector<string> res{
		"",
		"; lshiftr:",

		"pop ecx"+comment,
		"pop eax"+comment,
		"shr eax, cl"+comment,
		"push eax"+comment
	};
	return res;
}

vector<string> ror(VMInstr instr){
	//example: ror 3
	//rotates the bits of the element on top of stack to the right by the specified amount of bits
	string n = instr.arg1;

	return {
		"",
		"; ror:",
		"pop eax",
		"ror eax, "+n,
		"push eax"
	};
}

vector<string> rol(VMInstr instr){
	//example: rol 3
	//rotates the bits of the element on top of stack to the left by the specified amount of bits
	string n = instr.arg1;
	
	return {
		"",
		"; rol:",
		"pop eax",
		"rol eax, "+n,
		"push eax"
	};
}



