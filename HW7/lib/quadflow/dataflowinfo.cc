#define DEBUG
#undef DEBUG

#include <iostream>
#include <queue>
#include <algorithm>
#include <map>
#include "quad.hh"
#include "flowinfo.hh"

using namespace std;
using namespace quad;
// Find all variables used or defined in the function
void DataFlowInfo::findAllVars() {
#ifdef DEBUG
    cout << "Finding all variables in function: " << func->funcname << endl;
#endif
    if (!func || !func->quadblocklist) return;
    
    allVars.clear();
    defs->clear();
    uses->clear();
    
    // Iterate through all blocks and statements to collect variables
    for (auto block : *func->quadblocklist) {
        if (!block || !block->quadlist) continue;
        for (auto stmt : *block->quadlist) {
            if (!stmt) continue;
            
            // Add variables from def set
            if (stmt->def) {
                for (auto temp : *stmt->def) {
                    if (temp) {
                        allVars.insert(temp->num);
                        (*defs)[temp->num].insert(pair<QuadBlock*, QuadStm*>(block, stmt));
                    }
                }
            }
            // Add variables from use set
            if (stmt->use) {
                for (auto temp : *stmt->use) {
                    if (temp) {
                        allVars.insert(temp->num);
                        (*uses)[temp->num].insert(pair<QuadBlock*, QuadStm*>(block, stmt));
                    }
                }
            }
        }
    }
#ifdef DEBUG
    cout << "All variables found in function: " << func->funcname << endl;
    for (auto var : allVars) {
        cout << " " << var ;
    }
    cout << endl;
#endif
}

// Calculate both live-in and live-out sets for all statements
void DataFlowInfo::computeLiveness() {
#ifdef DEBUG
    cout << "Computing liveness for function: " << func->funcname << endl;
#endif
    if (!func || !func->quadblocklist) return;
    ControlFlowInfo* domInfo = new ControlFlowInfo(func);
    domInfo->computeEverything();

    // Maps a block to its successor statements
    map<QuadBlock*, vector<QuadStm*>> blockSuccStatements;
    // Maps a statement to the next statement in the same block
    map<QuadStm*, QuadStm*> nextStmtInBlock;
    // Maps a statement to its containing block
    map<QuadStm*, QuadBlock*> stmtToBlock;
    
    // Initialize data structures
    liveout->clear();
    livein->clear();
    
    // Build statement-to-statement mapping within blocks
    for (auto block : *func->quadblocklist) {
        if (!block || !block->quadlist || block->quadlist->empty()) continue;
        
        for (size_t i = 0; i < block->quadlist->size(); i++) {
            QuadStm* stmt = block->quadlist->at(i);
            if (!stmt) continue;
            
            stmtToBlock[stmt] = block;
            
            // Set next statement within block
            if (i < block->quadlist->size() - 1) {
                nextStmtInBlock[stmt] = block->quadlist->at(i + 1);
            } else {
                nextStmtInBlock[stmt] = nullptr;  // Last statement in block
            }
            
            // Initialize empty live-out and live-in sets for this statement
            (*liveout)[stmt] = set<int>();
            (*livein)[stmt] = set<int>();
        }
    }
    
    // Build block-to-successor-statements mapping
    for (auto block : *func->quadblocklist) {
#ifdef DEBUG
        cout << "Processing block: " << block->entry_label->num << endl; 
#endif
        if (!block || !block->quadlist || !block->exit_labels) continue;
        
        blockSuccStatements[block] = vector<QuadStm*>();
        
        // Find blocks that are successors to this block
        for (auto exitLabel : *block->exit_labels) {
            for (auto succBlock : *func->quadblocklist) {
                if (succBlock->entry_label && succBlock->entry_label->num == exitLabel->num) {
                    // First statement of successor block is a successor statement
                    if (!succBlock->quadlist->empty()) {
                        blockSuccStatements[block].push_back(succBlock->quadlist->front());
                    }
                    break;
                }
            }
        }
#ifdef DEBUG
        cout << "Block " << block->entry_label->num << " has successors: ";
        for (auto succStmt : blockSuccStatements[block]) {
            cout << quadKindToString(succStmt->kind) << " ";
        }
        cout << endl;   
#endif
    }
    
    // Iterative dataflow analysis for liveness
    bool changed = true;
    while (changed) {
        changed = false;
        
        // Process blocks in reverse order for faster convergence
        for (auto it = func->quadblocklist->rbegin(); it != func->quadblocklist->rend(); ++it) {
            QuadBlock* block = *it;
            if (!block || !block->quadlist || block->quadlist->empty()) continue;
            
            // Process statements in reverse order
            for (auto stmtIt = block->quadlist->rbegin(); stmtIt != block->quadlist->rend(); ++stmtIt) {
                QuadStm* stmt = *stmtIt;
                if (!stmt) continue;
#ifdef DEBUG
    cout << "Processing statement: " << quadKindToString(stmt->kind) << endl;
#endif
                // remember the old ones
                set<int> oldLiveIn = (*livein)[stmt];
                set<int> oldLiveOut = (*liveout)[stmt]; 
                
                //calculate the new ones livein first: livein = (liveout - def) + use

                set<int> newLiveIn = oldLiveOut; //starting from oldliveout
                // Remove variables defined in this statement
                if (stmt->def) {
                    for (auto temp : *stmt->def) {
                        if (temp) {
                            newLiveIn.erase(temp->num);
#ifdef DEBUG
    cout << "Removing variable " << temp->num << " from live-in set" << endl;   
#endif
                        }
                    }
                }
                
                // Add variables used in this statement
                if (stmt->use) {
                    for (auto temp : *stmt->use) {
                        if (temp) {
                            newLiveIn.insert(temp->num);
#ifdef DEBUG
    cout << "Adding variable " << temp->num << " to live-in set" << endl;
#endif
                        }
                    }
                }
                
                // Update live-in set if changed
                if (newLiveIn != oldLiveIn) {
                    (*livein)[stmt] = newLiveIn;
                    changed = true;
                }
#ifdef DEBUG
    cout << "New live-in for statement " << quadKindToString(stmt->kind) << ": { ";
    for (int var : (*livein)[stmt]) {
        cout << "t" << var << " ";
    } 
    cout << "}" << endl;
#endif
                //now calculate liveout: liveout = union_{s \in succ} livein[s]
                set<int> newLiveOut = set<int>();
                // If this is the last statement in a block, then
                // its live-out is the union of first statements in successor blocks
                if (nextStmtInBlock[stmt] == nullptr) {
                    for (auto succStmt : blockSuccStatements[block]) {
#ifdef DEBUG
    cout << "Next statement (next block) is " << quadKindToString(succStmt->kind) << endl;
#endif
                        // For each successor statement, add its live-in
                        if (livein->find(succStmt) != livein->end()) {
                            for (int var : (*livein)[succStmt]) {
                                newLiveOut.insert(var);
                            }
                        }
                    }
                } else {
                    // Otherwise, live-out is the same as live-in of next statement
                    QuadStm* nextStmt = nextStmtInBlock[stmt];
#ifdef DEBUG
    cout << "Next statement (in block) is " << quadKindToString(nextStmt->kind) << endl;
#endif
                    if (livein->find(nextStmt) != livein->end()) {
                        newLiveOut = (*livein)[nextStmt];
                    }
                }
                
                // Update live-out set if changed
                if (newLiveOut != oldLiveOut) {
                    (*liveout)[stmt] = newLiveOut;
                    changed = true;
                }
            }
        }
#ifdef DEBUG
        cout << "Went through all statements in function: " << func->funcname << endl;
        cout << "Changed: " << changed << endl;
        cout << "Final liveness information for function: " << func->funcname << endl;
        cout << printLiveness() << endl;
#endif
    }
}

