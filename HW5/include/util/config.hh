#ifndef __CONFIG_HH__
#define __CONFIG_HH__
#include <map>
#include <stdexcept>

using namespace std;

//this is the global configuration for the compiler
//mostly for the sizes of the data types
//and the alignment of the memory.
//To use it, just include this header and call Compiler_Config::get("key"). 
//No need to instantiate it.

static const map<string, int> compiler_config = {
        {"address_length", 4},  // Length of an address in bytes
        {"memory_alignment", 4},  // memory alignment boundary at multiple of
        {"int_length", 4},
        {"float_length", 4},
        {"double_length", 8}
};

class Compiler_Config {
public:
    // Disable copy and assignment
    Compiler_Config() = delete;
    Compiler_Config& operator=(const Compiler_Config&) = delete;

    // Access values from the configuration
    static int get(const std::string& key) {
        if (compiler_config.find(key) == compiler_config.end()) {
            throw std::runtime_error("Key '" + key + "' not found in configuration");
        } else
            return compiler_config.at(key);
    }
    static void print_config();
};

#endif
