#ifndef VMINSTR_HPP
#define VMINSTR_HPP

using namespace std;

class VMInstr{

	public:
		std::string cmd;
		std::string arg1; //may be ""
		std::string arg2; //may be ""
		VMInstr(std::string str);
		std::string toString();
};

#endif
