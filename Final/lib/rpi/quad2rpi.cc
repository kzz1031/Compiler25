#define DEBUG
// #undef DEBUG

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "quad.hh"
#include "flowinfo.hh"
#include "color.hh"
#include "quad2rpi.hh"

#ifdef DEBUG
#define DEBUG_PRINT(x) cout << x << endl;
#else
#define DEBUG_PRINT(x)
#endif

static string current_funcname = "";

//This is to convert the names of the functions to a format that is acceptable by the assembler
string normalizeName(string name) {
    // Normalize the name by replacing special characters with underscores
    if (name == "_^main^_^main") { //speial case for main
        return "main";
    }
    for (char& c : name) {
        if (!isalnum(c)) {
            c = '$';
        }
    }
    return name;
}

bool rpi_isMachineReg(int n) {
    // Check if a node is a machine register
    return (n >= 0 && n <= 15);
}

string term2str(QuadTerm *term, Color *color) {
    string result;
    if (term->kind == QuadTermKind::TEMP) {
        Temp *t = term->get_temp()->temp;
        result = "r" + to_string(color->color_of(t->num));
    } else if (term->kind == QuadTermKind::CONST) {
        result = "#" + to_string(term->get_const());
    } else if (term->kind == QuadTermKind::MAME) {
        result = "@" + term->get_name();
    } else {
        cerr << "Error: Unknown term kind" << endl;
        exit(EXIT_FAILURE);
    }
    return result;
}

//Always use function name to prefix a label
//Note that you should do the same for Jump and CJump labels
string convert(QuadLabel* label, Color *c, int indent) {
#ifdef DEBUG
    //cout << "In convert Label" << endl;
#endif
    string result; 
    result = current_funcname + "$" + label->label->str() + ": \n";
    return result;
}

/*************************************************** */
/*************************************************** */
/*************************************************** */
/*************************************************** */
/*************************************************** */
/*************************************************** */

