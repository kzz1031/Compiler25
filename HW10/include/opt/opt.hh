#ifndef __OPT_HH__
#define __OPT_HH__

#include <iostream>
#include <string>
#include <variant>
#include "quad.hh"
#include "temp.hh"

using namespace std;
using namespace tree;
using namespace quad;

enum  class ValueType {
    NO_VALUE = 0,
    ONE_VALUE = 1,
    MANY_VALUES = 2
};
class RtValue {
    ValueType vt; 
    int value; // the actual value if bct=ONE_VALUE
public:
    RtValue() : vt(ValueType::NO_VALUE), value(0) {} //default value type: no value
    RtValue(ValueType vt) : vt(vt), value(0) {} //only for ONE value and MANY values
    RtValue(int value) : vt(ValueType::ONE_VALUE), value(value) {} //if int, then one value
    ValueType getType() {return vt;}
    int getIntValue() {return value;}
};

class Opt {
public:
    QuadFuncDecl* func;
    map<int, bool> block_executable; //int: entry label num, bool: true: executable, false: not executable
    map<int, RtValue> temp_value; //int: temp num, true: top (many values), false: bottom (no value), int: value
    map<int, QuadBlock*> label2block;

    Opt(quad::QuadFuncDecl* func) : func(func) {}

    //for temp, then return the value type of the temp
    RtValue getRtValue(int temp_num) {
        if (temp_value.find(temp_num) != temp_value.end()) {
            return temp_value[temp_num];
        }
        else {
            return temp_value[temp_num] = RtValue(); 
            //not found, return no value, effectively make all temps to have no value to start with
        }
    }

    //this is to calculate (1) the executable blocks and (2) the temp values
    void calculateBT();
    //this is to change the temp values to consts (if it is const), and remove instructions and blocks when possible
    void modifyFunc();

    QuadFuncDecl* optFunc();

    void printRtValue() {
        for (auto& it : temp_value) {
            cout << "temp: " << it.first ;
            cout << ", type: ";
            if (it.second.getType() == ValueType::NO_VALUE) cout << "NO_VALUE";
            else if (it.second.getType() == ValueType::ONE_VALUE) cout << "ONE_VALUE";
            else if (it.second.getType() == ValueType::MANY_VALUES) cout << "MANY_VALUES";
            if (it.second.getType() == ValueType::ONE_VALUE)
                cout << ", value: " << it.second.getIntValue() << endl;
            else cout << endl;
        }
    }

    void printBlockExecutable() {
        for (auto& it : block_executable) {
            cout << "block: " << it.first << ", executable: " << it.second << endl;
        }
    }
};

QuadProgram* optProg(QuadProgram* prog);

#endif