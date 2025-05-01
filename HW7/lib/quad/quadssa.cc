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

#ifdef DEBUG
#define DEBUG_PRINT(x) cout << x << endl
#else
#define DEBUG_PRINT(x)
#endif

// Forward declarations for internal functions
static void deleteUnreachableBlocks(QuadFuncDecl* func, ControlFlowInfo* domInfo);
static void placePhi(QuadFuncDecl* func, ControlFlowInfo* domInfo);
static void renameVariables(QuadFuncDecl* func, ControlFlowInfo* domInfo);
static void cleanupUnusedPhi(QuadFuncDecl* func);
static void renameQuadTerm(QuadTerm* term, const map<int, stack<int>>& stacks);
static void renameQuadStmAfterDef(QuadStm* stmt, const map<int, stack<int>>& stacks);
static void renameQuadStmAfterUse(QuadStm* stmt, const map<int, stack<int>>& stacks);

static void deleteUnreachableBlocks(QuadFuncDecl* func, ControlFlowInfo* domInfo) {
    for(auto block : *func->quadblocklist) {
        if (block->entry_label && domInfo->unreachableBlocks.find(block->entry_label->num) != domInfo->unreachableBlocks.end()) {
            // Remove the unreachable block from the function
            func->quadblocklist->erase(std::remove(func->quadblocklist->begin(), func->quadblocklist->end(), block), func->quadblocklist->end());
        }
    }
    return; // Placeholder for the actual implementation
}
static void placePhi(QuadFuncDecl* func, ControlFlowInfo* domInfo) {
    
    DataFlowInfo* dfInfo = new DataFlowInfo(func);
    dfInfo->findAllVars();
    dfInfo->computeLiveness();

    map<int, set<int>> defBlocks;
    map<int, set<int>> phiBlocks;

    for (auto block : *func->quadblocklist) {
        if (!block->entry_label || !block->quadlist) continue;
        int blockNum = block->entry_label->num;
        
        for (auto stmt : *block->quadlist) {
            if (!stmt->def) continue;
            for (auto temp : *stmt->def) {
                if (defBlocks.find(temp->num) == defBlocks.end()) {
                    defBlocks[temp->num] = set<int>();
                }
                //DEBUG_PRINT("Adding block " << blockNum << " to defBlocks for temp " << temp->num);
                defBlocks[temp->num].insert(blockNum);
            }
        }
    }
    set<int> var_set;
    for (auto& defPair : defBlocks) {
        int varNum = defPair.first;
        if (var_set.find(varNum) == var_set.end()) {
            var_set.insert(varNum);
        }
    }
    for (auto& defPair : defBlocks) {
        
        Temp* var = new Temp(defPair.first);
        //DEBUG_PRINT("visiting var: "<< var->str());
        set<int>& varDefBlocks = defPair.second;
        
        // worklist 初始化
        set<int> worklist(varDefBlocks.begin(), varDefBlocks.end());
        while (!worklist.empty()) {
            int block = *worklist.begin();
            worklist.erase(worklist.begin());
            
            // visit dominance frontiers
            for (int dfBlock : domInfo->dominanceFrontiers[block]) {
                bool needPhi = false;
                
                QuadStm* entryStmt = domInfo->labelToBlock[dfBlock]->quadlist->front();
                
                // 检查变量在入口语句是否为 liveout
                if (phiBlocks[dfBlock].find(var->num) == phiBlocks[dfBlock].end() &&
                    dfInfo->liveout->find(entryStmt) != dfInfo->liveout->end() &&
                    dfInfo->liveout->at(entryStmt).find(var->num) != dfInfo->liveout->at(entryStmt).end()) {
                    needPhi = true;
                    phiBlocks[dfBlock].insert(var->num);
                }

                if (needPhi) {
                    // 在 dfBlock 对应的基本块开头插入 phi 函数
                    QuadBlock* insertBlock = domInfo->labelToBlock[dfBlock];
                    if (!insertBlock || !insertBlock->quadlist) continue;
                    
                    //DEBUG_PRINT("InsertBlock: " << dfBlock<< "  var: " << var->str()<<" at block: "<< block);
                    vector<pair<Temp*, Label*>>* phiArgs = new vector<pair<Temp*, Label*>>();
                    for (auto pred : domInfo->predecessors[dfBlock]) {
                        QuadBlock* predBlock = domInfo->labelToBlock[pred];
                        if (!predBlock || !predBlock->exit_labels || predBlock->exit_labels->empty()) continue;
                        phiArgs->push_back(make_pair(var, predBlock->entry_label));
                    }
                    if(phiArgs->size() <= 1) {
                        delete phiArgs;
                        continue; // No need to insert phi function
                    }
                    
                    // 创建 phi 函数的 def/use 集合
                    set<Temp*>* phiDef = new set<Temp*>{var};
                    set<Temp*>* phiUse = new set<Temp*>();
                    for (auto& arg : *phiArgs) {
                        phiUse->insert(arg.first);
                    }
                    
                    // 创建并插入 phi 函数
                    QuadPhi* phiStm = new QuadPhi(nullptr, new TempExp(Type::INT, var), phiArgs, phiDef, phiUse);
                    insertBlock->quadlist->insert(insertBlock->quadlist->begin() + 1, phiStm);
                    
                    // 如果这是变量的新定值点，加入工作表
                    if (varDefBlocks.find(dfBlock) == varDefBlocks.end()) {
                        varDefBlocks.insert(dfBlock);
                        worklist.insert(dfBlock);
                    }
                }
            }
        }
    }
}
static void renameVariables(QuadFuncDecl* func, ControlFlowInfo* domInfo) {
    DEBUG_PRINT("\n" << "Renaming variables in function: " << func->funcname << "\n");
    map<int, int> counters;  // Count[a]
    map<int, stack<int>> stacks;     // Stack[a]
     
    for (auto block : *func->quadblocklist) {
        for (auto stmt : *block->quadlist) {
            if (stmt->def) {
                for (auto temp : *stmt->def) {
                    int origNum = temp->num > 999 ? VersionedTemp::origTempNum(temp->num) : temp->num;
                    if (counters.find(origNum) == counters.end()) {
                        counters[origNum] = -1;
                        stacks[origNum] = stack<int>();
                    }
                }
            }
            if (stmt->use) {
                for (auto temp : *stmt->use) {
                    int origNum = temp->num > 999 ? VersionedTemp::origTempNum(temp->num) : temp->num;
                    if (counters.find(origNum) == counters.end()) {
                        counters[origNum] = -1;
                        stacks[origNum] = stack<int>();
                    }
                }
            }
        }
    }

    function<void(int)> rename = [&](int blockNum) {
        QuadBlock* block = domInfo->labelToBlock[blockNum];
        if (!block || !block->quadlist) return;
        
        // map<int, int> originalSize;

        int index = 0;
        for (auto stmt : *block->quadlist) {
            DEBUG_PRINT("Processing statement: " << index++ << " in block: " << block->entry_label->num);
            if (stmt->kind != QuadKind::PHI) {
                if (stmt->use) {
                    set<Temp*>* newUse = new set<Temp*>();
                    for (auto temp : *stmt->use) {
                        DEBUG_PRINT("Using temp: " << temp->num);
                        int origNum = temp->num > 999 ? VersionedTemp::origTempNum(temp->num) : temp->num;
                        if (!stacks[origNum].empty()) {
                            newUse->insert(new Temp(stacks[origNum].top()));
                        }
                        else {
                            newUse->insert(temp);
                        }
                    }
                    delete stmt->use;
                    stmt->use = newUse;
                }
                renameQuadStmAfterUse(stmt, stacks);
            }
            if (stmt->def) {
                set<Temp*>* newDef = new set<Temp*>();
                for (auto temp : *stmt->def) {
                    DEBUG_PRINT("Defining temp: " << temp->num);
                    int origNum = temp->num > 999 ? VersionedTemp::origTempNum(temp->num) : temp->num;
                    
                    int newVersion = counters[origNum] + 1;
                    counters[origNum] = newVersion;
                    int newNum = VersionedTemp::versionedTempNum(origNum, newVersion);
                    stacks[origNum].push(newNum);
                    DEBUG_PRINT("At block: "<< block->entry_label->num <<"  Renaming temp " << origNum << " to " << newNum);
                    Temp* newTemp = new Temp(newNum);
                    newDef->insert(newTemp);
                }
                delete stmt->def;
                stmt->def = newDef;
            }
            renameQuadStmAfterDef(stmt, stacks);
        }

        for (int succNum : domInfo->successors[blockNum]) {
            QuadBlock* succ = domInfo->labelToBlock[succNum];
            if (!succ || !succ->quadlist) continue;

            for (auto stmt : *succ->quadlist) {
                if (stmt->kind != QuadKind::PHI) continue;
                auto phi = static_cast<QuadPhi*>(stmt);
                
                for (auto& arg : *phi->args) {
                    if (arg.second->num == block->entry_label->num) {
                        int origNum = arg.first->num;
                        if (!stacks[origNum].empty()) {
                            arg.first = new Temp(stacks[origNum].top());
                            DEBUG_PRINT("At block: "<< block->entry_label->num <<"  Renaming phi arg " << origNum << " to " << stacks[origNum].top());
                        }
                    }
                }
                set<Temp*>* newUse = new set<Temp*>();
                for (auto& arg : *phi->args) {
                    newUse->insert(arg.first);
                }
                delete phi->use;
                phi->use = newUse;
            }
        }
        
        // 递归处理支配树中的子节点
        for (auto child : domInfo->domTree[blockNum]) {
            DEBUG_PRINT("Father block: "<< blockNum  << " Renaming child block: " << child);
            rename(child);
        }
        
        // 恢复栈到原始大小
        for (auto stmt : *block->quadlist) {
            if (stmt->def) {
                for (auto temp : *stmt->def) {
                    int origNum = temp->num > 999 ? VersionedTemp::origTempNum(temp->num) : temp->num;
                    if (!stacks[origNum].empty()) {
                        stacks[origNum].pop();
                    }
                }
            }
        }
        // for (auto& pair : stacks) {
        //     while (pair.second.size() > originalSize[pair.first]) {
        //         pair.second.pop();
        //     }
        // }
    };
    
    // 从入口基本块开始重命名
    if (!func->quadblocklist->empty()) {
        rename(func->quadblocklist->front()->entry_label->num);
    }
    DEBUG_PRINT("Completed for function: " << func->funcname);
}
static void cleanupUnusedPhi(QuadFuncDecl* func) {
    if (!func || !func->quadblocklist) return;

    // 标记所有被使用的变量
    set<int> usedVars;
    
    // 第一遍：收集所有非 phi 语句中使用的变量
    for (auto block : *func->quadblocklist) {
        if (!block->quadlist) continue;
        
        for (auto stmt : *block->quadlist) {
            if (!stmt || stmt->kind == QuadKind::PHI) continue;
            
            if (stmt->use) {
                for (auto temp : *stmt->use) {
                    usedVars.insert(temp->num);
                }
            }
        }
    }

    bool changed;
    do {
        changed = false;
        
        // 遍历所有 phi 函数
        for (auto block : *func->quadblocklist) {
            if (!block->quadlist) continue;
            
            for (auto it = block->quadlist->begin(); it != block->quadlist->end();) {
                auto stmt = *it;
                if (!stmt || stmt->kind != QuadKind::PHI) {
                    ++it;
                    continue;
                }
                
                auto phi = static_cast<QuadPhi*>(stmt);
                bool isUsed = false;
                
                // 检查 phi 的结果是否被使用
                if (phi->temp && usedVars.find(phi->temp->temp->num) != usedVars.end()) {
                    isUsed = true;
                    // 将 phi 的参数也标记为使用
                    for (auto& arg : *phi->args) {
                        if (usedVars.find(arg.first->num) == usedVars.end()) {
                            usedVars.insert(arg.first->num);
                            changed = true;
                        }
                    }
                    ++it;
                } else {
                    it = block->quadlist->erase(it);
                }
            }
        }
    } while (changed);
}

