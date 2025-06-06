#ifndef __COLOR_HH
#define __COLOR_HH
#include <iostream>
#include <string>
#include <set>
#include <map>
#include "config.hh"
#include "tinyxml2.hh"

using namespace std;
using namespace tinyxml2;

//bad idea below: to fix the number of callee saved registers to push into the stack
//this is to make coding easier, but not a good idea. Can be improved.
#define BYTES_FOR_CALLEE_REGS 36 // bytes for callee saved registers (r4-r10, sp, lr, 9*4=36) in the stack frame

class Color {
public:
    string funcname;
    map<int, int> color_map;
    set<int> spills;
    map<int, int> spill_offset; // spill offset for each spilled temp

    Color(string funcname): funcname(funcname) {
        color_map = map<int, int>();
        spills = set<int>();
    }

    Color(string funcname, map<int, int> color_map, set<int> spills): 
        funcname(funcname), color_map(color_map), spills(spills) {}
    
    void add_color(int temp, int color) {
        color_map[temp] = color;
    }

    void add_spill(int temp) {
        spills.insert(temp);
    }

    int color_of(int temp) {
        if (color_map.find(temp) == color_map.end()) {
            return -1; // spilled temp has no color
        }
        return color_map[temp];
    }

    bool is_spill(int temp) {
        if (spill_offset.size()!=spills.size())  compute_spill_offsets();
        return spills.find(temp) != spills.end();
    }
    
    int get_spill_offset(int temp) {
        if (spill_offset.size()!=spills.size())  compute_spill_offsets();
        if (spill_offset.find(temp) != spill_offset.end()) {
            return spill_offset[temp];
        }
        return -1; // not found
    }

    void compute_spill_offsets() {
        int offset = BYTES_FOR_CALLEE_REGS; //+Compiler_Config::get("int_length");
        for (auto it = spills.begin(); it != spills.end(); ++it) {
            spill_offset[*it] = offset;
            offset += Compiler_Config::get("int_length");
        }   
    }
    void print();
};

class ColorMap {
public:
    //function name to color map
    map<string, Color*> color_map;

    ColorMap() {
        color_map = map<string, Color*>();
    }

    void addColor(string fn, Color* c) {
        if (color_map.find(fn) != color_map.end()) {
            cerr << "Error: Function " << fn << " already exists in the color map." << endl;
            return;
        }
        color_map[fn] = c;
    }

    Color* getColor(string fn) {
        if (color_map.find(fn) == color_map.end()) {
            cerr << "Error: Function " << fn << " not found in the color map." << endl;
            return nullptr;
        }
        return color_map[fn];
    }

    void print();
};

ColorMap* xml2colormap(string fn);

#endif 

    