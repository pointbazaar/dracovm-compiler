#include <map>
#include <iostream>
#include <vector>

#include "VMInstr.hpp"
#include "AssemblyCodeGen.hpp"

using namespace std;

//https://en.wikipedia.org/wiki/X86_instruction_listings

map<string,vector<string>> compile_vmcodes(map<string,vector<string>> vm_sources){
	
	//parse vm instructions

	//map<string,vector<VMInstr>> instrs;
	map<string,vector<string>> results;

	for(auto const& entry : vm_sources){
		string filename = entry.first;
		vector<string> vmcodes = entry.second;

		//first vmcode denotes the subroutine,
		//which should be put as the key in the
		//'instrs' map

		string subr_line = vmcodes.at(0);
		string tmp = "subroutine ";
		string subr_name = subr_line.substr(tmp.size());
		subr_name = subr_name.substr(0,subr_name.find(" "));

		//delete the subroutine declaration
		vmcodes.erase(vmcodes.begin());

		//vector<VMInstr> parsed;

		vector<string> asm_cmds;	
		for(auto const& vmcmd : vmcodes){

			VMInstr instr(vmcmd);
			//compile
			vector<string> asms = compile_vm_instr(instr);
			asm_cmds.insert(asm_cmds.end(),asms.begin(),asms.end());
		}

		//instrs[subr_name]=parsed;
		results[subr_name]=asm_cmds;
	}


	return results;
}

vector<string> compile_vm_instr(VMInstr instr){
	//returns the assembly codes generated for a given
	//vm instruction.

	//string result;
	//switch(instr.cmd){
		/*
		//constants
		case "iconst":
			return iconst(instr);
		case "fconst":
			return fconst(instr);
		case "cconst":
			return cconst(instr);

		//stack related
		case "pop":
			return pop(instr);
		*/

	//}
	//return result;
	vector<string> err;
	cerr << "fatal error" << endl;
	exit(1);
	return err;
}

//			VMInstr Compilation subroutines:

vector<string> iconst(VMInstr instr){
	const int i = stoi(instr.arg1);
	return {
		"push "+to_string(i)+"	; iconst"
	};
}

vector<string> fconst(VMInstr instr){
	//parse arg
	const float f = stof(instr.arg1);
	return {
		"mov eax, __float32__("+to_string(f)+")		;fconst",
		"push eax					;fconst"
	};
}

vector<string> cconst(VMInstr instr){
	
	return {
		"push '"+instr.arg1+"'	; cconst"		
	};
}

vector<string> pop(VMInstr instr){

	const int byte_offset_32_bit=4;

	const string segment = instr.arg1;
	const int index      = stoi(instr.arg2);

	if(segment.compare("LOCAL")==0){
		//ebp is on the caller's retun address, with the local variable below

		return {
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
			"; pop ARG "+to_string(index),
			
			"mov eax,"+to_string(2*byte_offset_32_bit),

			"add eax,ebp",
			"add eax,"+to_string(index*byte_offset_32_bit),

			"pop ebx",

			"mov [eax],ebx"
		};
	}else{
		cerr << "FATAL" << endl;
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
			";	push LOCAL "+to_string(index),
			
			"mov eax,ebp",
			
			"mov ebx,"+to_string(byte_offset_32_bit),
			"sub eax,ebx",

			"mov ebx,"+to_string(index*byte_offset_32_bit),
			"sub eax,ebx"
			
			"mov eax,[eax]",
			"push eax"
		};
		
	}else if(segment.compare("ARG")==0){
		//arguments are on the stack in reverse order
		//ebp is on the caller's return address, with the arguments above
		
		//eax=ebp+index+2*(4 byte)

		return {
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
		"mov eax, [esp]  ;dup",
		"push eax        ;dup"
	};
}

vector<string> swap(VMInstr instr){
	//swaps the 2 values on top of stack
	return {
		"; swap:"
		"pop eax",
		"pop ebx",
		"push eax",
		"push ebx"
	};
}

