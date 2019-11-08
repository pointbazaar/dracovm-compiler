#include <map>
#include <string>
#include <vector>

#include "VMInstr.hpp"

std::map<std::string,std::vector<std::string>> compile_vmcodes(std::map<std::string,std::vector<std::string>> vm_sources);

std::vector<std::string> compile_vm_instr(VMInstr instr);