static void renameQuadTerm(QuadTerm* term, const map<int, stack<int>>& stacks) {
    if (!term || term->kind != QuadTermKind::TEMP) return;
    
    TempExp* tempExp = term->get_temp();
    if (!tempExp || !tempExp->temp) return;
    
    int origNum = tempExp->temp->num > 999 ? VersionedTemp::origTempNum(tempExp->temp->num) : tempExp->temp->num;
    auto it = stacks.find(origNum);
    if (it != stacks.end() && !it->second.empty()) {
        tempExp->temp = new Temp(it->second.top());
    }
}

static void renameQuadStmAfterUse(QuadStm* stmt, const map<int, stack<int>>& stacks) {
    if (!stmt) return;
    
    switch (stmt->kind) {
        case QuadKind::MOVE: {
            auto move = static_cast<QuadMove*>(stmt);
            renameQuadTerm(move->src, stacks);
            break;
        }
        case QuadKind::LOAD: {
            auto load = static_cast<QuadLoad*>(stmt);
            renameQuadTerm(load->src, stacks);
            break;
        }
        case QuadKind::STORE: {
            auto store = static_cast<QuadStore*>(stmt);
            renameQuadTerm(store->src, stacks);
            break;
        }
        case QuadKind::MOVE_BINOP: {
            auto binop = static_cast<QuadMoveBinop*>(stmt);
            renameQuadTerm(binop->left, stacks);
            renameQuadTerm(binop->right, stacks);
            break;
        }
        case QuadKind::CALL: {
            auto call = static_cast<QuadCall*>(stmt);
            if (call->obj_term) renameQuadTerm(call->obj_term, stacks);
            if (call->args) {
                for (auto arg : *call->args) {
                    renameQuadTerm(arg, stacks);
                }
            }
            break;
        }
        case QuadKind::MOVE_CALL: {
            auto moveCall = static_cast<QuadMoveCall*>(stmt);
            if (moveCall->call) renameQuadStmAfterUse(moveCall->call, stacks);
            break;
        }
        case QuadKind::EXTCALL: {
            auto extCall = static_cast<QuadExtCall*>(stmt);
            if (extCall->args) {
                for (auto arg : *extCall->args) {
                    renameQuadTerm(arg, stacks);
                }
            }
            break;
        }
        case QuadKind::MOVE_EXTCALL: {
            auto moveExtCall = static_cast<QuadMoveExtCall*>(stmt);
            if (moveExtCall->extcall) renameQuadStmAfterUse(moveExtCall->extcall, stacks);
            break;
        }
        case QuadKind::CJUMP: {
            auto cjump = static_cast<QuadCJump*>(stmt);
            renameQuadTerm(cjump->left, stacks);
            renameQuadTerm(cjump->right, stacks);
            break;
        }
        case QuadKind::RETURN: {
            auto ret = static_cast<QuadReturn*>(stmt);
            renameQuadTerm(ret->value, stacks);
            break;
        }
        case QuadKind::PHI: {
            break;
        }
        default:
            DEBUG_PRINT("Unknown statement kind: " << static_cast<int>(stmt->kind));
            break;
    }
}

