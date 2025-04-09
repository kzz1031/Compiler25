#include <iostream>
#include <string>
#include "config.hh"
 
using namespace std;
void Compiler_Config::print_config() {
    cout << "Compiler Configuration:" << ": " ;
    string s = "address_length";
    cout << s << ": " << compiler_config.at(s) << "; " ;
    s = "memory_alignment";
    cout << s << ": " << compiler_config.at(s) << "; " ;
    s = "int_length";
    cout << s << ": " << compiler_config.at(s) << "; " ;
    s = "float_length";
    cout << s << ": " << compiler_config.at(s) << "; " ;
    s = "double_length";
    cout << s << ": " << compiler_config.at(s) << endl;
}