string convert(QuadFuncDecl* func, DataFlowInfo *dfi, Color *color, int indent) {
    string result; 
    current_funcname = normalizeName(func->funcname); //set the global variable
    
    result += "\n.balign 4\n";
    result += ".global " + normalizeName(func->funcname) + "\n";
    result += ".section .text\n\n";
    result += normalizeName(func->funcname) + ":\n";
    
    result += string(indent, ' ') + "push {r4-r10, fp, lr}\n";
    result += string(indent, ' ') + "add fp, sp, #32\n";
    
    int spill_size = color->spills.size() * 4;  // 每个spill需要4字节
    DEBUG_PRINT("spill_size: " << spill_size);
    if (spill_size > 0) {
        result += string(indent, ' ') + "sub sp, sp, #" + to_string(spill_size) + "\n";
    }
    
    bool skip = false;
    if (func->quadblocklist != nullptr && !func->quadblocklist->empty()) {
        QuadBlock* block = func->quadblocklist->at(0);  // 由于trace，现在只有一个block
        if (block->quadlist != nullptr) {
            for (auto it = block->quadlist->begin(); it != block->quadlist->end(); ++it) {
                if(skip) {
                    skip = false;
                    continue;
                }
                QuadStm* stm = *it;
                if (stm == nullptr) continue;
                
                // 处理标签
                if (stm->kind == QuadKind::LABEL) {
                    QuadLabel* label = static_cast<QuadLabel*>(stm);
                    // 检查结果字符串中的最后一条非空指令
                    size_t lastInstrPos = result.find_last_of('\n', result.length() - 2);
                    if (lastInstrPos != string::npos) {
                        string lastInstr = result.substr(lastInstrPos + 1);
                        string expectedJump = string(indent, ' ') + "b " + current_funcname + "$" + 
                                           label->label->str() + "\n";
                        if (lastInstr == expectedJump) {
                            // 删除多余的跳转指令
                            result = result.substr(0, lastInstrPos + 1);
                        }
                    }
                    result += convert(label, color, indent);
                    continue;
                }
                
                // 处理其他指令
                switch (stm->kind) {
                    case QuadKind::MOVE: {
                        QuadMove* move = static_cast<QuadMove*>(stm);
                        string src = term2str(move->src, color);
                        QuadTerm* dst_term = new QuadTerm(move->dst);
                        string dst = term2str(dst_term, color);
                        
                        if (move->src->kind == QuadTermKind::TEMP && 
                            color->spills.find(move->src->get_temp()->temp->num) != color->spills.end()) {
                            result += string(indent, ' ') + "ldr r9, [fp, #-" + 
                                    to_string(color->get_spill_offset(move->src->get_temp()->temp->num)) + "]\n";
                            src = "r9";
                        }
                        
                        if (dst_term->kind == QuadTermKind::TEMP && 
                            color->spills.find(dst_term->get_temp()->temp->num) != color->spills.end()) {
                            result += string(indent, ' ') + "mov r10, " + src + "\n";
                            result += string(indent, ' ') + "str r10" + ", [fp, #-" + 
                                    to_string(color->get_spill_offset(dst_term->get_temp()->temp->num)) + "]\n";
                        } else {
                            //DEBUG_PRINT(" dst: "<< dst<<" src: " << src);
                            if(dst == src) {
                                //DEBUG_PRINT("Skipping redundant move: " << dst);
                                break;
                            }
                            result += string(indent, ' ') + "mov " + dst + ", " + src + "\n";
                        }
                        break;
                    }
                    case QuadKind::MOVE_BINOP: {
                        QuadMoveBinop* binop = static_cast<QuadMoveBinop*>(stm);
                        string src1 = term2str(binop->left, color);
                        string src2 = term2str(binop->right, color);
                        QuadTerm* dst_term = new QuadTerm(binop->dst);
                        string dst = term2str(dst_term, color);
                        
                        // 检查是否能与下一条指令合并
                        auto it = find(block->quadlist->begin(), block->quadlist->end(), stm);
                        if (it != block->quadlist->end() && std::next(it) != block->quadlist->end()) {
                            QuadStm* nextStm = *std::next(it);
                            // 检查是否是 add rx, ry, #0 模式
                            if (binop->binop == "+" && 
                                binop->right->kind == QuadTermKind::CONST) {
                                
                                if (nextStm->kind == QuadKind::LOAD) { //TODO: might have a bug here
                                    printf("LOAD\n");
                                    auto load = static_cast<QuadLoad*>(nextStm);
                                    if (load->src->kind == QuadTermKind::TEMP &&
                                        term2str(load->src, color) == dst) {
                                        if(color->spills.find(binop->left->get_temp()->temp->num) != color->spills.end()) {
                                            result += string(indent, ' ') + "ldr r9, [fp, #-" + 
                                                    to_string(color->get_spill_offset(binop->left->get_temp()->temp->num)) + "]\n";
                                            src1 = "r9";
                                        }
                                        result += string(indent, ' ') + "ldr " + 
                                                term2str(new QuadTerm(load->dst), color) + 
                                                ", [" + src1 + ", #" + to_string(binop->right->get_const()) + "]\n";
                                        skip = true; 
                                        continue;
                                    }
                                }
                                else if (nextStm->kind == QuadKind::STORE) {
                                    printf("STORE\n");
                                    auto store = static_cast<QuadStore*>(nextStm);
                                    if (store->dst->kind == QuadTermKind::TEMP &&
                                        term2str(store->dst, color) == dst) {
                                        if(color->spills.find(binop->left->get_temp()->temp->num) != color->spills.end()) {
                                            result += string(indent, ' ') + "ldr r9, [fp, #-" + 
                                                    to_string(color->get_spill_offset(binop->left->get_temp()->temp->num)) + "]\n";
                                            src1 = "r9";
                                        }
                                        result += string(indent, ' ') + "str " + 
                                                term2str(store->src, color) + 
                                                ", [" + src1 + ", #" + to_string(binop->right->get_const()) + "]\n";
                                        skip = true; 
                                        continue;
                                    }
                                }
                            }
                        }

                        if (binop->left->kind == QuadTermKind::TEMP && 
                            color->spills.find(binop->left->get_temp()->temp->num) != color->spills.end()) {
                            result += string(indent, ' ') + "ldr r9, [fp, #-" + 
                                    to_string(color->get_spill_offset(binop->left->get_temp()->temp->num)) + "]\n";
                            src1 = "r9";
                        }
                        if (binop->right->kind == QuadTermKind::TEMP && 
                            color->spills.find(binop->right->get_temp()->temp->num) != color->spills.end()) {
                            result += string(indent, ' ') + "ldr r10, [fp, #-" + 
                                    to_string(color->get_spill_offset(binop->right->get_temp()->temp->num)) + "]\n";
                            src2 = "r10";
                        }
                        if (dst_term->kind == QuadTermKind::TEMP && 
                            color->spills.find(dst_term->get_temp()->temp->num) != color->spills.end()) {
                                dst = "r10";
                        }
                        // 生成对应的操作指令
                        if (binop->binop == "+") {
                            result += string(indent, ' ') + "add " + dst + ", " + src1 + ", " + src2 + "\n";
                        } else if (binop->binop == "-") {
                            result += string(indent, ' ') + "sub " + dst + ", " + src1 + ", " + src2 + "\n";
                        } else if (binop->binop == "*") {
                            result += string(indent, ' ') + "mul " + dst + ", " + src1 + ", " + src2 + "\n";
                        } else if (binop->binop == "/") {
                            result += string(indent, ' ') + "sdiv " + dst + ", " + src1 + ", " + src2 + "\n";
                        }
                        
                        // 如果目标是spill，需要存回栈
                        if (dst_term->kind == QuadTermKind::TEMP && 
                            color->spills.find(dst_term->get_temp()->temp->num) != color->spills.end()) {
                            result += string(indent, ' ') + "str " + dst + ", [fp, #-" + 
                                    to_string(color->get_spill_offset(dst_term->get_temp()->temp->num)) + "]\n";
                        }
                        break;
                    }
                    case QuadKind::JUMP: {
                        QuadJump* jump = static_cast<QuadJump*>(stm);
                        result += string(indent, ' ') + "b " + current_funcname + "$" + 
                                jump->label->str() + "\n";
                        break;
                    }
                    case QuadKind::CJUMP: {
                        QuadCJump* cjump = static_cast<QuadCJump*>(stm);
                        string src1 = term2str(cjump->left, color);
                        string src2 = term2str(cjump->right, color);
                        
                        // 处理spills
                        if (cjump->left->kind == QuadTermKind::TEMP && 
                            color->spills.find(cjump->left->get_temp()->temp->num) != color->spills.end()) {
                            result += string(indent, ' ') + "ldr r9, [fp, #-" + 
                                    to_string(color->get_spill_offset(cjump->left->get_temp()->temp->num)) + "]\n";
                            src1 = "r9";
                        }
                        if (cjump->right->kind == QuadTermKind::TEMP && 
                            color->spills.find(cjump->right->get_temp()->temp->num) != color->spills.end()) {
                            result += string(indent, ' ') + "ldr r10, [fp, #-" + 
                                    to_string(color->get_spill_offset(cjump->right->get_temp()->temp->num)) + "]\n";
                            src2 = "r10";
                        }
                        
                        result += string(indent, ' ') + "cmp " + src1 + ", " + src2 + "\n";
                        if (cjump->relop == "==") {
                            result += string(indent, ' ') + "beq " + current_funcname + "$" + 
                                    cjump->t->str() + "\n";
                            result += string(indent, ' ') + "b " + current_funcname + "$" + 
                                    cjump->f->str() + "\n";
                        } else if (cjump->relop == "!=") {
                            result += string(indent, ' ') + "bne " + current_funcname + "$" + 
                                    cjump->t->str() + "\n";
                            result += string(indent, ' ') + "b " + current_funcname + "$" + 
                                    cjump->f->str() + "\n";
                        } else if (cjump->relop == "<") {
                            result += string(indent, ' ') + "blt " + current_funcname + "$" + 
                                    cjump->t->str() + "\n";
                            result += string(indent, ' ') + "b " + current_funcname + "$" + 
                                    cjump->f->str() + "\n";
                        } else if (cjump->relop == "<=") {
                            result += string(indent, ' ') + "ble " + current_funcname + "$" + 
                                    cjump->t->str() + "\n";
                            result += string(indent, ' ') + "b " + current_funcname + "$" + 
                                    cjump->f->str() + "\n";
                        } else if (cjump->relop == ">") {
                            result += string(indent, ' ') + "bgt " + current_funcname + "$" + 
                                    cjump->t->str() + "\n";
                            result += string(indent, ' ') + "b " + current_funcname + "$" + 
                                    cjump->f->str() + "\n";
                        } else if (cjump->relop == ">=") {
                            result += string(indent, ' ') + "bge " + current_funcname + "$" + 
                                    cjump->t->str() + "\n";
                            result += string(indent, ' ') + "b " + current_funcname + "$" + 
                                    cjump->f->str() + "\n";
                        }
                        break;
                    }
                    case QuadKind::CALL: {
                        QuadCall* call = static_cast<QuadCall*>(stm);
                        // 保存参数寄存器
                        for (int i = 0; i < call->args->size() && i < 4; i++) {
                            string arg = term2str(call->args->at(i), color);
                            if (arg == "r" + to_string(i)) {
                                continue; 
                            }
                            result += string(indent, ' ') + "mov r" + to_string(i) + ", " + arg + "\n";
                        }
                        string obj_ptr = term2str(call->obj_term, color);
                        result += string(indent, ' ') + "blx " + obj_ptr + "\n";
                        break;
                    }
                    case QuadKind::MOVE_CALL: {
                       //DEBUG_PRINT("In MOVE_CALL"<<" "<<block->entry_label);
                        QuadMoveCall *movecall = static_cast<QuadMoveCall*>(stm);
                        // 处理参数
                        for (int i = 0; i < movecall->call->args->size(); i++) {
                            QuadTerm *arg = (*movecall->call->args)[i];
                            string arg_reg = term2str(arg, color);
                            if(arg_reg == "r" + to_string(i)) {
                                continue;
                            }
                            result += string(indent, ' ') + "mov r" + to_string(i) + ", " + arg_reg + "\n";
                        }
                        

                        string obj_ptr = term2str(movecall->call->obj_term, color);
                        result += string(indent, ' ') + "blx" + " " + obj_ptr + "\n";
                  
                        
                        // 保存返回值
                        QuadTerm *dst_term = new QuadTerm(movecall->dst);
                        if (dst_term->kind == QuadTermKind::TEMP) {
                            TempExp *dst_temp_exp = dst_term->get_temp();
                            if (color->spills.find(dst_temp_exp->temp->num) != color->spills.end()) {
                                result += string(indent, ' ') + "str r0, [fp, #-" + 
                                        to_string(color->get_spill_offset(dst_temp_exp->temp->num)) + "]\n";
                            } else {
                                if(term2str(dst_term, color) == "r0") {
                                    break;
                                }
                                result += string(indent, ' ') + "mov " + 
                                        term2str(dst_term, color) + ", r0\n";
                            }
                        }
                        break;
                    }
                    case QuadKind::RETURN: {
                        QuadReturn *ret = static_cast<QuadReturn*>(stm);
                        string ret_reg = term2str(ret->value, color);
                        if(ret_reg != "r0") {
                            result += string(indent, ' ') + "mov r0, " + ret_reg + "\n";
                        }
                        
                        result += string(indent, ' ') + "sub sp, fp, #32\n";
                        result += string(indent, ' ') + "pop {r4-r10, fp, pc}\n";
                        break;
                    }
                    case QuadKind::EXTCALL: {
                        QuadExtCall* extcall = static_cast<QuadExtCall*>(stm);
                        for (int i = 0; i < extcall->args->size() && i < 4; i++) {
                            string arg = term2str(extcall->args->at(i), color);
                            if(arg == "r" + to_string(i)) {
                                continue; 
                            }
                            result += string(indent, ' ') + "mov r" + to_string(i) + ", " + arg + "\n";
                        }
                        result += string(indent, ' ') + "bl " + extcall->extfun + "\n";
                        break;
                    }
                    case QuadKind::MOVE_EXTCALL: {
                        QuadMoveExtCall* moveextcall = static_cast<QuadMoveExtCall*>(stm);
                        // 处理参数
                        for (int i = 0; i < moveextcall->extcall->args->size() && i < 4; i++) {
                            string arg = term2str(moveextcall->extcall->args->at(i), color);
                            if(arg == "r" + to_string(i)) {
                                continue; 
                            }
                            result += string(indent, ' ') + "mov r" + to_string(i) + ", " + arg + "\n";
                        }
                        result += string(indent, ' ') + "bl " + moveextcall->extcall->extfun + "\n";
                        // 保存返回值
                        QuadTerm* dst_term = new QuadTerm(moveextcall->dst);
                        if (dst_term->kind == QuadTermKind::TEMP) {
                            TempExp* dst_temp_exp = dst_term->get_temp();
                            if (color->spills.find(dst_temp_exp->temp->num) != color->spills.end()) {
                                result += string(indent, ' ') + "str r0, [fp, #-" + 
                                        to_string(color->get_spill_offset(dst_temp_exp->temp->num)) + "]\n";
                            } else {
                                if(term2str(dst_term, color) == "r0") {
                                    break;
                                }
                                result += string(indent, ' ') + "mov " + 
                                        term2str(dst_term, color) + ", r0\n";
                            }
                        }
                        break;
                    }
                    case QuadKind::LOAD: {
                        QuadLoad* load = static_cast<QuadLoad*>(stm);
                        string src = term2str(load->src, color);
                        QuadTerm* dst_term = new QuadTerm(load->dst);
                        string dst = term2str(dst_term, color);
                        
                        // 处理spills
                        if(dst_term->kind == QuadTermKind::TEMP && 
                           color->spills.find(dst_term->get_temp()->temp->num) != color->spills.end()){
                            dst = "r10";
                        }
                        if (load->src->kind == QuadTermKind::TEMP && 
                            color->spills.find(load->src->get_temp()->temp->num) != color->spills.end()) {
                            result += string(indent, ' ') + "ldr "+ "r9" + ", [fp, #-" + 
                                    to_string(color->get_spill_offset(load->src->get_temp()->temp->num)) + "]\n";
                                    src = "r9";
                        }
                        if(load->src->kind == QuadTermKind::MAME) {
                            src = normalizeName(load->src->get_name());
                            result += string(indent, ' ') + "ldr " + dst + ", =" + src + "\n";
                        }
                        else 
                            result += string(indent, ' ') + "ldr " + dst + ", [" + src + "]\n";
                        
                        // 如果目标是spill，需要存回栈
                        if (dst_term->kind == QuadTermKind::TEMP && 
                            color->spills.find(dst_term->get_temp()->temp->num) != color->spills.end()) {
                            result += string(indent, ' ') + "str r10, [fp, #-" + 
                                    to_string(color->get_spill_offset(dst_term->get_temp()->temp->num)) + "]\n";
                        }
                        break;
                    }
                    case QuadKind::STORE: {
                        QuadStore* store = static_cast<QuadStore*>(stm);
                        string src = term2str(store->src, color);
                        string dst = term2str(store->dst, color);
                        
                        // 处理spills
                        if (store->src->kind == QuadTermKind::TEMP && 
                            color->spills.find(store->src->get_temp()->temp->num) != color->spills.end()) {
                            result += string(indent, ' ') + "ldr r9, [fp, #-" + 
                                    to_string(color->get_spill_offset(store->src->get_temp()->temp->num)) + "]\n";
                            src = "r9";
                        }
                        if (store->dst->kind == QuadTermKind::TEMP && 
                            color->spills.find(store->dst->get_temp()->temp->num) != color->spills.end()) {
                            result += string(indent, ' ') + "ldr r10, [fp, #-" + 
                                    to_string(color->get_spill_offset(store->dst->get_temp()->temp->num)) + "]\n";
                            dst = "r10";
                        }
                        
                        result += string(indent, ' ') + "str " + src + ", [" + dst + "]\n";
                        break;
                    }
                    case QuadKind::PHI: {
                        QuadPhi* phi = static_cast<QuadPhi*>(stm);
                        QuadTerm* dst_term = new QuadTerm(phi->temp);
                        string dst = term2str(dst_term, color);
                        
                        // 处理phi节点的每个分支
                        for (auto& pair : *phi->args) {
                            Temp* temp = pair.first;
                            Label* label = pair.second;
                            string src = "r" + to_string(color->color_of(temp->num));
                            
                            // 如果源是spill，需要从栈加载
                            if (color->spills.find(temp->num) != color->spills.end()) {
                                result += string(indent, ' ') + "ldr r9, [fp, #-" + 
                                        to_string(color->get_spill_offset(temp->num)) + "]\n";
                                src = "r9";
                            }
                            
                            result += string(indent, ' ') + "mov " + dst + ", " + src + "\n";
                        }
                        
                        // 如果目标是spill，需要存回栈
                        if (dst_term->kind == QuadTermKind::TEMP && 
                            color->spills.find(dst_term->get_temp()->temp->num) != color->spills.end()) {
                            result += string(indent, ' ') + "str r9, [fp, #-" + 
                                    to_string(color->get_spill_offset(dst_term->get_temp()->temp->num)) + "]\n";
                        }
                        delete dst_term;
                        break;
                    }
                }
            }
        }
    }
    
    return result;
}

