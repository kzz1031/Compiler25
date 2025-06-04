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
    
    for (auto& block : *func->quadblocklist) {
        DEBUG_PRINT("Processing block: " + block->entry_label->str());
        if (!block_executable[block->entry_label->num]) {
            continue;
        }
        for(auto& exit_label : *block->exit_labels) {
            if (!block_executable[exit_label->num]) {
                block->exit_labels->erase(std::find(block->exit_labels->begin(), block->exit_labels->end(), exit_label));
            }
        }
        vector<QuadStm*>* new_quads = new vector<QuadStm*>();
        
        for (auto& quad : *block->quadlist) {
            DEBUG_PRINT("Processing quad");
            if (quad->kind == QuadKind::MOVE) {
                DEBUG_PRINT("MOVE!!!");
                auto move = static_cast<QuadMove*>(quad);
                auto dest = move->dst->temp->num;
                if (temp_value[dest].getType() == ValueType::ONE_VALUE) {
                    DEBUG_PRINT("ONE_VALUE in MOVE");
                } else {
                    new_quads->push_back(quad);
                }
            }
            else if (quad->kind == QuadKind::MOVE_BINOP) {
                DEBUG_PRINT("MOVE_BINOP!!!");
                auto binop = static_cast<QuadMoveBinop*>(quad);
                auto dest = binop->dst->temp->num;
                if (temp_value[dest].getType() == ValueType::ONE_VALUE) {
                        
                } else {
                    if(binop->left->get_temp()){
                        if(temp_value[binop->left->get_temp()->temp->num].getType() == ValueType::ONE_VALUE){
                            binop->use->erase(binop->left->get_temp()->temp);
                            binop->left = new QuadTerm(temp_value[binop->left->get_temp()->temp->num].getIntValue());
                        }
                        
                    }
                    if(binop->right->get_temp()){
                        if(temp_value[binop->right->get_temp()->temp->num].getType() == ValueType::ONE_VALUE){
                            binop->use->erase(binop->right->get_temp()->temp);
                            binop->right = new QuadTerm(temp_value[binop->right->get_temp()->temp->num].getIntValue());
                        }
                    }
                    new_quads->push_back(quad);
                }
                //DEBUG_PRINT("Finish MOVE_BINOP!!!");
            }
            else if (quad->kind == QuadKind::PHI) {
                auto phi = static_cast<QuadPhi*>(quad);
                auto dest = phi->temp->temp->num;
                RtValue destValue = getRtValue(dest);
                
                if (destValue.getType() == ValueType::ONE_VALUE) {

                } else {
                    for (auto& arg : *phi->args) {
                        auto temp = arg.first;
                        auto label = arg.second;
                        if (block_executable[label->num]) {
                            if(temp_value[temp->num].getType() == ValueType::ONE_VALUE) {
                                DEBUG_PRINT("ONE_VALUE in PHI");
                                QuadBlock* target_block = label2block[label->num];
                                Temp* new_temp = new Temp(++func->last_temp_num);
                                set<Temp*>* def = new set<Temp*>();
                                def->insert(new_temp);
                                set<Temp*>* use = new set<Temp*>();
                                
                                TempExp* dst_temp = new TempExp(Type::INT, new_temp);
                                QuadTerm* src_term = new QuadTerm(temp_value[temp->num].getIntValue());
                                QuadMove* move_quad = new QuadMove(phi->node, dst_temp, src_term, def, use);
                                
                                vector<QuadStm*>* pred_quads = target_block->quadlist;
                                if (!pred_quads->empty()) {
                                    // 找到前驱块的最后一条非跳转指令的位置
                                    int insert_pos = pred_quads->size() - 1;
                                    while (insert_pos >= 0 && 
                                           (pred_quads->at(insert_pos)->kind == QuadKind::JUMP || 
                                            pred_quads->at(insert_pos)->kind == QuadKind::CJUMP)) {
                                        insert_pos--;
                                    }

                                    pred_quads->insert(pred_quads->begin() + insert_pos + 1, move_quad);
                                }
                                phi->use->erase(temp);
                                phi->use->insert(new_temp);
                                arg.first = new_temp;
                            }
                        }
                    }
                    new_quads->push_back(quad);
                }
            }
            else if (quad->kind == QuadKind::JUMP || quad->kind == QuadKind::CJUMP) {
                if (quad->kind == QuadKind::JUMP) {
                    new_quads->push_back(quad);
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
                        
                        set<Temp*>* def = new set<Temp*>();
                        set<Temp*>* use = new set<Temp*>();
                        QuadJump* new_quad = new QuadJump(cjump->node, 
                            condition ? cjump->t : cjump->f,
                            def, use);
                        block->exit_labels->erase(std::find(block->exit_labels->begin(), block->exit_labels->end(), condition ? cjump->t : cjump->f));
                        new_quads->push_back(new_quad);
                    } else {
                        new_quads->push_back(quad);
                    }
                }
            }
            else if(quad->kind == QuadKind::EXTCALL){
                auto extcall = static_cast<QuadExtCall*>(quad);
                for(auto& arg : *extcall->args){
                    if(arg->get_temp() && temp_value[arg->get_temp()->temp->num].getType() == ValueType::ONE_VALUE){
                        extcall->use->erase(arg->get_temp()->temp);
                        arg = new QuadTerm(temp_value[arg->get_temp()->temp->num].getIntValue());
                    }
                }
                new_quads->push_back(quad);
            }
            else if(quad->kind == QuadKind::LOAD){
                auto load = static_cast<QuadLoad*>(quad);
                if(load->src->get_temp() && temp_value[load->src->get_temp()->temp->num].getType() == ValueType::ONE_VALUE){
                    load->use->erase(load->src->get_temp()->temp);
                    load->src = new QuadTerm(temp_value[load->src->get_temp()->temp->num].getIntValue());
                }
                new_quads->push_back(quad);
            }
            else if(quad->kind == QuadKind::STORE){
                auto store = static_cast<QuadStore*>(quad);
                if(store->src->get_temp() && temp_value[store->src->get_temp()->temp->num].getType() == ValueType::ONE_VALUE){
                    store->use->erase(store->src->get_temp()->temp);    
                    store->src = new QuadTerm(temp_value[store->src->get_temp()->temp->num].getIntValue());
                }
                new_quads->push_back(quad);
            }
            else if(quad->kind == QuadKind::RETURN){
                auto ret = static_cast<QuadReturn*>(quad);
                if(ret->value && ret->value->get_temp() && temp_value[ret->value->get_temp()->temp->num].getType() == ValueType::ONE_VALUE){
                    ret->use->erase(ret->value->get_temp()->temp);
                    ret->value = new QuadTerm(temp_value[ret->value->get_temp()->temp->num].getIntValue());
                }  
                new_quads->push_back(quad);
            }
            else {
                new_quads->push_back(quad);
            }
        }
        
        QuadBlock* new_block = new QuadBlock(block->node, new_quads, block->entry_label, block->exit_labels);
        label2block[block->entry_label->num] = new_block;
        new_blocks->push_back(new_block);
    }
    
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