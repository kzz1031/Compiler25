#define DEBUG
#undef DEBUG

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <algorithm>
#include "temp.hh"
#include "quad.hh"
#include "flowinfo.hh"

using namespace std;
using namespace quad;

void ControlFlowInfo::computeAllBlocks() {
#ifdef DEBUG
    cout << "Computing all blocks in function: " << func->funcname << endl;
    cout << "#blocks = " << func->quadblocklist->size() << endl;
#endif
    // Compute all blocks in the function
    if (func == nullptr || func->quadblocklist == nullptr) {
        return ; // Nothing to do
    }
    //Collect block information
    allBlocks = set<int>(); //empty set
    labelToBlock = map<int, QuadBlock*>(); //empty map

    for (auto block : *func->quadblocklist) {
        if (block->entry_label) {
            allBlocks.insert(block->entry_label->num);
            labelToBlock[block->entry_label->num] = block;
        }
    }
#ifdef DEBUG
    cout << "All blocks in function: " << func->funcname << endl;
    for (auto block : allBlocks) {
        cout << block << " " << labelToBlock[block]->entry_label->str() << endl;
    }
    cout << endl;
#endif
}

void ControlFlowInfo::computeUnreachableBlocks() {
#ifdef DEBUG
    cout << "Computing unreachable blocks in function: " << func->funcname << endl;
#endif
    // Compute unreachable blocks in the function
    if (func == nullptr || func->quadblocklist == nullptr) {
        return ; // Nothing to do
    }
    unreachableBlocks = set<int>(); //empty set

    if (successors.empty()) computeSuccessors(); //need this

    set<int> visited = set<int>(); //empty set
    set<int> worklist = set<int>(); worklist.insert(entryBlock);
    while (!worklist.empty())
    {   int block = *worklist.begin(); 
        worklist.erase(worklist.begin());
        visited.insert(block);
        for (auto succ : successors[block]) {
            if (visited.find(succ) == visited.end()) {
                worklist.insert(succ);
            }
        }
    }
    if (allBlocks.empty()) computeAllBlocks(); //need this
    unreachableBlocks = set<int>(); //empty set
    for (auto block : allBlocks) {
        if (visited.find(block) == visited.end()) {
            unreachableBlocks.insert(block);
        }
    }
#ifdef DEBUG
    cout << "Unreachable blocks in function: " << func->funcname << endl;
    if (unreachableBlocks.empty()) {
        cout << "None" << endl;
        return;
    } else {
        cout << "Unreachable blocks: ";
    }
    for (auto block : unreachableBlocks) {
        cout << block << " ";
    }
    cout << endl;
#endif
}

void ControlFlowInfo::eliminateUnreachableBlocks() {
#ifdef DEBUG
    cout << "Eliminating unreachable blocks in function: " << func->funcname << endl;
#endif
    // Eliminate unreachable blocks from the function
    if (func == nullptr || func->quadblocklist == nullptr) {
        return ; // Nothing to do
    }
    if (!unreachableBlocks.empty()) computeUnreachableBlocks();
    
    int i=0;
    while (i < func->quadblocklist->size()) {
        int block = func->quadblocklist->at(i)->entry_label->num;
        if (unreachableBlocks.find(block) == unreachableBlocks.end()) {
            i++;
            continue;
        } else {
#ifdef DEBUG
            cout << "Removing block: " << block << endl;
            cout << "Before removing: " << func->quadblocklist->size() << endl;
#endif
            // Remove block from the function's quadlist
            func->quadblocklist->erase(func->quadblocklist->begin() + i);
#ifdef DEBUG
            cout << "After removing: " << func->quadblocklist->size() << endl;
#endif
        }
    }
    //recompute everything if already computed
    if (!allBlocks.empty()) computeAllBlocks();
    if (!unreachableBlocks.empty()) computeUnreachableBlocks();
    if (!successors.empty()) computeSuccessors();
    if (!predecessors.empty()) computePredecessors();
    if (!dominators.empty()) computeDominators();
    if (!immediateDominator.empty()) computeImmediateDominator();
    if (!dominanceFrontiers.empty()) computeDominanceFrontiers();
    if (!domTree.empty()) computeDomTree();
}

static void pMap(map<int, set<int>>& map2set, bool rightarrow) {
    for (auto& pair : map2set) {
        cout << "Block: " << pair.first << (rightarrow?" -> ":" <- ");
        for (auto dom : pair.second) {
            cout << dom << " ";
        }
        cout << endl;
    }
}

void ControlFlowInfo::printPredecessors() {
    cout << "Predecessors:" << endl;
    pMap(predecessors, false); //leftarrow
} 

void ControlFlowInfo::printSuccessors() {
    cout << "Successors:" << endl;
    pMap(successors, true); //rightarrow
} 

