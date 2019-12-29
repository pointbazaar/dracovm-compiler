//#include <map>
#include <iostream>
#include <vector>
#include <map>

#include "VMInstr.hpp"
#include "AssemblyCodeGenForATMEL.hpp"
#include "BuiltinSubroutines.hpp"

using namespace std;

namespace AssemblyCodeGenForATMEL{

//https://en.wikipedia.org/wiki/X86_instruction_listings
//https://c9x.me/x86/

map<string,vector<string>> compile_vmcodes_atmel(map<string,vector<string>> vm_sources){
	
	if(DEBUG){
		cout << "AssemblyCodeGenForATMEL::compile_vmcodes" << endl;
	}
	map<string,vector<string>> results;

	for(auto const& entry : vm_sources){
		const string filename = entry.first;
		vector<string> vmcodes = entry.second;

		if(DEBUG){
			cout << " compile vmcodes from " << filename << endl;
		}

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
		results[subr_name]=asm_cmds;
	}
	return results;
}

vector<string> compile_vm_instr(VMInstr instr){
	if(DEBUG){
		cout << "AssemblyCodeGenForATMEL::compile_vm_instr" << endl;
	}
	//returns the assembly codes generated for a given
	//vm instruction.
	const string cmd=instr.cmd; //short for command

	if(DEBUG){
		cout << "compile '" << cmd << "'" << endl;
	}

	//create a map of function pointers
	map<string,vector<string> (*)(VMInstr)> func_map;

	//store our subroutines there:

	//constants
	func_map["iconst"]=iconst;
	func_map["cconst"]=cconst;

		
	//stack related
	func_map["pop"]=pop;
	func_map["push"]=push;
	func_map["swap"]=swap;
	func_map["dup"]=dup;
	
	//these 2 will probably not be implemented,
	//as i found no matching instruction in the book.
	//also, they are not very important
	//func_map["fneq"]=fneq;//not implemented
	//func_map["feq"]=feq;	//not implemented
	//integer arithmetic
	func_map["iadd"]=iadd;
	func_map["isub"]=isub;
	func_map["imod"]=imod;
	func_map["imul"]=imul;
	func_map["idiv"]=idiv;

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
	func_map["call"]=call;
	func_map["if-goto"]=if_goto;
	func_map["goto"]=_goto;
	func_map["return"]=_return;
	func_map["exit"]=_exit;
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
		cerr << "FATAL: (AssemblyCodeGenForATMEL.cpp) "<< instr.cmd << " not supported" << endl;
		exit(1);
		return {};
	}
}

// ---- VMInstr Compilation subroutines -----

vector<string> iconst(VMInstr instr){
	const int i = stoi(instr.arg1);
	return {
		"ldi r16, "+to_string(i),
		"push r16"
	};
}


vector<string> cconst(VMInstr instr){
	const int i= (int)(instr.arg1[0]);
	return {
		"ldi r16, "+to_string(i),
		"push r16"
	};
}

