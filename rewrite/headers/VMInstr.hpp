#include <string>

using namespace std;

class VMInstr{

	public:
		const string cmd;
		const string arg1; //may be null
		const string arg2; //may be null
		VMInstr(string str);
		string toString();
};