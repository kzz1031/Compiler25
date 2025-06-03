#define DEBUG
//#undef DEBUG

#include <string>
#include <stack>
#include <variant>
#include <vector>
#include <map>
#include <set>
#include "quad.hh"
#include "opt.hh"

#ifdef DEBUG
#define DEBUG_PRINT(x) std::cout << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

void Opt::calculateBT() {
    // 初始化：所有变量设为⊤（MANY_VALUES）
    for (auto& block : *func->quadblocklist) {
        for (auto& quad : *block->quadlist) {
            if (quad->def) {
                for (auto& temp : *quad->def) {
                    temp_value[temp->num] = RtValue(ValueType::MANY_VALUES);
                }
            }
        }
    }

    // 初始化：第一个块可执行
    if (!func->quadblocklist->empty()) {
        block_executable[func->quadblocklist->front()->entry_label->num] = true;
    }

    bool changed;
    do {
        changed = false;
        
        // 遍历所有基本块
        for (auto& block : *func->quadblocklist) {
            if (!block_executable[block->entry_label->num]) {
                continue;
            }

            DEBUG_PRINT("Processing block: " + block->entry_label->str());
            
            // 对于只有一个后继的块，标记后继为可执行
            if (block->exit_labels && block->exit_labels->size() == 1) {
                int next_label = block->exit_labels->front()->num;
                if (!block_executable[next_label]) {
                    block_executable[next_label] = true;
                    changed = true;
                }
            }

            for (auto& quad : *block->quadlist) {
                DEBUG_PRINT("Processing quad");
                
                // 处理赋值指令
                if (quad->kind == QuadKind::MOVE) {
                    auto move = static_cast<QuadMove*>(quad);
                    auto dest = move->dst->temp->num;
                    int src_val;
                    bool src_is_const = move->src->kind == QuadTermKind::CONST;
                    
                    if (src_is_const) {
                        src_val = move->src->get_const();
                    } else {
                        src_val = move->src->get_temp()->temp->num;
                    }
                    
                    DEBUG_PRINT("Dest: " + std::to_string(dest) + 
                                ", Src: " + std::to_string(src_val));
                    
                    RtValue srcValue = src_is_const ? RtValue(src_val) : getRtValue(src_val);
                    if (srcValue.getType() != temp_value[dest].getType() || 
                        (srcValue.getType() == ValueType::ONE_VALUE && 
                         srcValue.getIntValue() != temp_value[dest].getIntValue())) {
                        temp_value[dest] = srcValue;
                        changed = true;
                    }
                }
                // 处理二元运算指令
                else if (quad->kind == QuadKind::MOVE_BINOP) {
                    auto binop = static_cast<QuadMoveBinop*>(quad);
                    auto dest = binop->dst->temp->num;
                    int src1_val, src2_val;
                    bool src1_is_const = binop->left->kind == QuadTermKind::CONST;
                    bool src2_is_const = binop->right->kind == QuadTermKind::CONST;
                    
                    if (src1_is_const) {
                        src1_val = binop->left->get_const();
                    } else {
                        src1_val = binop->left->get_temp()->temp->num;
                    }
                    if (src2_is_const) {
                        src2_val = binop->right->get_const();
                    } else {
                        src2_val = binop->right->get_temp()->temp->num;
                    }
                    
                    DEBUG_PRINT("Dest: " + std::to_string(dest) + 
                                ", Src1: " + std::to_string(src1_val) + 
                                ", Src2: " + std::to_string(src2_val));
                    
                    RtValue val1 = src1_is_const ? RtValue(src1_val) : getRtValue(src1_val);
                    RtValue val2 = src2_is_const ? RtValue(src2_val) : getRtValue(src2_val);
                    
                    if (val1.getType() == ValueType::ONE_VALUE && 
                        val2.getType() == ValueType::ONE_VALUE) {
                        int result;
                        if (binop->binop == "+") result = val1.getIntValue() + val2.getIntValue();
                        else if (binop->binop == "-") result = val1.getIntValue() - val2.getIntValue();
                        else if (binop->binop == "*") result = val1.getIntValue() * val2.getIntValue();
                        else if (binop->binop == "/") result = val1.getIntValue() / val2.getIntValue();
                        else continue;

                        if (temp_value[dest].getType() != ValueType::ONE_VALUE || 
                            temp_value[dest].getIntValue() != result) {
                            temp_value[dest] = RtValue(result);
                            changed = true;
                        }
                    } else if (temp_value[dest].getType() != ValueType::MANY_VALUES) {
                        temp_value[dest] = RtValue(ValueType::MANY_VALUES);
                        changed = true;
                    }
                }
                // 处理PHI指令
                else if (quad->kind == QuadKind::PHI) {
                    auto phi = static_cast<QuadPhi*>(quad);
                    auto dest = phi->temp->temp->num;
                    bool all_same = true;
                    RtValue first_value;
                    bool has_first = false;
                    
                    for (auto& arg : *phi->args) {
                        auto temp = arg.first;
                        auto label = arg.second;
                        if (block_executable[label->num]) {
                            RtValue val = getRtValue(temp->num);
                            if (!has_first) {
                                first_value = val;
                                has_first = true;
                            } else if (val.getType() != first_value.getType() || 
                                     (val.getType() == ValueType::ONE_VALUE && 
                                      val.getIntValue() != first_value.getIntValue())) {
                                all_same = false;
                                break;
                            }
                        }
                    }
                    
                    if (all_same && has_first) {
                        if (temp_value[dest].getType() != first_value.getType() || 
                            (first_value.getType() == ValueType::ONE_VALUE && 
                             temp_value[dest].getIntValue() != first_value.getIntValue())) {
                            temp_value[dest] = first_value;
                            changed = true;
                        }
                    } else if (temp_value[dest].getType() != ValueType::MANY_VALUES) {
                        temp_value[dest] = RtValue(ValueType::MANY_VALUES);
                        changed = true;
                    }
                }
                // 处理内存访问和函数调用
                else if (quad->kind == QuadKind::LOAD || quad->kind == QuadKind::STORE || 
                         quad->kind == QuadKind::CALL || quad->kind == QuadKind::EXTCALL) {
                    if (quad->def) {
                        for (auto& temp : *quad->def) {
                            if (temp_value[temp->num].getType() != ValueType::MANY_VALUES) {
                                temp_value[temp->num] = RtValue(ValueType::MANY_VALUES);
                                changed = true;
                            }
                        }
                    }
                }
                // 处理条件跳转指令
                else if (quad->kind == QuadKind::JUMP || quad->kind == QuadKind::CJUMP) {
                    if (quad->kind == QuadKind::JUMP) {
                        auto jump = static_cast<QuadJump*>(quad);
                        int target = jump->label->num;
                        if (!block_executable[target]) {
                            block_executable[target] = true;
                            changed = true;
                        }
                    } else {
                        auto cjump = static_cast<QuadCJump*>(quad);
                        int src1_val, src2_val;
                        bool src1_is_const = cjump->left->kind == QuadTermKind::CONST;
                        bool src2_is_const = cjump->right->kind == QuadTermKind::CONST;
                        
                        if (src1_is_const) {
                            src1_val = cjump->left->get_const();
                        } else {
                            src1_val = cjump->left->get_temp()->temp->num;
                        }
                        if (src2_is_const) {
                            src2_val = cjump->right->get_const();
                        } else {
                            src2_val = cjump->right->get_temp()->temp->num;
                        }
                        
                        DEBUG_PRINT("Src1: " + std::to_string(src1_val) + 
                                    ", Src2: " + std::to_string(src2_val));
                        
                        RtValue val1 = src1_is_const ? RtValue(src1_val) : getRtValue(src1_val);
                        RtValue val2 = src2_is_const ? RtValue(src2_val) : getRtValue(src2_val);
                        
                        if (val1.getType() == ValueType::ONE_VALUE && 
                            val2.getType() == ValueType::ONE_VALUE) {
                            bool condition = false;
                            if (cjump->relop == "<=") condition = val1.getIntValue() <= val2.getIntValue();
                            else if (cjump->relop == "<") condition = val1.getIntValue() < val2.getIntValue();
                            else if (cjump->relop == ">=") condition = val1.getIntValue() >= val2.getIntValue();
                            else if (cjump->relop == ">") condition = val1.getIntValue() > val2.getIntValue();
                            else if (cjump->relop == "==") condition = val1.getIntValue() == val2.getIntValue();
                            else if (cjump->relop == "!=") condition = val1.getIntValue() != val2.getIntValue();
                            
                            int target = condition ? cjump->t->num : cjump->f->num;
                            if (!block_executable[target]) {
                                block_executable[target] = true;
                                changed = true;
                            }
                        } else {
                            // 如果条件不确定，两个分支都可能执行
                            int target1 = cjump->t->num;
                            int target2 = cjump->f->num;
                            if (!block_executable[target1]) {
                                block_executable[target1] = true;
                                changed = true;
                            }
                            if (!block_executable[target2]) {
                                block_executable[target2] = true;
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    } while (changed);
}


void Opt::modifyFunc() {
    DEBUG_PRINT("Modifying function");
    vector<QuadBlock*>* new_blocks = new vector<QuadBlock*>();
    
    // 遍历所有基本块
    for (auto& block : *func->quadblocklist) {
        DEBUG_PRINT("Processing block: " + block->entry_label->str());
        if (!block_executable[block->entry_label->num]) {
            continue;
        }
        
        vector<QuadStm*>* new_quads = new vector<QuadStm*>();
        
        for (auto& quad : *block->quadlist) {
            if (quad->kind == QuadKind::MOVE) {
                auto move = static_cast<QuadMove*>(quad);
                auto dest = move->dst->temp->num;
                auto src = move->src->get_temp()->temp->num;
                
                RtValue srcValue = getRtValue(src);
                if (srcValue.getType() == ValueType::ONE_VALUE) {
                    set<Temp*>* def = new set<Temp*>();
                    def->insert(move->dst->temp);
                    set<Temp*>* use = new set<Temp*>();
                    QuadMove* new_quad = new QuadMove(move->node, 
                        move->dst, 
                        new QuadTerm(srcValue.getIntValue()),
                        def, use);
                    new_quads->push_back(new_quad);
                } else {
                    new_quads->push_back(quad);
                }
            }
            else if (quad->kind == QuadKind::MOVE_BINOP) {
                auto binop = static_cast<QuadMoveBinop*>(quad);
                auto dest = binop->dst->temp->num;
                auto src1 = binop->left->get_temp()->temp->num;
                auto src2 = binop->right->get_temp()->temp->num;
                
                RtValue val1 = getRtValue(src1);
                RtValue val2 = getRtValue(src2);
                
                if (val1.getType() == ValueType::ONE_VALUE && 
                    val2.getType() == ValueType::ONE_VALUE) {
                    // 如果两个操作数都是常量，计算结果并创建新的常量赋值指令
                    int result;
                    if (binop->binop == "+") result = val1.getIntValue() + val2.getIntValue();
                    else if (binop->binop == "-") result = val1.getIntValue() - val2.getIntValue();
                    else if (binop->binop == "*") result = val1.getIntValue() * val2.getIntValue();
                    else if (binop->binop == "/") result = val1.getIntValue() / val2.getIntValue();
                    else {
                        new_quads->push_back(quad);
                        continue;
                    }

                    set<Temp*>* def = new set<Temp*>();
                    def->insert(binop->dst->temp);
                    set<Temp*>* use = new set<Temp*>();
                    QuadMove* new_quad = new QuadMove(binop->node, 
                        binop->dst, 
                        new QuadTerm(result),
                        def, use);
                    new_quads->push_back(new_quad);
                } else {
                    new_quads->push_back(quad);
                }
            }
            // 处理条件跳转指令
            else if (quad->kind == QuadKind::JUMP || quad->kind == QuadKind::CJUMP) {
                if (quad->kind == QuadKind::JUMP) {
                    new_quads->push_back(quad);
                } else {
                    auto cjump = static_cast<QuadCJump*>(quad);
                    auto src1 = cjump->left->get_temp()->temp->num;
                    auto src2 = cjump->right->get_temp()->temp->num;
                    
                    RtValue val1 = getRtValue(src1);
                    RtValue val2 = getRtValue(src2);
                    
                    if (val1.getType() == ValueType::ONE_VALUE && 
                        val2.getType() == ValueType::ONE_VALUE) {
                        bool condition = false;
                        if (cjump->relop == "<=") condition = val1.getIntValue() <= val2.getIntValue();
                        else if (cjump->relop == "<") condition = val1.getIntValue() < val2.getIntValue();
                        else if (cjump->relop == ">=") condition = val1.getIntValue() >= val2.getIntValue();
                        else if (cjump->relop == ">") condition = val1.getIntValue() > val2.getIntValue();
                        else if (cjump->relop == "==") condition = val1.getIntValue() == val2.getIntValue();
                        else if (cjump->relop == "!=") condition = val1.getIntValue() != val2.getIntValue();
                        
                        // 如果条件可以确定，只保留一个分支
                        set<Temp*>* def = new set<Temp*>();
                        set<Temp*>* use = new set<Temp*>();
                        QuadJump* new_quad = new QuadJump(cjump->node, 
                            condition ? cjump->t : cjump->f,
                            def, use);
                        new_quads->push_back(new_quad);
                    } else {
                        new_quads->push_back(quad);
                    }
                }
            }
            // 其他指令保持不变
            else {
                new_quads->push_back(quad);
            }
        }
        
        // 创建新的基本块
        QuadBlock* new_block = new QuadBlock(block->node, new_quads, block->entry_label, block->exit_labels);
        new_blocks->push_back(new_block);
    }
    
    // 更新函数的基本块列表
    func->quadblocklist = new_blocks;
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