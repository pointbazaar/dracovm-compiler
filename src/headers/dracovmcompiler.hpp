#include <map>

bool compile_main(std::vector<std::string> filenames, std::vector<std::string> options);

bool compile_main2(std::map<std::string,std::vector<std::string>> vm_sources, std::vector<std::string> options);

std::string asm_filename(std::string filename);
std::string obj_filename(std::string filename);