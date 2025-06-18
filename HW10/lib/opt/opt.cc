#define DEBUG
//#undef DEBUG

#include <string>
#include <stack>
#include <variant>
#include <vector>
#include <map>
#include "quad.hh"
#include "opt.hh"

void Opt::calculateBT() {
    //your code here
    //this is to calculate the executable blocks and the temp values
}


void Opt::modifyFunc() {
    //your code here
    //this is to change the temp values to consts (if it is const), and remove instructions and blocks when possible
 
}

QuadFuncDecl* Opt::optFunc() {
    // Initialize the block_executable map
    for (auto& block : *func->quadblocklist) {
        block_executable[block->entry_label->num] = false;
        label2block[block->entry_label->num] = block;
    }

    // Initialize the temp_value map for parameters
    for (auto& temp : *func->params) {
        temp_value[temp->num] = RtValue(ValueType::MANY_VALUES); // Initialize to many values for all parameters
    }

    calculateBT();

    printRtValue(); 
    printBlockExecutable();

    modifyFunc();

    return func;
}

QuadProgram* optProg(QuadProgram* prog) {
    QuadProgram* newProg = new QuadProgram(nullptr, new vector<QuadFuncDecl*>());
    for (int i=0; i < prog->quadFuncDeclList->size(); i++) {
        Opt optthis(prog->quadFuncDeclList->at(i));
        newProg->quadFuncDeclList->push_back(optthis.optFunc());
    }
    return newProg;
}