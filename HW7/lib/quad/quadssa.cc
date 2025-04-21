#define DEBUG
//#undef DEBUG

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <algorithm>
#include <utility>
#include "treep.hh"
#include "quad.hh"
#include "flowinfo.hh"
#include "quadssa.hh"
#include "temp.hh"

using namespace std;
using namespace quad;

// Forward declarations for internal functions
static void deleteUnreachableBlocks(QuadFuncDecl* func, ControlFlowInfo* domInfo);
static void placePhi(QuadFuncDecl* func, ControlFlowInfo* domInfo);
static void renameVariables(QuadFuncDecl* func, ControlFlowInfo* domInfo);
static void cleanupUnusedPhi(QuadFuncDecl* func);

static void deleteUnreachableBlocks(QuadFuncDecl* func, ControlFlowInfo* domInfo) {
    return; // Placeholder for the actual implementation
}
static void placePhi(QuadFuncDecl* func, ControlFlowInfo* domInfo) {
    return; // Placeholder for the actual implementation
}
static void renameVariables(QuadFuncDecl* func, ControlFlowInfo* domInfo) {
    return; // Placeholder for the actual implementation
}
static void cleanupUnusedPhi(QuadFuncDecl* func) {
    return; // Placeholder for the actual implementation
}

QuadProgram *quad2ssa(QuadProgram* program) {
    // Create a new QuadProgram to hold the SSA version
    QuadProgram* ssaProgram = new QuadProgram(static_cast<tree::Program*>(program->node), new vector<QuadFuncDecl*>());
    // Iterate through each function in the original program
    for (auto func : *program->quadFuncDeclList) {
        // Create a new ControlFlowInfo object for the function
        ControlFlowInfo* domInfo = new ControlFlowInfo(func);
        // Compute control flow information
        domInfo->computeEverything();
        
        // Eliminate unreachable blocks
        deleteUnreachableBlocks(func, domInfo);
        
        // Place phi functions
        placePhi(func, domInfo);
        
        // Rename variables
        renameVariables(func, domInfo);
        
        // Cleanup unused phi functions
        cleanupUnusedPhi(func);
        
        // Add the SSA version of the function to the new program
        ssaProgram->quadFuncDeclList->push_back(func);
    }
    //return ssaProgram; //uncomment this line
    return program; //delete this line
}
