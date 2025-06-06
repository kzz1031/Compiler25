#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include "quad.hh"
#include "quad2rpi.hh"

using namespace std;
using namespace quad;

//trace the program
void trace(QuadProgram* prog) {
    if (prog == nullptr || prog->quadFuncDeclList == nullptr 
                        || prog->quadFuncDeclList->size() == 0) return;
    for (QuadFuncDecl* func : *prog->quadFuncDeclList) {
        trace(func); //trace each function
    }
}

//stick all blocks together into one block
void trace(QuadFuncDecl* func) {
    // This function is used to trace the function and collect all blocks into one block
    // It also collects the callee registers used and the number of spills to return to the caller
#ifdef DEBUG
    cout << "Tracing function: " << func->name << endl;
#endif
    if (func == nullptr || func->quadblocklist == nullptr 
                        || func->quadblocklist ->size() == 0) return;
    vector<QuadStm*> *sl = new vector<QuadStm*>(); //initializing to collect all stms
    //collect all blocks into allBlocks and allBlockLabels
    map<int, QuadBlock*> allBlocks= map<int, QuadBlock*>();
    set<int> allBlockLabels = set<int>();
    for (QuadBlock *b : *func->quadblocklist) {
        if ( b->quadlist == nullptr || b->quadlist->size()==0) continue;
        int label_num = b->entry_label->num;
        allBlocks[label_num] = b;
        allBlockLabels.insert(label_num);
    }
    //iterate through all blocks, starting with the entry block 
    int currBlock = func->quadblocklist->at(0)->entry_label->num;
    //remember how may time a label being jumped to, and the label stm
    while (allBlockLabels.size() > 0) {
        if (currBlock == -1) break;
        // put all stmts in the current block into sl
        vector<QuadStm*> *bsl = allBlocks[currBlock]->quadlist;
        for (QuadStm *s : *bsl) {
            if (s == nullptr) continue;
            sl->push_back(s);
        }
        allBlockLabels.erase(currBlock); allBlocks.erase(currBlock);
        //check the last statement in the block to decide which block to process next
        QuadStm *last = bsl->back();
        int jump_to_label = -1;
        switch (last->kind) {
            case QuadKind::JUMP: {
                QuadJump *j = static_cast<QuadJump*>(last);
                if (j != nullptr || j->label != nullptr) {
                    jump_to_label = j->label->num;
                } else {
                    cerr << "Error: jump or jump-to-label is nullptr!" << endl;
                    exit(EXIT_FAILURE); //should never happen
                }
            }
            break;
            case QuadKind::CJUMP: {
                    QuadCJump *cj = static_cast<QuadCJump*>(last);
                    if (cj != nullptr || cj->f != nullptr || cj->t != nullptr) {
                        jump_to_label = cj->f->num;
                    } else {
                        cerr << "Error: cjump or false-jump-to-label is nullptr!" << endl;
                        exit(EXIT_FAILURE); //should never happen
                    }
            }
            break;
            default: break;
        } 
        //it's a jump or cjump, so we find the block to jump to as the next block
        if (jump_to_label != -1 && allBlockLabels.find(jump_to_label) != allBlockLabels.end()) {
            currBlock = jump_to_label;
        } else { //the jump-to label is alread processed, or it was not a jump
            //Pick the first unprocessed block
            if (allBlockLabels.size() > 0)
                currBlock = *(allBlockLabels.begin());
            else
                currBlock = -1; //no more blocks to process
        } 
    }
    vector<QuadBlock*> *bl =  new vector<QuadBlock*>();
    QuadBlock *b = new QuadBlock(func->node, sl, func->quadblocklist->at(0)->entry_label, nullptr);
    bl->push_back(b);
    func->quadblocklist = bl;
}
