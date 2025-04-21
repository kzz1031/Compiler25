#include <iostream>
#include <string>
#include "config.hh"

using namespace std;
void Compiler_Config::print_config() {
    cout << "Config:" << endl;
    string s = "address_length";
    cout << s << ": " << compiler_config.at(s) << endl;
    s = "memory_alignment";
    cout << s << ": " << compiler_config.at(s) << endl;
    s = "int_length";
    cout << s << ": " << compiler_config.at(s) << endl;
    s = "float_length";
    cout << s << ": " << compiler_config.at(s) << endl;
    s = "double_length";
    cout << s << ": " << compiler_config.at(s) << endl;
}