vector<string> subroutine(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> call(VMInstr instr){
	string subr_name = instr.arg1;
	return {
		"call "+subr_name,

		//swap return value with saved ebp
		"pop eax",
		"pop ebx",
		"push eax",
		"push ebx"

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
		"mov eax,1",	//system call number: (sys_exit)	
		"pop ebx",		//pop exit code from stack
		"int 0x80"		//interrupt
	};
}

vector<string> pushsubroutine(VMInstr instr){
	const string name = instr.arg1;

	return {
		"push "+name+"	;pushsubroutine"
	};
}

vector<string> callfromstack(VMInstr instr){

	return {
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


vector<string> fadd(VMInstr instr){
	//https://stackoverflow.com/questions/11853133/adding-floating-point-double-numbers-in-assembly
	//https://gist.github.com/nikAizuddin/0e307cac142792dcdeba

	return {
		"; START fadd"
		//init floating point unit
		"finit",
		//load from stack
		"fld dword [esp]",
		"pop eax",
		"fld dword [esp]",
		"pop ebx",
		"fadd",
		"push 0",			//push unknown value
		"fstp dword [esp]" //fill that value
		"; END 	 fadd"
	};
}

vector<string> fsub(VMInstr instr){
	//https://stackoverflow.com/questions/11853133/adding-floating-point-double-numbers-in-assembly
	//https://gist.github.com/nikAizuddin/0e307cac142792dcdeba

	return {
		"; START fadd"
		//init floating point unit
		"finit",
		//load from stack
		"fld dword [esp]",
		"pop eax",
		"fld dword [esp]",
		"pop ebx",
		"fsub",
		"push 0",			//push unknown value
		"fstp dword [esp]" //fill that value
		"; END 	 fadd"
	};
}

vector<string> fmul(VMInstr instr){
	//https://stackoverflow.com/questions/11853133/adding-floating-point-double-numbers-in-assembly
	return {
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
		";  iadd : "
		"pop eax",
		"pop ebx",
		"add eax,ebx",
		"push eax"
	};
}

vector<string> isub(VMInstr instr){
	return {
		";	isub: "
		"pop ebx",
		"pop eax",
		"sub eax,ebx",
		"push eax"
	};
}

vector<string> imul(VMInstr instr){

	return {
		"; imul:"
		"pop ebx",
		"pop eax",
		"imul eax,ebx",
		"push eax"		
	};
}

vector<string> idiv(VMInstr instr){

	return {
		"pop ecx",	//pop the divisor
		"pop eax",	//pop the dividend

		"xor edx,edx",	//dividend high half := 0

		//sign-extend eax in edx, in case eax is negative
		//https://www.aldeid.com/wiki/X86-assembly/Instructions/cdq
		"cdq",

		"idiv ecx",
		"push eax"
	};
}

vector<string> imod(VMInstr instr){
	//https://stackoverflow.com/questions/8021772/assembly-language-how-to-do-modulo
	
	return {
		"; -- START imod  -- "
		"pop ebx",
		"pop eax",
		"xor edx,edx",
		//sign-extend eax in edx, in case eax is negative
		//https://www.aldeid.com/wiki/X86-assembly/Instructions/cdq
		"cdq",
		"idiv ebx",
		"push edx",
		"; -- END 	imod --"
	};
}

vector<string> ineg(VMInstr instr){
	return {
		";----- ineg -----",
		"pop eax",
		"mov ebx,-1",
		"imul eax,ebx",
		"push eax"
	};
}

vector<string> _and(VMInstr instr){

	return {
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

	const string comment = "	;not";
	const vector<string> res{
		"mov ebx,1"+comment,
		"pop eax"+comment,
		"xor eax ebx"+comment,
		"push eax"+comment
	};
	return res;
}

vector<string> _or(VMInstr instr){
	return {
		"; or: "
		"pop eax",
		"pop ebx",
		"or eax,ebx",
		"push eax"		
	};
}

vector<string> ieq(VMInstr instr){
	const int unique = rand();

	const string label_true = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;

	return {
		//pop operands
		"pop eax",
		"pop ebx",
		
		"cmp eax,ebx",
		
		"je "+label_true,
		"push 0", //push false
		"jmp "+label_end,

		label_true+":",
		"push 1", //push true

		label_end+":"		
	};
}

vector<string> feq(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}


vector<string> igt(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> fgt(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> igeq(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> fgeq(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}


vector<string> ilt(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> flt(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> ileq(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> fleq(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> inc(const VMInstr instr){
	//increments the value on top of stack
	
	//maybe this can work. maybe it can be done
	//in a single instruction
	return {
		"inc [esp] ;	inc"
	};
}

vector<string> dec(VMInstr instr){
	//decrements the value on top of stack

	//maybe this can work. maybe it can be done
	//in a single instruction
	return {
		"dec [esp]	;dec"
	};
}

vector<string> _goto(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> if_goto(VMInstr instr){

	const string subr_name = instr.arg1;
	return {
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
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> arraystore(VMInstr instr){

	const int byte_offset_32_bit = 4;
	return {
		
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
		"pop ecx"+comment,
		"pop eax"+comment,
		"shl eax cl"+comment,
		"push eax"+comment
	};
	return res;
}

vector<string> lshiftr(VMInstr instr){
	const string comment = "	;lshiftl";
	const vector<string> res{
		"pop ecx"+comment,
		"pop eax"+comment,
		"shr eax cl"+comment,
		"push eax"+comment
	};
	return res;
}

vector<string> ror(VMInstr instr){
	//example: ror 3
	//rotates the bits of the element on top of stack to the right by the specified amount of bits
	string n = instr.arg1;

	return {
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
		"pop eax",
		"rol eax, "+n,
		"push eax"
	};
}