void ControlFlowInfo::printDominators() {
    pMap(dominators, false); //leftarrow (been dominated)
}

void ControlFlowInfo::printImmediateDominators() {
    for (auto& pair : immediateDominator) {
        cout << "Block: " << pair.first << " -> " << pair.second << endl;
    }
}

void ControlFlowInfo::printDominanceFrontier() {
    cout << "Dominance Frontier:" << endl;
    pMap(dominanceFrontiers, true); //rightarrow
}

void ControlFlowInfo::printDomTree() {
    cout << "Dominator Tree:" << endl;
    pMap(domTree, true); //rightarrow
} 

void ControlFlowInfo::computePredecessors() {
    // Compute predecessors for each block
    
    if (func == nullptr || func->quadblocklist == nullptr) {
        return ; // Nothing to do
    }

    predecessors = map<int, set<int>>(); //empty set

    for (auto block : *func->quadblocklist) {
        if (!block->entry_label) continue;
        
        for (auto predBlock : *func->quadblocklist) {
            if (!predBlock->exit_labels) continue;
            
            for (auto exitLabel : *predBlock->exit_labels) {
                if (exitLabel->num == block->entry_label->num) {
                    predecessors[block->entry_label->num].insert(predBlock->entry_label->num);
                }
            }
        }
    }
}

void ControlFlowInfo::computeSuccessors() {
    // Compute successors for each block
    if (func == nullptr || func->quadblocklist == nullptr) {
        return ; // Nothing to do
    }

    successors = map<int, set<int>>(); //empty set

    for (auto block : *func->quadblocklist) {
        if (!block->entry_label) continue;
        
        if (block->exit_labels == nullptr || block->exit_labels->empty()) {
            continue; // No exit labels, no successors
        }
        for (auto exitLabel : *block->exit_labels) {
            if (successors[block->entry_label->num].empty()) {
                successors[block->entry_label->num] = set<int>();
            }
            successors[block->entry_label->num].insert(exitLabel->num);
        }
    }
}

void ControlFlowInfo::computeDominators() {
#ifdef DEBUG
    std::cout << "Computing dominators for: " << func->funcname << endl;
#endif
    if (successors.empty()) { //need to have the successors computed first
        computeSuccessors();
    }
    
    dominators = map<int, set<int>>(); //empty set

    // Compute dominators using iterative algorithm

    // Initialize dominators: all blocks dominate all other nodes
    for (auto block : *func->quadblocklist) {
        if (block->entry_label) {
            set<int> allBlocks;
            // Initially, each block is dominated by all blocks (except itself)
            for (auto otherBlock : *func->quadblocklist) {
                if (otherBlock->entry_label) {
                    allBlocks.insert(otherBlock->entry_label->num);
                }
            }
            dominators[block->entry_label->num] = allBlocks;
        }
    }
    
    // Entry block is only dominated by itself
    int entryBlock;
    if (func->quadblocklist && func->quadblocklist->size()>0) {
        if (func->quadblocklist->at(0)->entry_label) {
            entryBlock = func->quadblocklist->at(0)->entry_label->num;
            dominators[entryBlock].clear(); // Clear the entry block's dominator set
            dominators[entryBlock].insert(entryBlock); // Add itself
        } else {
            cerr << "Error: Entry block has no entry label!" << endl;
            return ;
        }
    }

#ifdef DEBUG
    std::cout << "Initial Dominators for: " << func->funcname << endl;
    pMap(dominators, false);
#endif
    
    // Iteratively compute dominators
    bool changed = true;
    int i=0;
    while (changed) {
        changed = false;
        
        for (auto block : *func->quadblocklist) {
            if (!block->entry_label) continue;
            int blockNum = block->entry_label->num;
            
            // Skip entry block
            if (blockNum == entryBlock) continue;
            
            // get the predecessors of this block
            set<int> preds = predecessors[blockNum];
            if (preds.empty()) continue; // No predecessors, can't compute dominators
            
            // Compute new dominator set: intersection of all predecessors' dominators
            set<int> newDom;
            
            bool first = true;
            for (auto pred : preds) {
                if (first) {
                    // For first predecessor, start with its dominator set
                    newDom.insert(dominators[pred].begin(), dominators[pred].end());
                    first = false;
                } else {
                    // For subsequent predecessors, intersect with their dominator sets
                    set<int> intersection= newDom;
                    for (auto dom : newDom) {
                        if (dominators[pred].find(dom) == dominators[pred].end()) {
                            //if not found in the pred's dominator set, remove it
                            intersection.erase(dom);
                        }
                    }
                    newDom = intersection;
                }
            }

            newDom.insert(blockNum); // Add itself
            
            // Check if dominator set has changed
            if (newDom == dominators[blockNum]) { //set equality
                continue; // No change
            } else{
                dominators[blockNum] = newDom;
                changed = true;
            }
        }
    }
#ifdef DEBUG
    std::cout << "Final Dominators for: " << func->funcname << endl;
    pMap(dominators, false);
#endif
}

