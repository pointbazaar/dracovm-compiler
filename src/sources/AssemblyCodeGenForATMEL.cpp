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
		cout << "AssemblyCodeGen::compile_vm_instr" << endl;
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

//			VMInstr Compilation subroutines:

vector<string> iconst(VMInstr instr){
	//cout << "DEBUG " << instr.arg1 << " stoi" << endl << flush;

	const int i = stoi(instr.arg1);
	return {
		"push dword "+to_string(i)+"	; iconst"
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
		""
	};
}

vector<string> swap(VMInstr instr){
	//swaps the 2 values on top of stack
	return {
		"",
		"; swap:"
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

		
	};
}

vector<string> _return(VMInstr instr){
	return {
		"ret"
	};
}

vector<string> _exit(VMInstr instr){
	return {
		"",
		"; exit:",

	};
}
//https://software.intel.com/en-us/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-1-2a-2b-2c-2d-3a-3b-3c-3d-and-4
//<3 data sheets

//https://stackoverflow.com/questions/37775144/how-to-load-the-value-stored-in-extended-registereax-into-st0-of-floating-poin

//cerr << "NOT IMPLEMENTED" << endl;
//exit(1);


vector<string> iadd(VMInstr instr){
	
	return {
		"",
		";  iadd : "
	};
}

vector<string> isub(VMInstr instr){
	return {
		"",
		";	isub: "
	};
}

vector<string> imul(VMInstr instr){
	return {
		"",
		"; imul:"
	};
}

vector<string> idiv(VMInstr instr){
	return {
		"",
		"; idiv:"
	};
}

vector<string> iexp(VMInstr instr){
	//calculates the powers.
	//this could also be implemented as an assembly subroutine
	//which is just added to every executable,
	//and is just called.

	unsigned int seed=38293;
	const string unique = to_string(rand_r(&seed));

	const string label_calc = ".iexp_calc"+unique;
	const string label_end = ".iexp_end"+unique;

	return {
		"",
		"; iexp:"
	};
}

vector<string> imod(VMInstr instr){
	//https://stackoverflow.com/questions/8021772/assembly-language-how-to-do-modulo
	return {
		"",
		"; imod: "
	};
}

vector<string> ineg(VMInstr instr){
	return {
		"",
		"; ineg:"
	};
}

vector<string> _and(VMInstr instr){
	return {
		"",
		"; and:"
	};
}

vector<string> _not(VMInstr instr){
	//logical not
    //https://www.tutorialspoint.com/assembly_programming/assembly_logical_instructions.htm

	const vector<string> res{
		"",
		"; not:",

		
	};
	return res;
}

vector<string> _or(VMInstr instr){
	return {
		"",
		"; or: ",

			
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

			
	};
}


vector<string> ileq(VMInstr instr){
	unsigned int seed=329;
	const string unique = to_string(rand_r(&seed));

	const string label_true = ".eq_push"+unique;
	const string label_end = ".eq_end"+unique;

	return {
		"",
		"; ileq:",

			
	};
}


vector<string> inc(const VMInstr instr){
	//increments the value on top of stack
	
	return {
		
	};
}

vector<string> dec(VMInstr instr){
	//decrements the value on top of stack

	return {
		
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

	};
}

vector<string> lshiftl(VMInstr instr){
	const string comment = "	;lshiftl";
	const vector<string> res{
		"",
		"; lshiftl:",

		
	};
	return res;
}

vector<string> lshiftr(VMInstr instr){
	const string comment = "	;lshiftl";
	const vector<string> res{
		"",
		"; lshiftr:",

		
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
		
	};
}

vector<string> rol(VMInstr instr){
	//example: rol 3
	//rotates the bits of the element on top of stack to the left by the specified amount of bits
	string n = instr.arg1;
	
	return {
		"",
		"; rol:",
		
	};
}

}
