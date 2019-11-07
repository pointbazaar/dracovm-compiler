#include <string>

using namespace std;

class VMInstr{

	public:
		string cmd;
		string arg1; //may be ""
		string arg2; //may be ""
		VMInstr(string str);
		string toString();
};