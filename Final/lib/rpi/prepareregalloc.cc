#define DEBUG
#undef DEBUG

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "quad.hh"
#include "flowinfo.hh"
#include "temp.hh"
#include "prepareregalloc.hh"
#include "coloring.hh"

using namespace std;
using namespace quad;

//This is to prepare a QuadFunc for register allocation, specifically following the ARM convention for interprocedure calls.
// 1) For each function def, push callee saved registers (r4-r11) and fp, lr to the stack add the adjustments to sp and fp
//    and: add instructions that move r0-r3 to the corresponding temps (after the entry label of the first block)
// 2) For each call: it assumes that the function has at most 4 parameters. Also, the call return needs to be in r0, and a move instruction
//    needs to begin the call to move r0 to the original the return value (if it's move_call)
//    Hence, for each (ext)call, add the move instructions to move the parameters to r0-r3, and change the call parameters to r0-r3
//    if it's a move_(ext)call, change the call return to r0 and add a move instruction to move r0 to the original destination
// 3) for each return, add the move instructions to move the return value to r0, and change the return to r0
//    and also adjust the sp and pop out the callee saved registers (r4-r11) and fp, lr
// 4) for each phi node, add the move instructions to move the phi value in the origin block
// 5) for each store, if the src is a constant, move it to a temp and then store it
//      if src is a name, then add an load instruction before it to move the name to a temp
// 6) for each move_binop, if the left is a constant, move it to a temp and then do the binop
//    in addition, for multiplication and division, if the right is a constant, move it to a temp and then do the binop
// 7) for each cjump, if the left or right is a constant, move it to a temp and then do the cjump

