#ifndef QUADFLOW_FLOWINFO_HH
#define QUADFLOW_FLOWINFO_HH

#include <map>
#include <set>
#include <string>
#include <vector>
#include "quad.hh"

// Data structures for dominator information
class ControlFlowInfo {
    public:
        quad::QuadFuncDecl* func; //blocked quad program
        map<int, quad::QuadBlock*> labelToBlock;     // Maps label to block
        int entryBlock; // Entry block of the function

        set<int> allBlocks; // Set of all blocks in the function
        set<int> unreachableBlocks; // Set of unreachable blocks

        //int is the label number, representing a block in a QuadFuncDecl
        map<int, set<int>> predecessors;      // Maps block to blocks that are predecessors 
        map<int, set<int>> successors;      // Maps block to blocks that are successors
        map<int, set<int>> dominators;      // Maps block to blocks are its dominators
        map<int, int> immediateDominator;   // Maps block to its immediate dominator
        map<int, set<int>> dominanceFrontiers; // Maps block to its dominance frontier blocks
        map<int, set<int>> domTree;      // Dominator tree (a node to its children blocks)

        ControlFlowInfo(quad::QuadFuncDecl *func): func(func) {
            entryBlock = -1;
            if (func->quadblocklist != nullptr && !func->quadblocklist->empty()) {
                entryBlock = func->quadblocklist->at(0)->entry_label->num;
            }
            if (entryBlock == -1) {
               cerr << "Error: Entry block not found in function" << endl;
            }
            for (auto block : *func->quadblocklist) {
                if (block->entry_label) {
                    labelToBlock[block->entry_label->num] = block;
                }
            }
        }

        void computeAllBlocks();
        void computeUnreachableBlocks();
        void eliminateUnreachableBlocks(); // eliminate unreachable blocks from the function,
                                            // and emptying all the sets and maps above
                                            // since everything may need to be recomputed!

        void printPredecessors();
        void printSuccessors();
        void printDominators();
        void printImmediateDominators();
        void printDominanceFrontier();
        void printDomTree();

        void computePredecessors();
        void computeSuccessors();
        void computeDominators();
        void computeImmediateDominator();
        void computeDominanceFrontiers();
        void computeDomTree();

        void computeEverything();
};

class DataFlowInfo {
    public:
        quad::QuadFuncDecl* func; //blocked quad program
        set<int> allVars; // Set of all variables 
        map<int, set<pair<quad::QuadBlock*, quad::QuadStm*>>> *defs; //where the variable is used
        map<int, set<pair<quad::QuadBlock*, quad::QuadStm*>>> *uses; //where the variable is used
        map<quad::QuadStm*, set<int>> *liveout; //live_in vars of a statement
        map<quad::QuadStm*, set<int>> *livein; //live_in vars of a statment 

        DataFlowInfo(quad::QuadFuncDecl *func) : func(func) {
            allVars.clear();
            defs = new map<int, set<pair<quad::QuadBlock*, quad::QuadStm*>>>();
            uses = new map<int, set<pair<quad::QuadBlock*, quad::QuadStm*>>>();
            liveout = new map<quad::QuadStm*, set<int>>();
            livein = new map<quad::QuadStm*, set<int>>();
        }

        void findAllVars();
        void computeLiveness();
        string printLiveness();
};

set<DataFlowInfo*>* dataFLowProg(quad::QuadProgram* prog);

#endif