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
                DEBUG_PRINT("Adding block " << blockNum << " to defBlocks for temp " << temp->num);
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
                    
                    // 创建 phi 参数列表
                    DEBUG_PRINT("InsertBlock: " << dfBlock<< "  var: " << var->str()<<" at block: "<< block);
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
    // // 为每个变量维护一个计数器栈
    // map<Temp*, vector<int>> counters;
    // map<Temp*, stack<int>> stacks;
    
    // // 初始化计数器和栈
    // for (auto block : *func->quadblocklist) {
    //     for (auto stmt : *block->quadlist) {
    //         if (stmt->def) {
    //             for (auto temp : *stmt->def) {
    //                 if (counters.find(temp) == counters.end()) {
    //                     counters[temp] = vector<int>{0};
    //                     stacks[temp] = stack<int>();
    //                     stacks[temp].push(0);
    //                 }
    //             }
    //         }
    //         if (stmt->use) {
    //             for (auto temp : *stmt->use) {
    //                 if (counters.find(temp) == counters.end()) {
    //                     counters[temp] = vector<int>{0};
    //                     stacks[temp] = stack<int>();
    //                     stacks[temp].push(0);
    //                 }
    //             }
    //         }
    //     }
    // }
    
    // // 递归重命名函数
    // function<void(int)> rename = [&](int blockNum) {
    //     QuadBlock* block = domInfo->labelToBlock[blockNum];
    //     if (!block || !block->quadlist) return;
        
    //     // 保存原始栈大小
    //     map<Temp*, int> originalSize;
    //     for (auto& pair : stacks) {
    //         originalSize[pair.first] = pair.second.size();
    //     }
        
    //     // 重命名基本块中的变量
    //     for (auto stmt : *block->quadlist) {
    //         // 重命名使用
    //         if (stmt->use) {
    //             for (auto temp : *stmt->use) {
    //                 if (!stacks[temp].empty()) {
    //                     temp->num = stacks[temp].top();
    //                 }
    //             }
    //         }
            
    //         // 处理 phi 函数的特殊情况
    //         if (stmt->kind == QuadKind::PHI) {
    //             auto phi = static_cast<QuadPhi*>(stmt);
    //             for (auto& arg : *phi->args) {
    //                 if (!stacks[arg.first].empty()) {
    //                     arg.first->num = stacks[arg.first].top();
    //                 }
    //             }
    //         }
            
    //         // 重命名定义
    //         if (stmt->def) {
    //             for (auto temp : *stmt->def) {
    //                 int newVersion = counters[temp].back() + 1;
    //                 counters[temp].push_back(newVersion);
    //                 stacks[temp].push(newVersion);
    //                 temp->num = newVersion;
    //             }
    //         }
    //     }
        
    //     // 递归处理支配树中的子节点
    //     for (auto child : domInfo->domTree[blockNum]) {
    //         rename(child);
    //     }
        
    //     // 恢复栈到原始大小
    //     for (auto& pair : stacks) {
    //         while (pair.second.size() > originalSize[pair.first]) {
    //             pair.second.pop();
    //         }
    //     }
    // };
    
    // // 从入口基本块开始重命名
    // if (!func->quadblocklist->empty()) {
    //     rename(func->quadblocklist->front()->entry_label->num);
    // }
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

QuadProgram *quad2ssa(QuadProgram* program) {
    // Create a new QuadProgram to hold the SSA version
    QuadProgram* ssaProgram = new QuadProgram(static_cast<tree::Program*>(program->node), new vector<QuadFuncDecl*>());
    // Iterate through each function in the original program
    for (auto func : *program->quadFuncDeclList) {
        // Create a new ControlFlowInfo object for the function
        ControlFlowInfo* domInfo = new ControlFlowInfo(func);
        // Compute control flow information
        domInfo->computeEverything();
        
        // for(auto block : *func->quadblocklist) {
        //     if (block->entry_label) {
        //         DEBUG_PRINT("Block: " << block->entry_label->num);
        //         for(auto dominantFrontier : domInfo->dominanceFrontiers[block->entry_label->num]) {
        //             DEBUG_PRINT("  Dominance Frontier: " << dominantFrontier);
        //         }
        //     }
        // }
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