// Print liveness information for debugging
string DataFlowInfo::printLiveness() {
#ifdef DEBUG
    cout << "Printing liveness for function: " << func->funcname << endl;
#endif
    if (!func || !liveout || !livein) return "";
    
    string output_str; output_str.reserve(10000);
    output_str += "Liveness information for function: " + func->funcname + "\n";
    
    for (auto block : *func->quadblocklist) {
        if (!block || !block->quadlist) continue;
        
        output_str += "Block ";
        output_str += to_string(block->entry_label->num);
        output_str += ":\n";    
        for (QuadStm* qs: *block->quadlist) {
            if (!qs) continue;
#ifdef DEBUG
            cout << "Printing statement: " << quadKindToString(qs->kind) << endl;
            cout << "livein size=" << (livein->find(qs) != livein->end() ? (*livein)[qs].size() : 0) << endl;
            cout << "liveout size=" << (liveout->find(qs) != liveout->end() ? (*liveout)[qs].size() : 0) << endl;
#endif
            bool first= true;
            output_str += "Statement ";
            output_str += quadKindToString(qs->kind);
            //output_str += ":\n";
            output_str += "  Live-in:  {";
            for (int var : (*livein)[qs]) {
                if (first) {
                    first = false;
                } else {
                    output_str += ", ";
                }
                output_str += "t";
                output_str += to_string(var);
            }
            first=true;
            //output_str += "}\n Live-out: {";
            output_str += "} Live-out: {";
            for (int var : (*liveout)[qs]) {
                if (first) {
                    first = false;
                } else {
                    output_str += ", ";
                }
                output_str += "t";
                output_str += to_string(var);

            }
            output_str +=  "}\n";
        }
    }
    cout << output_str;
    return output_str;
}

set<DataFlowInfo*>* dataFLowProg(QuadProgram* prog) {
    if (!prog || !prog->quadFuncDeclList) return nullptr;
    set<DataFlowInfo*>* allDataFlows = new set<DataFlowInfo*>();
    for (auto func : *prog->quadFuncDeclList) {
        if (!func || !func->quadblocklist) continue;
        
        DataFlowInfo* dfInfo = new DataFlowInfo(func);
        dfInfo->findAllVars();
        dfInfo->computeLiveness();
#ifdef DEBUG
        cout << "Liveness information for function: " << func->funcname << endl;
        cout << dfInfo->printLiveness() << endl;
#endif
        allDataFlows->insert(dfInfo);
    }
    return allDataFlows;
}
