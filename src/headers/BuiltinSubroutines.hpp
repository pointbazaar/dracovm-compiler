#ifndef BUILTINSUBROUTINES_HPP
#define BUILTINSUBROUTINES_HPP

using namespace std;

	map<string,vector<string>> compile_builtin_subroutines();


	vector<string> _readchar();
	vector<string> _putchar();
	vector<string> _putdigit();
	vector<string> _new();
	vector<string> _free();
	vector<string> _len();
	vector<string> _abs();
	vector<string> _time();
	vector<string> _fopen();
	vector<string> _fputs();


	string join(vector<string> vec, string delim);

#endif