void handleEntry(QuadFuncDecl* quadFuncDecl) {
    // for func declarations, add instructions that move r0-r3 to the corresponding parameters 
    // we assume each method has at most 4 parameters
    // the move instructions are added to the beginning of the function (after the entry label of the first block)
    if (quadFuncDecl == nullptr || quadFuncDecl->quadblocklist == nullptr || quadFuncDecl->quadblocklist->empty()) {
        return ; //nothing to do
    }
    QuadBlock* firstBlock = quadFuncDecl->quadblocklist->at(0);
    //Temp* return_add_temp = new Temp(quadFuncDecl->last_temp_num+1); //this is to remember the return value
    //quadFuncDecl->last_temp_num++; //increment the last temp number

    if (quadFuncDecl->params != nullptr && !quadFuncDecl->params->empty()) {
        // Now add the move instructions for parameters
        if (quadFuncDecl->params->size() > 4) {
            cerr << "Error: too many parameters, only 4 are supported!" << endl;
            exit(EXIT_FAILURE);
        }
#ifdef DEBUG
        cout << " -------- Parameters ------- size=" << quadFuncDecl->params->size() << endl;
#endif
        for (int i = 0; i < quadFuncDecl->params->size(); i++) {
            Temp* param = quadFuncDecl->params->at(i);
            if (param != nullptr) {
                // Create a move instruction to move the corresponding register to the parameter
                set<Temp*>* def = new set<Temp*>(); def->insert(new Temp(param->num));
                set<Temp*>* use = new set<Temp*>(); use->insert(new Temp(i));
                QuadMove* move = new QuadMove(nullptr, new TempExp(Type::INT, new Temp(param->num)), 
                                        new QuadTerm(new TempExp(Type::INT, new Temp(i))), def, use);
                            // Here we use INT as the type to simpify the code (as we assume all params are int for now)
                            // This implies that all addresses are int as well (32 bits or 64 bits machines are both ok)
                firstBlock->quadlist->insert(firstBlock->quadlist->begin()+1, move);
            } else {
                cerr << "Error: parameter is null" << endl;
                exit(EXIT_FAILURE);
            }
        }
        //now, change the params of the funcdecl to r0-r3, etc.
        for (int i = 0; i < quadFuncDecl->params->size(); i++) {
            Temp* param = quadFuncDecl->params->at(i);
            if (param != nullptr) {
                param->num = i; //change the temp number to the register number
            } else {
                cerr << "Error: parameter is null" << endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    /*
    // Now add the def of lr to the label of the first block
    QuadLabel* entryLabel = static_cast<QuadLabel*>(firstBlock->quadlist->at(0));
    if (entryLabel != nullptr) {
        //set<Temp*>* def = new set<Temp*>(); def->insert(new Temp(14)); //lr is temp 14
        //entryLabel->def = def;
        //set<Temp*>* new_def = new set<Temp*>(); new_def->insert(return_add_temp); //this is to remember the return value
        //QuadMove* move = new QuadMove(nullptr, new TempExp(Type::PTR, return_add_temp), 
        //                                new QuadTerm(new TempExp(Type::PTR, new Temp(14))), new_def, def);
        //firstBlock->quadlist->insert(firstBlock->quadlist->begin()+1, move);
    } else {
        cerr << "Error: entry label is null" << endl;
        exit(EXIT_FAILURE);
    }
    */
    return ; //return the temp for the return value
}

static void handleReturn(QuadFuncDecl* quadFuncDecl) {
    if (quadFuncDecl == nullptr || quadFuncDecl->quadblocklist == nullptr || quadFuncDecl->quadblocklist->empty()) {
        return; //nothing to do
    }
    // Now for each return instruction: add an instruction before return that 
    // moves the return value to r0 (and change return to return (r0)), and then 
    // add r14 to the return statments
    for (QuadBlock* block : *quadFuncDecl->quadblocklist) {
        if (block != nullptr && block->quadlist != nullptr) {
            int i = block->quadlist->size();
            if (i==0) continue; else i--; //find the position of last stmt
            QuadStm *stmt = block->quadlist->at(i);
            if (stmt != nullptr && stmt->kind == QuadKind::RETURN) {
                QuadReturn* ret = static_cast<QuadReturn*>(stmt);
                if (ret->value != nullptr) {
                    // Create a move instruction to move the return value to r0 
                    set<Temp*>* def = new set<Temp*>(); def->insert(new Temp(0)); // r0
                    set<Temp*>* use = new set<Temp*>(); 
                    QuadTerm* src = ret->value;
                    if (src->kind == QuadTermKind::TEMP) { //if it's a temp, insert into use of this move
                        use->insert(src->get_temp()->temp);
                    } else if (src->kind == QuadTermKind::CONST) {
                        // do nothing
                    } else {
                        cerr << "Error: return value is not a temp or const" << endl;
                        exit(EXIT_FAILURE);
                    }
                    QuadMove* move = new QuadMove(nullptr, new TempExp(Type::INT, new Temp(0)), src, def, use);
                    block->quadlist->insert(block->quadlist->begin()+i, move);
                    ret->value = new QuadTerm(new TempExp(Type::INT, new Temp(0))); // change return value to r0
                } else {
                    cerr << "Error: return value is null!" << endl;
                    exit(EXIT_FAILURE);
                }
                // add r0 and return_address_temp to the use set of the return (nothing else)
                ret->use = new set<Temp*>();
                ret->use->insert(new Temp(0));
            }
            //if not a return (could be anything else), ignore
        }
    }
}

static void handlePhi(QuadFuncDecl* quadFuncDecl) {
    if (quadFuncDecl == nullptr || quadFuncDecl->quadblocklist == nullptr || quadFuncDecl->quadblocklist->empty()) {
        return; //nothing to do
    }
    //if there is a phi node, add the move instructions to move the phi value in the origin block
    for (QuadBlock* block : *quadFuncDecl->quadblocklist) {
        if (block != nullptr && block->quadlist != nullptr) {
            for (QuadStm* stmt : *block->quadlist) {
                if (stmt != nullptr && stmt->kind == QuadKind::PHI) {
                    QuadPhi* phi = static_cast<QuadPhi*>(stmt);
                    if (phi->temp != nullptr) {
                        for (int i=0; i<phi->args->size(); i++) {
                            pair<Temp*, Label*> arg = phi->args->at(i);
                            Temp* arg_temp = arg.first;
                            Label* arg_label = arg.second;
                            // Create a move instruction to move the phi value to r0 
                            set<Temp*>* def = new set<Temp*>(); def->insert(phi->temp->temp);
                            set<Temp*>* use = new set<Temp*>(); use->insert(arg_temp); //change the temp number to the index  
                            QuadMove* move = new QuadMove(nullptr, new TempExp(Type::INT, phi->temp->temp),
                                        new QuadTerm(new TempExp(Type::INT, arg_temp)), def, use);
                            //find the block that has the label arg_label
                            bool found = false;
                            for (QuadBlock* block : *quadFuncDecl->quadblocklist) {
                                if (block->entry_label->num == arg_label->num) { //found the block
                                    //last stmt has to be a jump, so add the move right before it
                                    block->quadlist->insert(block->quadlist->begin()+block->quadlist->size()-1, move); 
                                    found = true;
                                    break; //found the label, break
                                }
                            }
                            if (!found) {
                                //if not found, it means the label is not in the block
                                //this should not happen, but just in case
                                //cerr << "Error: phi label not found!" << endl;
                                //exit(EXIT_FAILURE);
                                //or we simply ignore it (as it should not happen)
                                //but if phi allows a constant argument, we need to do a hack to 
                                //use a move to move the constant to a temp, but this temp is not from 
                                //any predecessor block... so ignore this case is a better solution
                            }
                        }
                    } else {
                        cerr << "Error: phi temp is null!" << endl;
                        exit(EXIT_FAILURE);
                    }
                }
            }
            //now got over the statements again to remove the phi statements
            int i=0;
            while (true) {
                if (i >= block->quadlist->size()) break; //end of the list
                if (block->quadlist->at(i)->kind != QuadKind::PHI) {
                    i++; //not a phi, continue
                    continue;
                }
                block->quadlist->erase(block->quadlist->begin()+i); //remove the phi node
            }
        }
    }
}

static void handleCall(QuadFuncDecl *quadFuncDecl) {
    if (quadFuncDecl == nullptr || quadFuncDecl->quadblocklist == nullptr || quadFuncDecl->quadblocklist->empty()) {
        return; //nothing to do
    }
    // Add r0-r3, lr to the use of all call stmts
    for (QuadBlock* block : *quadFuncDecl->quadblocklist) {
        if (block != nullptr && block->quadlist != nullptr && !block->quadlist->empty()) {
            int i=0;
            while (true) {
                if (i >= block->quadlist->size()) break; //end of the list
                QuadStm *stmt = block->quadlist->at(i);
                if (stmt != nullptr && (stmt->kind == QuadKind::CALL || stmt->kind == QuadKind::EXTCALL 
                                        || stmt->kind == QuadKind::MOVE_CALL || stmt->kind == QuadKind::MOVE_EXTCALL)) {
                    vector<QuadTerm*>* args;
                    QuadTerm* obj = nullptr;
                    if (stmt->kind == QuadKind::CALL) {
                        QuadCall* call = static_cast<QuadCall*>(stmt);
                        args = call->args;
                        obj = call->obj_term;
                    } else if (stmt->kind == QuadKind::EXTCALL) {
                        QuadExtCall* call = static_cast<QuadExtCall*>(stmt);
                        args = call->args;
                    } else if (stmt->kind == QuadKind::MOVE_CALL) {
                        QuadMoveCall* call = static_cast<QuadMoveCall*>(stmt);
                        args = call->call->args;
                        obj = call->call->obj_term;
                    } else if (stmt->kind == QuadKind::MOVE_EXTCALL) {
                        QuadMoveExtCall* call = static_cast<QuadMoveExtCall*>(stmt);
                        args = call->extcall->args;
                    } else {
                        cerr << "Error: unknown call kind!" << endl;
                        exit(EXIT_FAILURE);
                    }
                    //for each arg, insert an instruction to move the arg to the corresponding register
                    // Create a move instruction to move the arg to the corresponding register
                    int j=0;
                    if (args != nullptr && !args->empty()) { //if not empty, add move instructions 
                        for (QuadTerm* arg : *args) {
                            if (j>= 4) {
                                cerr << "Error: Only four parameters are supported!" << endl;
                                break; //only 4 args
                            }
                            if (arg != nullptr) {
                                set<Temp*>* def = new set<Temp*>(); def->insert(new Temp(j));
                                set<Temp*>* use = new set<Temp*>(); 
                                if (arg->kind == QuadTermKind::TEMP) { //if it's a temp, insert into use of this move
                                    use->insert(arg->get_temp()->temp);
                                } else if (arg->kind == QuadTermKind::CONST) {
                                    // do nothing
                                } else {
                                    cerr << "Error: arg is not a temp or const" << endl;
                                    exit(EXIT_FAILURE);
                                }
                                QuadTerm * new_arg = arg->clone(); // clone the arg
                                Type new_type;
                                if (new_arg->kind == QuadTermKind::TEMP) new_type = new_arg->get_temp()->type;
                                else if (new_arg->kind == QuadTermKind::CONST) new_type = Type::INT; //for now, we assume all args are int
                                else {
                                    cerr << "Error: arg is not a temp or const" << endl;
                                    exit(EXIT_FAILURE);
                                }   
                                QuadMove* move = new QuadMove(nullptr, new TempExp(new_type, new Temp(j)), new_arg, def, use);
                                block->quadlist->insert(block->quadlist->begin()+i, move);
                                *arg = *new QuadTerm(new TempExp(new_type, new Temp(j))); // change arg to rj
                            } else {
                                cerr << "Error: arg is null" << endl;
                                exit(EXIT_FAILURE);
                            }
                            j++;
                        }
                    }
                    // Add whatever is used in the args (r0-r3) to the use set of the call
                    stmt->use = new set<Temp*>();
                    for (int ii = 0; ii<j; ii++) {
                        stmt->use->insert(new Temp(ii));
                    }
                    //also need to add obj to the use set of the call
                    if (obj != nullptr) {
                        if (obj->kind == QuadTermKind::TEMP) {
                            stmt->use->insert(obj->get_temp()->temp);
                        } else if (obj->kind == QuadTermKind::CONST) {
                            // do nothing
                        } else {
                            cerr << "Error: obj is not a temp or const" << endl;
                            exit(EXIT_FAILURE);
                        }
                    }
                    // r0-r3, lr also need to be added to the def set of the call (caller saved registers)
                    stmt->def = new set<Temp*>();
                    //stmt->def->insert(new Temp(14)); //lr
                    stmt->def->insert(new Temp(0)); //r0
                    stmt->def->insert(new Temp(1)); //r1
                    stmt->def->insert(new Temp(2)); //r2
                    stmt->def->insert(new Temp(3)); //r3
                    // Now we need to move the index forward, as we have added new instructions
                    i += j; //with j new instructions added, we need to move the index forward
                    if (stmt->kind == QuadKind::MOVE_CALL || stmt->kind == QuadKind::MOVE_EXTCALL) {
                        //add a statement after the call to move the return value r0 to the original destination
                        TempExp* dst = nullptr;
                        TempExp* new_dst = nullptr;
                        if (stmt->kind == QuadKind::MOVE_CALL) {
                            dst = static_cast<QuadMoveCall*>(stmt)->dst;
                        } else if (stmt->kind == QuadKind::MOVE_EXTCALL) {
                            dst = static_cast<QuadMoveExtCall*>(stmt)->dst;
                        }
                        new_dst = new TempExp(dst->type, new Temp(dst->temp->num)); //clone the dst
                        *dst = *new TempExp(dst->type, new Temp(0)); //change dst of the move to r0
                        set<Temp*>* mc_def = new set<Temp*>();  mc_def->insert(new_dst->temp);
                        set<Temp*>* mc_use = new set<Temp*>(); mc_use->insert(new Temp(0)); //r0
                        QuadMove* mv_move = new QuadMove(nullptr, new_dst, new QuadTerm(new TempExp(new_dst->type, new Temp(0))), 
                                                        mc_def, mc_use); 
                        block->quadlist->insert(block->quadlist->begin()+i+1, mv_move);
                        i++; //move the index forward
                    }   
                }
                i++;
            }
        }
    }
}

static void handleOtherStm(QuadFuncDecl *quadFuncDecl) {
    if (quadFuncDecl == nullptr || quadFuncDecl->quadblocklist == nullptr || quadFuncDecl->quadblocklist->empty()) {
        return; //nothing to do
    }
    Temp_map *temp_map = new Temp_map();
    temp_map->next_temp = quadFuncDecl->last_temp_num+1;
    temp_map->next_label = quadFuncDecl->last_label_num+1;

    for (QuadBlock* block : *quadFuncDecl->quadblocklist) {
        if (block != nullptr && block->quadlist != nullptr && !block->quadlist->empty()) {
            int i=0;
            while (true) {
                if (i >= block->quadlist->size()) break; //end of the list
                switch (block->quadlist->at(i)->kind) {
                    case QuadKind::STORE: { //store-src can't be constant. has to move to a temp and then str
                            QuadStore* store = static_cast<QuadStore*>(block->quadlist->at(i));
                            if (store->src->kind == QuadTermKind::CONST) {
                                TempExp *new_t = new TempExp(Type::INT, temp_map->newtemp());
                                //create two instrucitons to replace the current one 
                                set<Temp*> *def = new set<Temp*>(); def->insert(new_t->temp);
                                QuadMove *new_move = new QuadMove(nullptr, new_t, store->src, def, nullptr);
                                if (store->use == nullptr)  store->use = new set<Temp*>();
                                store->use->insert(new_t->temp);
                                QuadStore *new_store = new QuadStore(nullptr, new QuadTerm(new_t), store->dst, nullptr, store->use);
                                //now replace 
                                block->quadlist->insert(block->quadlist->begin()+i, new_store);
                                block->quadlist->insert(block->quadlist->begin()+i, new_move);
                                block->quadlist->erase(block->quadlist->begin()+i+2); //remove the original store
                                i++;
                            } if (store->src->kind == QuadTermKind::MAME) {
                                TempExp *new_t = new TempExp(Type::INT, temp_map->newtemp());
                                //create two instrucitons to replace the current one 
                                set<Temp*> *def = new set<Temp*>(); def->insert(new_t->temp);
                                QuadLoad *new_load = new QuadLoad(nullptr, new_t, store->src, def, nullptr);
                                if (store->use == nullptr)  store->use = new set<Temp*>();  
                                store->use->insert(new_t->temp);
                                QuadStore *new_store = new QuadStore(nullptr, new QuadTerm(new_t), store->dst, nullptr, store->use);
                                //now replace 
                                block->quadlist->insert(block->quadlist->begin()+i, new_store);
                                block->quadlist->insert(block->quadlist->begin()+i, new_load);
                                block->quadlist->erase(block->quadlist->begin()+i+2); //remove the original store
                                i++;
                            }
                        }
                        break;
                    case QuadKind::MOVE_BINOP: {//binop left must be a temp, multiplication must be all temps
                            QuadMoveBinop* move_binop = static_cast<QuadMoveBinop*>(block->quadlist->at(i));
#ifdef DEBUG
                        cout << "========Move binop" << endl;
                        string sss;
                        move_binop->print(sss, 0, true);
                        cout << sss << endl;
#endif
                            TempExp *new_t = nullptr, *new_t2 = nullptr;
                            QuadMove *new_move, *new_move2;
                            if (move_binop->left->kind == QuadTermKind::CONST) {
                                new_t = new TempExp(Type::INT, temp_map->newtemp());
                                set<Temp*> *def = new set<Temp*>(); def->insert(new_t->temp);
                                new_move = new QuadMove(nullptr, new_t, move_binop->left, def, nullptr);
                            } 
#ifdef DEBUG
                            cout << "move_binop->op=" << move_binop->binop << "|||" << endl;
                            cout << "move_binop->left->kind=" << (move_binop->left->kind==QuadTermKind::CONST?"CONST":"TEMP") << endl;
                            cout << "move_binop->right->kind=" << (move_binop->right->kind==QuadTermKind::CONST?"CONST":"TEMP") << endl;
#endif
                            if ((move_binop->binop == "*" || move_binop->binop == "/") && move_binop->right->kind == QuadTermKind::CONST) {
                                    new_t2 = new TempExp(Type::INT, temp_map->newtemp());
                                    set<Temp*> *def2 = new set<Temp*>(); def2->insert(new_t2->temp);
                                    new_move2 = new QuadMove(nullptr, new_t2, move_binop->right, def2, nullptr); 
                            }
                            block->quadlist->erase(block->quadlist->begin()+i);
                            if (new_t != nullptr) {block->quadlist->insert(block->quadlist->begin()+i, new_move); i++;}
                            if (new_t2 != nullptr) {block->quadlist->insert(block->quadlist->begin()+i, new_move2); i++;}
                            if (move_binop->use == nullptr)  move_binop->use = new set<Temp*>();
                            if (new_t != nullptr) move_binop->use->insert(new_t->temp);
                            if (new_t2 != nullptr) move_binop->use->insert(new_t2->temp);
                            QuadMoveBinop *new_move_binop = new QuadMoveBinop(nullptr, move_binop->dst, 
                                (new_t!=nullptr?new QuadTerm(new_t):move_binop->left), move_binop->binop, 
                                    (new_t2!=nullptr?new QuadTerm(new_t2):move_binop->right),
                                    move_binop->def, move_binop->use);
                            block->quadlist->insert(block->quadlist->begin()+i, new_move_binop);
#ifdef DEBUG
                        cout << "new_t=" << (new_t==nullptr?"null":to_string(new_t->temp->num)) << endl;
                        cout << "new_t2=" << (new_t2==nullptr?"null":to_string(new_t2->temp->num)) << endl;
                        cout << "========Move binop" << endl;
#endif
                        }
                        break;
                    case QuadKind::CJUMP: { //both sides must be temps
                            QuadCJump* cjump = static_cast<QuadCJump*>(block->quadlist->at(i));
                            TempExp *new_t = nullptr, *new_t2 = nullptr;
                            QuadMove *new_move, *new_move2;
                            if (cjump->left->kind == QuadTermKind::CONST) {
                                new_t = new TempExp(Type::INT, temp_map->newtemp());
                                set<Temp*> *def = new set<Temp*>(); def->insert(new_t->temp);
                                new_move = new QuadMove(nullptr, new_t, cjump->left, def, nullptr);
                            }
                            if (cjump->right->kind == QuadTermKind::CONST) {
                                new_t2 = new TempExp(Type::INT, temp_map->newtemp());
                                set<Temp*> *def2 = new set<Temp*>(); def2->insert(new_t2->temp);
                                new_move2 = new QuadMove(nullptr, new_t2, cjump->right, def2, nullptr);
                            }
                            if (cjump->use == nullptr)  cjump->use = new set<Temp*>();
                            if (new_t != nullptr) cjump->use->insert(new_t->temp);
                            if (new_t2 != nullptr) cjump->use->insert(new_t2->temp);
                            block->quadlist->erase(block->quadlist->begin()+i);
                            if (new_t != nullptr) {block->quadlist->insert(block->quadlist->begin()+i, new_move); i++;}
                            if (new_t2 != nullptr) {block->quadlist->insert(block->quadlist->begin()+i, new_move2); i++;}
                            QuadCJump *new_cjump = new QuadCJump(nullptr, cjump->relop, 
                                (new_t!=nullptr?new QuadTerm(new_t):cjump->left), 
                                (new_t2!=nullptr?new QuadTerm(new_t2):cjump->right), 
                                cjump->t, cjump->f, cjump->def, cjump->use);
                            block->quadlist->insert(block->quadlist->begin()+i, new_cjump);
                        }   
                        break;
                    default:
                        break;
                }
                i++;
            }
        }
    }
    quadFuncDecl->last_temp_num = temp_map->next_temp-1;
    quadFuncDecl->last_label_num = temp_map->next_label-1;
}

QuadFuncDecl* prepareRegAlloc(QuadFuncDecl* quadFuncDecl) {
    QuadFuncDecl* newFuncDecl = quadFuncDecl->clone();
    handleEntry(newFuncDecl); //handle the return label and return value
    handleReturn(newFuncDecl); //handle the return label and return value
    handlePhi(newFuncDecl); //handle the phi nodes
    handleCall(newFuncDecl); //handle the call instructions
    handleOtherStm(newFuncDecl); //handle the other instructions
    return newFuncDecl;
}

QuadProgram *prepareRegAlloc(QuadProgram* quadProgram) {
    if (quadProgram == nullptr || quadProgram->quadFuncDeclList == nullptr || quadProgram->quadFuncDeclList->empty()) {
        return quadProgram; //nothing to do
    }
    // Prepare the QuadFuncDecl for register allocation
    for (QuadFuncDecl* funcDecl : *quadProgram->quadFuncDeclList) {
        QuadFuncDecl* newFuncDecl = funcDecl->clone();
        *funcDecl = *prepareRegAlloc(newFuncDecl);
    }
    return quadProgram;
}