vector<string> pop(VMInstr instr){
	//TODO
	const int byte_offset_32_bit=4;

	if(instr.arg1.compare("")==0 && instr.arg2.compare("")==0){
		//simply pop the value to discard it
		return {
			
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
	//TODO
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
	//TODO
	return {
		""
	};
}

vector<string> swap(VMInstr instr){
	
	//swaps the 2 values on top of stack
	return {
		"",
		"; swap:",
		"pop r16",
		"pop r17",
		"push r16",
		"push r17"
	};
}

vector<string> subroutine(VMInstr instr){
	//TODO
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
		"rcall "+subr_name
	};
}

vector<string> _return(VMInstr instr){
	return {
		"ret"
	};
}

vector<string> _exit(VMInstr instr){
	//because we have no OS, 
	//we cannot really exit. as far as i know.
	return {
		"",
		"; exit:",
		"exit:",
		"rjmp exit"

	};
}
//<3 data sheets

vector<string> iadd(VMInstr instr){
	return {
		"",
		";  iadd : ",
		"pop r16",
		"pop r17",
		"add r16, r17",
		"push r16"
	};
}

vector<string> isub(VMInstr instr){
	return {
		"",
		";	isub: ",
		"pop r16",
		"pop r17",
		"sub r17, r16",
		"push r17"
	};
}

vector<string> imul(VMInstr instr){
	return {
		"",
		"; imul:",
		"pop r16",
		"pop r17",
		"muls r16,r17",
		"push r16"
	};
}

vector<string> idiv(VMInstr instr){
	//TODO
	return {
		"",
		"; idiv:",
		
		"pop r16",
		"pop r17"
	};
}

vector<string> imod(VMInstr instr){
	//https://www.mikrocontroller.net/topic/23001
	
	return {
		"",
		"; imod: ",
		"pop r16",
		"pop r17",
		
		"dec r16",
		"and r17, r16",
		
		"push r17"	//push our result
	};
}

vector<string> ineg(VMInstr instr){
	return {
		"",
		"; ineg:",
		"pop r16",
		"neg r16",
		"push r16"
	};
}

vector<string> _and(VMInstr instr){
	return {
		"",
		"; and:",
		
		"pop r16",
		"pop r17",
		
		"and r16,r17",
		"push r16"
	};
}

vector<string> _not(VMInstr instr){
	//logical not
    
	const vector<string> res{
		"",
		"; not:",
		
		"pop r16",
		
		"com r16",
		
		"push r16"
		
	};
	return res;
}

vector<string> _or(VMInstr instr){
	//logical or
	return {
		"",
		"; or: ",
		
		"pop r16",
		"pop r17",
		
		"or r16,r17",
		
		"push r16"
	};
}

vector<string> ieq(VMInstr instr){
	unsigned int seed=20;
	const string unique = to_string(rand_r(&seed));

	const string label_true = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;

	return {
		"",
		"; ieq:",
		
		"pop r16",
		"pop r17",
		
		"cp r16,r17",
		"breq "+label_true,
		
		//case false:
		"ldi r16,0",
		"rjmp "+label_end,
		
		label_true+":",
		"ldi r16,1",
		
		
		label_end+":",
		"push r16"
	};
}


vector<string> igt(VMInstr instr){
	unsigned int seed=30;
	const string unique = to_string(rand_r(&seed));

	const string label_true = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;

	return {
		"",
		"; igt:",
		
		"pop r16",
		"pop r17",
		
		"cp r16,r17",
		"brpl "+label_true,
		
		//case false:
		"ldi r16,0",
		"rjmp "+label_end,
		
		label_true+":",
		"ldi r16,1",
		
		
		label_end+":",
		"push r16"
	
	};
}


//https://stackoverflow.com/questions/8201613/printing-a-character-to-standard-output-in-assembly-x86

vector<string> igeq(VMInstr instr){
	unsigned int seed=439;
	const string unique = to_string(rand_r(&seed));

	const string label_true = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;

	return {
		"",
		"; igeq:",
		
		"pop r16",
		"pop r17",
		
		"cp r16,r17",
		"brge "+label_true,
		
		//case false:
		"ldi r16,0",
		"rjmp "+label_end,
		
		label_true+":",
		"ldi r16,1",
		
		
		label_end+":",
		"push r16"
	
	};
}

vector<string> ineq(VMInstr instr){
	unsigned int seed=47389;
	const string unique = to_string(rand_r(&seed));

	const string label_true = ".ineq_push"+unique;
	const string label_end = ".ineq_end"+unique;

	return {
		"",
		"; ineq:",

		"pop r16",
		"pop r17",
		
		"cp r16,r17",
		"brne "+label_true,
		
		//case false:
		"ldi r16,0",
		"rjmp "+label_end,
		
		label_true+":",
		"ldi r16,1",
		
		
		label_end+":",
		"push r16"
	};
}


vector<string> ilt(VMInstr instr){
	unsigned int seed=7237;
	const string unique = to_string(rand_r(&seed));

	const string label_true = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;

	return {
		"",
		"; ilt:",

		"pop r16",
		"pop r17",
		
		"cp r16,r17",
		"brlo "+label_true,
		
		//case false:
		"ldi r16,0",
		"rjmp "+label_end,
		
		label_true+":",
		"ldi r16,1",
		
		
		label_end+":",
		"push r16"
	};
}


vector<string> ileq(VMInstr instr){
	unsigned int seed=329;
	const string unique = to_string(rand_r(&seed));

	const string label_false = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;

	return {
		"",
		"; ileq:",

		"pop r16",
		"pop r17",
		
		"cp r16,r17",
		"brpl "+label_false,
		
		//case true:
		"ldi r16,1",
		"rjmp "+label_end,
		
		label_false+":",
		"ldi r16,0",
		
		
		label_end+":",
		"push r16"
	};
}


vector<string> inc(const VMInstr instr){
	//increments the value on top of stack
	
	return {
		"pop r16",
		"inc r16",
		"push r16"
	};
}

vector<string> dec(VMInstr instr){
	//decrements the value on top of stack

	return {
		"pop r16",
		"dec r16",
		"push r16"
	};
}

vector<string> _goto(VMInstr instr){
	const string label = instr.arg1;
	return {

		"rjmp "+label
	};
}

vector<string> if_goto(VMInstr instr){
	const string subr_name = instr.arg1;
	return {
		"",
		"; if-goto:",
		
		"pop r16",
		
		"cpi r16, 0x01",
		"breq "+subr_name
	};
}

vector<string> label(VMInstr instr){
	const string label=instr.arg1;
	return {
		label+":"
	};
}

vector<string> arraystore(VMInstr instr){
	//TODO
	const int byte_offset_32_bit = 4;
	return {
		"",
		"; arraystore:",
		
		
	};
}

vector<string> arrayread(VMInstr instr){
	//TODO
	
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

	};
}

vector<string> lshiftl(VMInstr instr){
	const vector<string> res{
		"",
		"; lshiftl:",
		
		"pop r16",
		"lsl r16",
		"push r16"
	};
	return res;
}

vector<string> lshiftr(VMInstr instr){
	const vector<string> res{
		"",
		"; lshiftr:",

		"pop r16",
		"lsr r16",
		"push r16"
	};
	return res;
}


}
