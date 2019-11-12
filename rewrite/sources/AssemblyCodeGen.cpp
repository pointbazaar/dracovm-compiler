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
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> fconst(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> cconst(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> pop(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> push(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> dup(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> swap(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> subroutine(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> call(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> _return(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> exit(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> pushsubroutine(VMInstr instr){
	const string name = instr.arg1;

	return {
		"push "+name+"	;pushsubroutine"
	};
}

vector<string> callfromstack(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}


vector<string> fadd(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> fsub(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> fmul(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
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
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> isub(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> imul(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> idiv(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> imod(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> ineg(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> _and(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
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
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> ieq(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
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
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> label(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> arraystore(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> arrayread(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
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
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}

vector<string> rol(VMInstr instr){
	cerr << "NOT IMPLEMENTED" << endl;
	exit(1);

	vector<string> res;
	return res;
}



