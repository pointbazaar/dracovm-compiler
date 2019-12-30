//#include <map>
#include <iostream>
#include <vector>
#include <map>

#include "VMInstr.hpp"
#include "AssemblyCodeGenForATMEL.hpp"
#include "BuiltinSubroutines.hpp"

using namespace std;

namespace AssemblyCodeGenForATMEL{

int label_counter=0;

/*
 The ATMEL AVR microcontrollers start at 
 8 bit controllers AFAIK.
 
The Controller simulated by MDX
is also an 8 bit controller.
We want to have pointers as 
arguments and local variables.
These pointers most of the time
do not fit into 8 bit.
Therefore we aligned everything to be 16 bit

after pushing any 8 bit value, we push 0
before popping any 8 bit value, we pop and dump.
* 

when a pointer is on stack,
the low byte of the pointer should be above
the high byte of the pointer.
 
STACK (example) (stack grows down)
|-----
|8 bit value  <- value low
|0			  <- value high (always 0 for 8 bit value)
|-----
|XL	<- pointer low
|XH
|-----
*/

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
		if(DEBUG){
			cout << "adding Startup Codes " << endl;
		}
		
		asm_cmds.push_back(".include \"m32def.inc\"");
		
		//init stack pointer
		asm_cmds.push_back("ldi r16, LOW(RAMEND)");
		asm_cmds.push_back("out SPL, r16");
		asm_cmds.push_back("ldi r16, HIGH(RAMEND)");
		asm_cmds.push_back("out SPH, r16");

		//define our own subroutine label as global,
		//so it can be correctly linked
		
		//asm_cmds.push_back("global "+subr_name);
		