static void renameQuadStmAfterDef(QuadStm* stmt, const map<int, stack<int>>& stacks) {
    if (!stmt) return;
    
    switch (stmt->kind) {
        case QuadKind::MOVE: {
            auto move = static_cast<QuadMove*>(stmt);
            auto it = stacks.find(move->dst->temp->num);
            if (it != stacks.end() && !it->second.empty()) {
                move->dst->temp = new Temp(it->second.top());
            }
            break;
        }
        case QuadKind::LOAD: {
            auto load = static_cast<QuadLoad*>(stmt);
            auto it = stacks.find(load->dst->temp->num);
            if (it != stacks.end() && !it->second.empty()) {
                load->dst->temp = new Temp(it->second.top());
            }
            break;
        }
        case QuadKind::STORE: {
            auto store = static_cast<QuadStore*>(stmt);
            renameQuadTerm(store->dst, stacks);
            break;
        }
        case QuadKind::MOVE_BINOP: {
            auto binop = static_cast<QuadMoveBinop*>(stmt);
            auto it = stacks.find(binop->dst->temp->num);
            if (it != stacks.end() && !it->second.empty()) {
                binop->dst->temp = new Temp(it->second.top());
            }
            break;
        }
        case QuadKind::CALL: {
            break;
        }
        case QuadKind::MOVE_CALL: {
            auto moveCall = static_cast<QuadMoveCall*>(stmt);
            auto it = stacks.find(moveCall->dst->temp->num);
            if (it != stacks.end() && !it->second.empty()) {
                moveCall->dst->temp = new Temp(it->second.top());
            }
            break;
        }
        case QuadKind::EXTCALL: {
            break;
        }
        case QuadKind::MOVE_EXTCALL: {
            auto moveExtCall = static_cast<QuadMoveExtCall*>(stmt);
            auto it = stacks.find(moveExtCall->dst->temp->num);
            if (it != stacks.end() && !it->second.empty()) {
                DEBUG_PRINT(" MOVE_EXTCALL: " << moveExtCall->dst->temp->num << " to " << it->second.top());
                moveExtCall->dst->temp = new Temp(it->second.top());
            }
            break;
        }
        case QuadKind::CJUMP: {
            break;
        }
        case QuadKind::RETURN: {
            break;
        }
        case QuadKind::PHI: {
            auto phi = static_cast<QuadPhi*>(stmt);
            auto it = stacks.find(phi->temp->temp->num);
            if (it != stacks.end() && !it->second.empty()) {
                phi->temp->temp = new Temp(it->second.top());
            }
            break;
        }
        default:
            DEBUG_PRINT("Unknown statement kind: " << static_cast<int>(stmt->kind));
            break;
    }
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
        
        for(auto block : *func->quadblocklist) {
            if (block->entry_label) {
                DEBUG_PRINT("Block: " << block->entry_label->num);
                for(auto dominantFrontier : domInfo->domTree[block->entry_label->num]) {
                    DEBUG_PRINT("  Dominate: " << dominantFrontier);
                }
            }
        }
        // Eliminate unreachable blocks
        deleteUnreachableBlocks(func, domInfo);
        
        // Place phi functions
        placePhi(func, domInfo);
        
        // Rename variables
        renameVariables(func, domInfo);
        
        // Cleanup unused phi functions
        // cleanupUnusedPhi(func);
        
        // Add the SSA version of the function to the new program
        ssaProgram->quadFuncDeclList->push_back(func);
    }
    return ssaProgram; //uncomment this line
    //return program; //delete this line
}
