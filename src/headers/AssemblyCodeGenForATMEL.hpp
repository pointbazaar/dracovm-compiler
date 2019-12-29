#ifndef ASSEMBLYCODEGENFORATMEL_HPP
#define ASSEMBLYCODEGENFORATMEL_HPP

class VMInstr;

namespace AssemblyCodeGenForATMEL{

std::map<std::string,std::vector<std::string>> compile_vmcodes_atmel(std::map<std::string,std::vector<std::string>> vm_sources);

std::vector<std::string> compile_vm_instr(VMInstr instr);

//subroutines for translating individual VM Instructions

//constants
std::vector<std::string> iconst(VMInstr instr);
std::vector<std::string> cconst(VMInstr instr);

//stack related
std::vector<std::string> pop(VMInstr instr);
std::vector<std::string> push(VMInstr instr);
std::vector<std::string> dup(VMInstr instr);
std::vector<std::string> swap(VMInstr instr);

//subroutine related
std::vector<std::string> subroutine(VMInstr instr);
std::vector<std::string> call(VMInstr instr);
std::vector<std::string> _return(VMInstr instr);
std::vector<std::string> _exit(VMInstr instr);


//integer arithmetic commands
std::vector<std::string> iadd(VMInstr instr);
std::vector<std::string> isub(VMInstr instr);
std::vector<std::string> imul(VMInstr instr);
std::vector<std::string> idiv(VMInstr instr);
std::vector<std::string> imod(VMInstr instr);
std::vector<std::string> ineg(VMInstr instr);

//logic commands
std::vector<std::string> _and(VMInstr instr);
std::vector<std::string> _not(VMInstr instr);
std::vector<std::string> _or(VMInstr instr);

//	logic commands, implemented for integers and floating point values

//equal
std::vector<std::string> ieq(VMInstr instr);
//std::vector<std::string> feq(VMInstr instr);	
//will not be implemented, comparing floats for equality is not a good idea


//not equal
std::vector<std::string> ineq(VMInstr instr);
//std::vector<std::string> fneq(VMInstr instr);
//will not be implemented, comparing floats for equality is not a good idea


//greater than
std::vector<std::string> igt(VMInstr instr);
std::vector<std::string> fgt(VMInstr instr);

//greater than or equal
std::vector<std::string> igeq(VMInstr instr);
std::vector<std::string> fgeq(VMInstr instr);

//lesser than
std::vector<std::string> ilt(VMInstr instr);
std::vector<std::string> flt(VMInstr instr);

//lesser than or equal
std::vector<std::string> ileq(VMInstr instr);
std::vector<std::string> fleq(VMInstr instr);

//increment or decrement value on top of stack
std::vector<std::string> inc(VMInstr instr);
std::vector<std::string> dec(VMInstr instr);

//control flow
std::vector<std::string> _goto(VMInstr instr);
std::vector<std::string> if_goto(VMInstr instr);
std::vector<std::string> label(VMInstr instr);

//array related
std::vector<std::string> arraystore(VMInstr instr);
std::vector<std::string> arrayread(VMInstr instr);

//bit shifting, logical
std::vector<std::string> lshiftl(VMInstr instr);
std::vector<std::string> lshiftr(VMInstr instr);

//bit rotations: rotate left, rotate right
std::vector<std::string> ror(VMInstr instr);
std::vector<std::string> rol(VMInstr instr);

}

#endif