string quad2rpi(QuadProgram* quadProgram, ColorMap *cm) {// Convert a QuadProgram to RPI format with k registers
    string result; result.reserve(10000);
    // Iterate through the function declarations in the Quad program
    result = ".section .note.GNU-stack\n\n@ Here is the RPI code\n\n";
    for (QuadFuncDecl* func : *quadProgram->quadFuncDeclList) {
        //get the data flow info for the function
        result += "@ Here's function: " + func->funcname + "\n";
        DataFlowInfo *dfi = new DataFlowInfo(func);
        dfi->computeLiveness(); //liveness useful in some cases. Has to be done before trace otherwise this func code won't work!
        trace(func); //trace it (merge all blocks into one)
        current_funcname = func->funcname; //set the global variable
        //get the color for the function
        Color *c = cm->color_map[func->funcname]; 
        int indent = 9;
        result += convert(func, dfi, c, indent) + "\n";
    }
    //put the global functions at the end
    result += ".global malloc\n";
    result +=".global getint\n";
    result += ".global putint\n";
    result += ".global putch\n";
    result += ".global putarray\n";
    result += ".global getch\n";
    result += ".global getarray\n";
    result += ".global starttime\n";
    result += ".global stoptime\n";
    return result;
}

// Print the RPI code to the output file
void quad2rpi(QuadProgram* quadProgram, ColorMap *cm, string filename) {
    ofstream outfile(filename);
    if (outfile.is_open()) {
        outfile << quad2rpi(quadProgram, cm);
        outfile.flush();
        outfile.close();
    } else {
        cerr << "Error: Unable to open file " << filename << endl;
    }
}