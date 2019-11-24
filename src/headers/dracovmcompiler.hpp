#include <map>

bool compile_main(std::vector<std::string> filenames,std::string exec_filename);

bool compile_main2(std::map<std::string,std::vector<std::string>> vm_sources, std::string exec_filename);

std::string asm_filename(std::string filename);
std::string obj_filename(std::string filename);