		//TODO: define the external labels that 
		//might be jumped to, but are not in this file
		for(auto const& vmcmd : vmcodes){
			const VMInstr instr(vmcmd);
			if(
				instr.cmd.compare("call")==0
				|| instr.cmd.compare("pushsubroutine")==0
			){
				//TODO: worry about it later
				//asm_cmds.push_back("extern "+instr.arg1);
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
	
	//integer arithmetic
	func_map["iadd"]=iadd;
	func_map["isub"]=isub;
	func_map["imod"]=imod;
	func_map["imul"]=imul;

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

string push_0(){
	return 
	"ldi r19, 0 \npush r19 ";
}

string pop_0(){
	return "pop r19";
}

vector<string> iconst(VMInstr instr){
	const int i = stoi(instr.arg1);
	return {
		"ldi r16, "+to_string(i),
		"push r16",
		push_0()
	};
}


vector<string> cconst(VMInstr instr){
	const int i= (int)(instr.arg1[0]);
	return {
		"ldi r16, "+to_string(i),
		"push r16",
		push_0()
	};
}

vector<string> pop(VMInstr instr){
	if(instr.arg1.compare("")==0 && instr.arg2.compare("")==0){
		//simply pop the value to discard it
		return {
			pop_0(),
			"pop r16"
		};
	}
	const string segment = instr.arg1;
	const int index      = stoi(instr.arg2);
	if(segment.compare("LOCAL")==0){
		//base pointer is on the caller's retun address, with the local variable below
		//subs. eax=X, ebx=Y
		
		return {
			"",
			"; pop LOCAL "+to_string(index),
			
			"mov XL,"+baseptr_low,
			"mov XH,"+baseptr_high,

			//X-=1
			"ldi YL,"+to_string(byte_offset_8_bit),
			"ldi YH, 0",
			
			//just calc for XL,YL, 
			//as it is small difference
			"sub XL,YL",

			//X-=index
			"ldi YL,"+to_string(index*byte_offset_8_bit),
			"ldi YH,0",
			
			//just calc for XL,YL, 
			//as it is small difference
			"sub XL,YL",

			"pop YH",
			"pop YL",
			
			"st X+, YL", //mov [X], YL;  X++
			"st X, YH",
		};
	}else if(segment.compare("ARG")==0){
		//base pointer is on the caller's return address, with the arguments above
		//subs. eax=X, ebx=Y
		return {
			"",
			"; pop ARG "+to_string(index),
			
			"ldi XH,0",
			"ldi XL,"+to_string(2*byte_offset_8_bit),

			//"add eax,ebp",
			//X is not going to have high value
			//use Y to save XL
			"mov YL,XL",
			"mov XL, "+baseptr_low,
			"mov XH, "+baseptr_low,
			//add saved XL
			"add XL,YL",
			
			//http://ww1.microchip.com/downloads/en/devicedoc/atmel-0856-avr-instruction-set-manual.pdf
			"adiw X,"+to_string(index*byte_offset_8_bit),

			//"pop ebx",
			"pop YH",
			"pop YL",

			//"mov [eax],ebx"
			"st X+, YL", //mov [X], YL;  X++
			"st X, YH",
		};
	}else{
		cerr << "FATAL (AssemblyCodeGenForATMEL.cpp) , instr: " << segment << endl;
		exit(1);
	}
}

vector<string> push(VMInstr instr){
	
	const string segment = instr.arg1;
	const int index = stoi(instr.arg2);

	if(segment.compare("LOCAL")==0){
		//locals are on the stack in order
		//base pointer is on the caller's return address, with the local variables below

		//eax=base_pointer - index - 1
		//subs. eax=X, ebx=Y
		return {
			"",
			";	push LOCAL "+to_string(index),
			
			//"mov eax,ebp",
			"mov XL,"+baseptr_low,
			"mov XH,"+baseptr_high,
			
			//"mov ebx,"+to_string(byte_offset_8_bit),
			"ldi YH,0",
			"ldi YL,"+to_string(byte_offset_8_bit),
			
			//"sub eax,ebx",
			"sub XL, YL",
			"sbc XH, YH",

			//"mov ebx,"+to_string(index*byte_offset_8_bit),
			"ldi YH,0",
			"ldi YL,"+to_string(index*byte_offset_8_bit),
			
			//"sub eax,ebx",
			"sub XL, YL",
			"sbc XH, YH",
			
			//"mov eax,[eax]",
			"ld r16, X+",	//X+ is postincrement
			"ld r17, X",
			"mov XL,r16",
			"mov XH,r17",
			
			//"push eax"
			"push XL",
			"push XH"
		};
	}else if(segment.compare("ARG")==0){
		//TODO
		//arguments are on the stack in reverse order
		//base pointer is on the caller's return address, with the arguments above
		
		//eax=base pointer +index+2*(4 byte)
		//subs. eax=X, ebx=Y
		return {
			"",
			"; push ARG "+to_string(index),

			//"mov eax,"+to_string(2*byte_offset_8_bit),
			"ldi XL,"+to_string(2*byte_offset_8_bit),
			"ldi XH,0",
			
			//"add eax,ebp",
			"add XL,"+baseptr_low,
			"adc XH,"+baseptr_high,
			
			//"add eax,"+to_string(index*byte_offset_8_bit),
			"ldi r17,0",
			"ldi r16,"+to_string(index*byte_offset_8_bit),
			"add XL,r16",
			"adc XH,r17",

			//"mov eax,[eax]",
			"ld r16, X+",	//X+ is postincrement
			"ld r17, X",
			"mov XL,r16",
			"mov XH,r17"
			
			//"push eax"
			"push XL",
			"push XH"
		};
	}else{
		cerr << "FATAL" << endl;
		exit(1);
	}
}

vector<string> dup(VMInstr instr){
	return {
		pop_0(),
		"pop r16",
		
		"push r16",
		push_0(),
		"push r16",
		push_0()
	};
}

vector<string> swap(VMInstr instr){
	
	//swaps the 2 values on top of stack
	return {
		"",
		"; swap:",
		
		pop_0(),
		"pop r16",
		
		pop_0(),
		"pop r17",
		
		"push r16",
		push_0(),
		
		"push r17",
		push_0()
	};
}

vector<string> subroutine(VMInstr instr){
	
	const string full_name = instr.arg1;
	const string subr_name = full_name.substr(full_name.find("_")+1);
	
	//to provide a base to reference arguments and local variables
    //ebp will point to the return address of the caller
    //above it there are the current subroutines arguments,
    //below it are its local variables

	if(subr_name.compare("main")==0){
		//we need not save ebp of the caller, as there is no caller
      	//our basepointer is iconst 0, to reference local variables, we need an ebp
		return {
			"",
			"; subroutine :",

			//jump label for the subroutine
			full_name+":",

			//fake return address for main
			"ldi r16,0",
			"push r16",
			push_0(),
 
			//"mov "+baseptr_low+",SPL",
			//"mov "+baseptr_high+",SPH",
			"ldi XL, SPL",
			"clr XH",
			"ld "+baseptr_low+", X+",
			"ld "+baseptr_high+", X"
		};
	}else{
		//save the base pointer of the caller
      	//push base pointer
		return {
			"; subroutine:",
			//jump label for the subroutine
			full_name+":",

			"push "+baseptr_low,
			"push "+baseptr_high,
			
			//swap
			pop_0(),
			"pop r16",
			pop_0(),
			"pop r17",
			"push r16",
			push_0(),
			"push r17",
			push_0(),

			//"mov "+baseptr_low+",SPL",
			//"mov "+baseptr_high+",SPH",
			"ldi XL, SPL",
			"clr XH",
			"ld "+baseptr_low+", X+",
			"ld "+baseptr_high+", X",
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
		"ret"	//ret pops a 16bit address
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
		pop_0(),
		"pop r16",
		
		pop_0(),
		"pop r17",
		
		"add r16, r17",
		
		"push r16",
		push_0()
	};
}

vector<string> isub(VMInstr instr){
	return {
		"",
		";	isub: ",
		pop_0(),
		"pop r16",
		
		pop_0(),
		"pop r17",
		
		"sub r17, r16",
		
		"push r17",
		push_0()
	};
}

vector<string> imul(VMInstr instr){
	return {
		"",
		"; imul:",
		
		pop_0(),
		"pop r16",
		
		pop_0(),
		"pop r17",
		
		"muls r16,r17",
		
		"push r16",
		push_0()
	};
}


vector<string> imod(VMInstr instr){
	//https://www.mikrocontroller.net/topic/23001
	
	return {
		"",
		"; imod: ",
		
		pop_0(),
		"pop r16",
		
		pop_0(),
		"pop r17",
		
		"dec r16",
		"and r17, r16",
		
		"push r17",
		push_0()
	};
}

vector<string> ineg(VMInstr instr){
	return {
		"",
		"; ineg:",
		
		pop_0(),
		"pop r16",
		
		"neg r16",
		
		"push r16",
		push_0()
	};
}

vector<string> _and(VMInstr instr){
	return {
		"",
		"; and:",
		
		pop_0(),
		"pop r16",
		
		pop_0(),
		"pop r17",
		
		"and r16,r17",
		
		"push r16",
		push_0()
	};
}

vector<string> _not(VMInstr instr){
	//logical not
    
	const vector<string> res{
		"",
		"; not:",
		
		pop_0(),
		"pop r16",
		
		"com r16",
		
		"push r16",
		push_0()
	};
	return res;
}

vector<string> _or(VMInstr instr){
	//logical or
	return {
		"",
		"; or: ",
		
		pop_0(),
		"pop r16",
		
		pop_0(),
		"pop r17",
		
		"or r16,r17",
		
		"push r16",
		push_0()
	};
}

vector<string> ieq(VMInstr instr){
	const string unique = to_string(label_counter++);

	const string label_true = "eq_push"+unique;
	const string label_end = "eq_end"+unique;

	return {
		"",
		"; ieq:",
		
		pop_0(),
		"pop r16",
		
		pop_0(),
		"pop r17",
		
		"cp r16,r17",
		"breq "+label_true,
		
		//case false:
		"ldi r16,0",
		"rjmp "+label_end,
		
		label_true+":",
		"ldi r16,1",
		
		
		label_end+":",
		"push r16",
		push_0()
	};
}


vector<string> igt(VMInstr instr){
	const string unique = to_string(label_counter++);

	const string label_true = "eq_push"+unique;
	const string label_end = "eq_end"+unique;

	return {
		"",
		"; igt:",
		
		pop_0(),
		"pop r16",
		
		pop_0(),
		"pop r17",
		
		"cp r16,r17",
		"brpl "+label_true,
		
		//case false:
		"ldi r16,0",
		"rjmp "+label_end,
		
		label_true+":",
		"ldi r16,1",
		
		
		label_end+":",
		"push r16",
		push_0()
	
	};
}


//https://stackoverflow.com/questions/8201613/printing-a-character-to-standard-output-in-assembly-x86

vector<string> igeq(VMInstr instr){
	const string unique = to_string(label_counter++);

	const string label_true = "eq_push"+unique;
	const string label_end = "eq_end"+unique;

	return {
		"",
		"; igeq:",
		
		pop_0(),
		"pop r16",
		
		pop_0(),
		"pop r17",
		
		"cp r16,r17",
		"brge "+label_true,
		
		//case false:
		"ldi r16,0",
		"rjmp "+label_end,
		
		label_true+":",
		"ldi r16,1",
		
		
		label_end+":",
		"push r16",
		push_0()
	
	};
}

vector<string> ineq(VMInstr instr){
	const string unique = to_string(label_counter++);

	const string label_true = "ineq_push"+unique;
	const string label_end = "ineq_end"+unique;

	return {
		"",
		"; ineq:",

		pop_0(),
		"pop r16",
		
		pop_0(),
		"pop r17",
		
		"cp r16,r17",
		"brne "+label_true,
		
		//case false:
		"ldi r16,0",
		"rjmp "+label_end,
		
		label_true+":",
		"ldi r16,1",
		
		
		label_end+":",
		"push r16",
		push_0()
	};
}


vector<string> ilt(VMInstr instr){
	const string unique = to_string(label_counter++);

	const string label_true = "eq_push"+unique;
	const string label_end = "eq_end"+unique;

	return {
		"",
		"; ilt:",

		pop_0(),
		"pop r16",
		
		pop_0(),
		"pop r17",
		
		"cp r16,r17",
		"brlo "+label_true,
		
		//case false:
		"ldi r16,0",
		"rjmp "+label_end,
		
		label_true+":",
		"ldi r16,1",
		
		
		label_end+":",
		"push r16",
		push_0()
	};
}


vector<string> ileq(VMInstr instr){
	const string unique = to_string(label_counter++);

	const string label_false = "eq_push"+unique;
	const string label_end = "eq_end"+unique;

	return {
		"",
		"; ileq:",

		pop_0(),
		"pop r16",
		
		pop_0(),
		"pop r17",
		
		"cp r16,r17",
		"brpl "+label_false,
		
		//case true:
		"ldi r16,1",
		"rjmp "+label_end,
		
		label_false+":",
		"ldi r16,0",
		
		
		label_end+":",
		"push r16",
		push_0()
	};
}


vector<string> inc(const VMInstr instr){
	//increments the value on top of stack
	
	return {
		pop_0(),
		"pop r16",
		
		"inc r16",
		
		"push r16",
		push_0()
	};
}

vector<string> dec(VMInstr instr){
	//decrements the value on top of stack

	return {
		pop_0(),
		"pop r16",
		
		"dec r16",
		
		"push r16",
		push_0()
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
		
		pop_0(),
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



vector<string> lshiftl(VMInstr instr){
	const vector<string> res{
		"",
		"; lshiftl:",
		
		pop_0(),
		"pop r16",
		
		"lsl r16",
		
		"push r16",
		push_0()
	};
	return res;
}

vector<string> lshiftr(VMInstr instr){
	const vector<string> res{
		"",
		"; lshiftr:",

		pop_0(),
		"pop r16",
		
		"lsr r16",
		
		"push r16",
		push_0()
	};
	return res;
}


}
