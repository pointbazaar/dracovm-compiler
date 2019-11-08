#include <string>
#include <vector>
#include <iostream>
#include <istream>
#include <sstream>
#include <iterator>

#include "VMInstr.hpp"

using namespace std;

VMInstr::VMInstr(const string str){
	//this should split it into strings
	//https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/

	istringstream iss(str);
	vector<string> res((istream_iterator<string>(iss)),
                                 istream_iterator<string>());

	if(res.size()==0){
		cerr << "vm command " << str 
			<< " was empty" << endl;
		exit(1);
	}

	if(res.size()>3){
		cerr << "vm command " << str 
			<< " was too long. max size is 3 words." << endl;
	}

	this->cmd=res.at(0);
	if(res.size()>1){
		this->arg1=res.at(1);
	}else{
		this->arg1="";
	}
	if(res.size()>2){
		this->arg2=res.at(2);
	}else{
		this->arg2="";
	}
}

string VMInstr::toString(){

	string a1 = (this->arg1=="")?"":this->arg1;
	string a2 = (this->arg2=="")?"":this->arg2;
	return this->cmd+" "+a1+" "+a2;
}