void ControlFlowInfo::computeImmediateDominator() {
#ifdef DEBUG
    std::cout << "Start to find immediate dominators for: " << func->funcname << endl;
#endif
    
    immediateDominator = map<int, int>(); //empty set

    // Compute immediate dominators for each block
    if (dominators.empty())  computeDominators();
    
    // Initialize immediate dominator for each block
    for (auto& pair : dominators) {
        int block = pair.first;
        immediateDominator[block] = -1; // Default to -1 (no immediate dominator)
    }

    // Compute immediate dominators
    for (auto& pair : dominators) {
        int block = pair.first;
        set<int>& doms = pair.second;
        
        // A block's immediate dominator is the closest dominator
        int idom = -1;
        for (auto dom : doms) {
            if (dom == block) continue; // Skip self
            
            if (idom == -1) {
                idom = dom; // First dominator
            } else {
                // Check if dom dominates current idom
                if (dom != idom && dominators[dom].find(idom) != dominators[dom].end()) {
                    idom = dom;
                }
            }
        }
        
        if (idom!=-1) {
            immediateDominator[block] = idom;
        } else //no dominator other than self (entry node)
            immediateDominator[block] = -1;
    }
#ifdef DEBUG
    cout << "Immediate Dominators for: " << func->funcname << endl;
    for (auto& pair : immediateDominator) {
        cout << "Block: " << pair.first << " -> " << pair.second << endl;
    }
#endif
}

void ControlFlowInfo::computeDomTree() {
    #ifdef DEBUG
        std::cout << "Computing dominator tree for: " << func->funcname << endl;
    #endif
        // Compute dominator tree using immediate dominators
        
        domTree = map<int, set<int>>(); //empty set

        if (immediateDominator.empty()) computeImmediateDominator();
        
        // Initialize dominator tree
        for (auto& pair : immediateDominator) {
            int block = pair.first;
            domTree[block] = set<int>();
        }
        
        // Build the dominator tree
        for (auto& pair : immediateDominator) {
            int block = pair.first;
            int idom = pair.second;
            
            if (idom != -1) {
                domTree[idom].insert(block);
            }
        }
    #ifdef DEBUG
        std::cout << "Final Dominator Tree for: " << func->funcname << endl;
        pMap(domTree, true);
    #endif  
}

void ControlFlowInfo::computeDominanceFrontiers() {
#ifdef DEBUG
    std::cout << "Computing dominance frontier for: " << func->funcname << endl;
#endif

    dominanceFrontiers = map<int, set<int>>(); //empty set

    //get the prerequisites done: successors, dominators, domTree
    if (successors.empty())  computeSuccessors();
    if (predecessors.empty())  computePredecessors();
    if (dominators.empty()) computeDominators();
    if (immediateDominator.empty()) computeImmediateDominator();
    if (domTree.empty()) computeDomTree();

    // Compute dominance frontier for each block
    for (auto block : *func->quadblocklist) {
        if (!block->entry_label) continue;

        int blockNum = block->entry_label->num;
        
        //go over the dominator tree to find it's dominatees
        bool done = false;
        stack<int> worklist; worklist.push(blockNum);
        while (!done) {
            int dom = worklist.top(); worklist.pop();
            for (auto succ : successors[dom]) {
                if (dominators[succ].find(blockNum) == dominators[succ].end() ||
                            succ == blockNum) {
                    // If dom does not dominate succ and not itself, add to dominance frontier
                    if (dominanceFrontiers.find(blockNum) == dominanceFrontiers.end()) {
                        dominanceFrontiers[blockNum] = set<int>();
                    }
                    dominanceFrontiers[blockNum].insert(succ);
                }
            }
            for (auto child : domTree[dom]) {
                worklist.push(child);
            }
            if (worklist.empty()) {
                done = true; // No more dominator tree nodes
            } 
        }
    }
#ifdef DEBUG
    std::cout << "Final Dominance Frontier " << endl;
    pMap(dominanceFrontiers, true);
#endif
}

void ControlFlowInfo::computeEverything() {
    // Compute all control flow information
    computeAllBlocks();
    computeUnreachableBlocks();
    eliminateUnreachableBlocks();

    computePredecessors(); 
#ifdef DEBUG
    printPredecessors();
#endif
    computeSuccessors(); 
#ifdef DEBUG
    printSuccessors();
#endif
    computeDominators(); 
#ifdef DEBUG
    printDominators();
#endif
    computeImmediateDominator();
#ifdef DEBUG
    printImmediateDominators();
#endif
    computeDomTree();
#ifdef DEBUG
    printDomTree();
#endif
    computeDominanceFrontiers();
#ifdef DEBUG
    printDominanceFrontier();
#endif
}
