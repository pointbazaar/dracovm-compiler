#include <map>
#include <vector>

#include "VMInstr.hpp"

using namespace std;

//https://en.wikipedia.org/wiki/X86_instruction_listings

map<string,vector<string>> compile_vmcodes(map<string,vector<string>> vm_sources){
	
	//parse vm instructions

	map<string,vector<VMInstr>> instrs;

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

		vector<VMInstr> parsed;

		for(auto const& vmcmd : vmcodes){

			parsed.push_back(VMInstr(vmcmd));
		}

		instrs[subr_name]=parsed;
	}


	map<string,vector<string>> results;
	//TODO: compile 
	return results;
}
