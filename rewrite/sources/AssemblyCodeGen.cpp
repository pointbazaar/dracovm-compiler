#include <map>
#include <iostream>
#include <vector>

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