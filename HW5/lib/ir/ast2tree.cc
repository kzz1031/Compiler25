#define DEBUG
//#undef DEBUG

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include "config.hh"
#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "treep.hh"
#include "temp.hh"
#include "ast2tree.hh"

using namespace std;
//using namespace fdmj;
//using namespace tree;
#ifdef DEBUG
#define DEBUG_PRINT(x) cout << x << endl;
#endif

Class_table* generate_class_table(AST_Semant_Map* semant_map) {
    Class_table* ct = new Class_table();
    Name_Maps* nm = semant_map->getNameMaps();
    int var_offset = 0;
    int method_offset = 0;
    
    for (auto& class_name : *(nm->get_class_list())) {
        for (auto& var_name : *(nm->get_class_var_list(class_name))) {
            string full_var_name = class_name + "_" + var_name;
            if (ct->var_pos_map.find(full_var_name) == ct->var_pos_map.end()) {
                ct->var_pos_map[full_var_name] = var_offset++;
            }
        }
        
        for (auto& method_name : *(nm->get_method_list(class_name))) {
            string full_method_name = class_name + "_" + method_name;
            if (ct->method_pos_map.find(full_method_name) == ct->method_pos_map.end()) {
                ct->method_pos_map[full_method_name] = method_offset++;
            }
        }
    }
    
    return ct;
}

Method_var_table* generate_method_var_table(string class_name, string method_name, Name_Maps* nm, Temp_map* tm) {
    Method_var_table* mvt = new Method_var_table();
    // add this pointer
    if (class_name != "_^main^_") {
        DEBUG_PRINT("add this pointer");
        tree::Temp* this_temp = tm->newtemp();
        mvt->var_temp_map->insert({"this", this_temp});
        mvt->var_type_map->insert({"this", tree::Type::PTR});
    }

    // add local variables
    if (nm->is_method(class_name, method_name)) {
        auto var_list = nm->get_method_var_list(class_name, method_name);
        DEBUG_PRINT("local variables size: "<<var_list->size());
        for (auto& var_name : *var_list) {
            tree::Temp* var_temp = tm->newtemp();
            mvt->var_temp_map->insert({var_name, var_temp});
            DEBUG_PRINT("add local variables: "<<var_name);
            auto var = nm->get_method_var(class_name, method_name, var_name);
            if (var->type->typeKind == TypeKind::INT) {
                mvt->var_type_map->insert({var_name, tree::Type::INT});
            } else {
                mvt->var_type_map->insert({var_name, tree::Type::PTR});
            }
        }
    }

    // add formal parameters
    if (nm->is_method(class_name, method_name)) {
        auto formal_list = nm->get_method_formal_list(class_name, method_name);
        for (auto& formal_name : *formal_list) {
            // if(&formal_name == &formal_list->back()) {
            //     continue;
            // }
            tree::Temp* formal_temp = tm->newtemp();
            mvt->var_temp_map->insert({formal_name, formal_temp});
            
            auto formal = nm->get_method_formal(class_name, method_name, formal_name);
            if (formal->type->typeKind == TypeKind::INT) {
                mvt->var_type_map->insert({formal_name, tree::Type::INT});
            } else {
                mvt->var_type_map->insert({formal_name, tree::Type::PTR});
            }
        }
    }
    
    return mvt;
}

void ASTToTreeVisitor::visit(fdmj::Program* node) {
    DEBUG_PRINT("visit fdmj::Program");
    vector<tree::FuncDecl*>* fdl = new vector<tree::FuncDecl*>();
    
    if (node->main != nullptr) {
        node->main->accept(*this);
        if (visit_tree_result != nullptr) {
            fdl->push_back(static_cast<tree::FuncDecl*>(visit_tree_result));
        }
    }
    // no use
    if (node->cdl != nullptr) {
        for (auto classDecl : *(node->cdl)) {
            classDecl->accept(*this);
            if (visit_tree_result != nullptr) {
                tree::FuncDecl* fd = static_cast<tree::FuncDecl*>(visit_tree_result);
                fdl->push_back(fd);
            }
        }
    }
    
    visit_tree_result = new tree::Program(fdl);
}

void ASTToTreeVisitor::visit(fdmj::MainMethod* node) {
    DEBUG_PRINT("visit fdmj::MainMethod");
    
    vector<tree::Stm*>* sl = new vector<tree::Stm*>();
    current_mvt = generate_method_var_table("_^main^_", "main", name_maps, temp_map);

    if (node->vdl != nullptr) {
        for (auto varDecl : *(node->vdl)) {
            DEBUG_PRINT("visit fdmj::MainMethod"<<" varDecl id: "<<varDecl->id->id);
            if(varDecl->init.index() == 0) {
                continue;
            }
            varDecl->accept(*this);
            if(tr_exp == nullptr) {
                continue;
            }
            Tr_nx* tr_nx = dynamic_cast<Tr_nx*>(tr_exp);
            tree::Seq* seq = dynamic_cast<tree::Seq*>(tr_nx->stm);
            if (seq != nullptr && seq->sl != nullptr) {
                for (auto s : *(seq->sl)) {
                    sl->push_back(s);
                }
            } else {
                sl->push_back(tr_nx->stm);
            }
        }
    }
    
    if (node->sl != nullptr) {
        for (auto stm : *(node->sl)) {
            stm->accept(*this);
            if (tr_exp != nullptr) {
                Tr_nx* tr_nx = dynamic_cast<Tr_nx*>(tr_exp);
                if(tr_nx == nullptr) {
                    tr_nx = dynamic_cast<Tr_ex*>(tr_exp)->unNx(temp_map);
                }
                if (tr_nx == nullptr) {
                    tr_nx = dynamic_cast<Tr_cx*>(tr_exp)->unNx(temp_map);
                }
                sl->push_back(tr_nx->stm);
            }
        }
    }
    
    tree::Label* entry_label = temp_map->newlabel();
    tree::LabelStm* label_stm = new tree::LabelStm(entry_label);
    
    sl->insert(sl->begin(), label_stm);
    
    vector<tree::Block*>* bl = new vector<tree::Block*>();
    tree::Block* block = new tree::Block(entry_label, nullptr, sl);
    bl->push_back(block);
    
    visit_tree_result = new tree::FuncDecl(
        "_^main^_^main", 
        nullptr, 
        bl, 
        tree::Type::INT, 
        temp_map->next_temp - 1, 
        temp_map->next_label - 1
    );
}

void ASTToTreeVisitor::visit(fdmj::ClassDecl* node) {
}

void ASTToTreeVisitor::visit(fdmj::Type* node) {
}

void ASTToTreeVisitor::visit(fdmj::VarDecl* node) {
    DEBUG_PRINT("visit fdmj::VarDecl");
    DEBUG_PRINT("var name: "<<node->id->id);
    Temp* var_temp = current_mvt->var_temp_map->at(node->id->id);

    if(node->type->typeKind == TypeKind::ARRAY) {
        DEBUG_PRINT("var type: array");
        int array_size = node->type->arity ? node->type->arity->val : 0;
        DEBUG_PRINT("array size: "<<array_size);
        vector<tree::Stm*>* stms = new vector<tree::Stm*>();
        
        // 分配内存
        tree::ExtCall* malloc_call = new tree::ExtCall(
            tree::Type::PTR,
            "malloc",
            new vector<tree::Exp*>{new tree::Const((array_size + 1) * 4)}
        );
        stms->push_back(new tree::Move(
            new tree::TempExp(tree::Type::PTR, var_temp),
            malloc_call
        ));

        // 存储数组长度
        stms->push_back(new tree::Move(
            new tree::Mem(tree::Type::INT, new tree::TempExp(tree::Type::PTR, var_temp)),
            new tree::Const(array_size)
        ));

        //DEBUG_PRINT("varDecl init: "<<node->init.index());
        if(node->init.index() == 2) { // vector<IntExp*>
            DEBUG_PRINT("varDecl init: vector<IntExp*>");
            vector<IntExp*>* init_array = std::get<vector<IntExp*>*>(node->init);
            for(size_t i = 0; init_array != nullptr && i < init_array->size(); i++) {
                DEBUG_PRINT("init_array["<<i<<"]: "<<(*init_array)[i]->val);
                int val = (*init_array)[i]->val;
                stms->push_back(new tree::Move(
                    new tree::Mem(tree::Type::INT, 
                        new tree::Binop(tree::Type::PTR, "+", 
                            new tree::TempExp(tree::Type::PTR, var_temp),
                            // new tree::Binop(tree::Type::INT, "*", 
                            //     new tree::Const(i + 1),
                            //     new tree::Const(4)))), 
                            new tree::Const((i + 1)*4))),
                    new tree::Const(val)
                ));
            }
        }
        
        tr_exp = new Tr_nx(new tree::Seq(stms));
    }
    // 处理普通整数变量初始化 
    else if(node->type->typeKind == TypeKind::INT) {
        if(std::holds_alternative<IntExp*>(node->init)) {
            auto init_int = std::get<IntExp*>(node->init);
            int value = init_int->val;
            tr_exp = new Tr_nx(new tree::Move(
                new tree::TempExp(tree::Type::INT, var_temp), 
                new tree::Const(value)
            ));
        }
        else if(std::holds_alternative<monostate>(node->init)) {
            tr_exp = nullptr;
        }
    }
}

void ASTToTreeVisitor::visit(fdmj::MethodDecl* node) {
    vector<tree::Stm*>* sl = new vector<tree::Stm*>();
    
    current_mvt = generate_method_var_table("", node->id->id, nullptr, temp_map);
    
    vector<tree::Temp*>* args = new vector<tree::Temp*>();
    if (node->fl != nullptr) {
        for (auto formal : *(node->fl)) {
            formal->accept(*this);
        }
    }
    
    if (node->vdl != nullptr) {
        for (auto varDecl : *(node->vdl)) {
            string var_name = varDecl->id->id;
            tree::Temp* var_temp = current_mvt->var_temp_map->at(var_name);
            
            if (varDecl->init.index() != 0) { // 不是monostate
                if (varDecl->init.index() == 1) { // IntExp*
                    IntExp* init_int = get<IntExp*>(varDecl->init);
                    int value = init_int->val;
                    sl->push_back(new tree::Move(new tree::TempExp(tree::Type::INT, var_temp), new tree::Const(value)));
                }
            }
        }
    }
    
    // 处理语句
    if (node->sl != nullptr) {
        for (auto stm : *(node->sl)) {
            stm->accept(*this);
            if (visit_tree_result != nullptr) {
                tree::Stm* tree_stm = dynamic_cast<tree::Stm*>(visit_tree_result);
                if (tree_stm != nullptr) {
                    sl->push_back(tree_stm);
                }
            }
        }
    }
    
    tree::Label* entry_label = temp_map->newlabel();
    tree::LabelStm* label_stm = new tree::LabelStm(entry_label);
    
    sl->insert(sl->begin(), label_stm);
    
    vector<tree::Block*>* bl = new vector<tree::Block*>();
    tree::Block* block = new tree::Block(entry_label, nullptr, sl);
    bl->push_back(block);
    
    // 创建函数声明
    string method_name = "_" + node->id->id; // 临时方法名
    visit_tree_result = new tree::FuncDecl(method_name, args, bl, tree::Type::INT, temp_map->next_temp, temp_map->next_label);
}

void ASTToTreeVisitor::visit(fdmj::Formal* node) {
}

void ASTToTreeVisitor::visit(fdmj::Nested* node) {
    if (node->sl != nullptr) {
        vector<tree::Stm*>* sl = new vector<tree::Stm*>();
        
        for (auto stm : *(node->sl)) {
            stm->accept(*this);
            if (tr_exp != nullptr) {
                Tr_nx* stm_tr = dynamic_cast<Tr_nx*>(tr_exp);
                if(stm_tr == nullptr) {
                    stm_tr = dynamic_cast<Tr_ex*>(tr_exp)->unNx(temp_map);
                }
                if(stm_tr == nullptr) {
                    stm_tr = dynamic_cast<Tr_cx*>(tr_exp)->unNx(temp_map);
                }
                if (stm_tr != nullptr) {
                    sl->push_back(stm_tr->stm);
                }
            }
        }
        tr_exp = new Tr_nx(new tree::Seq(sl));
    } else {
        tr_exp = new Tr_nx(new tree::Seq());
    }
}

void ASTToTreeVisitor::visit(fdmj::If* node) {
    node->exp->accept(*this);
    Tr_cx* cond_tr = dynamic_cast<Tr_cx*>(tr_exp);
    if(cond_tr == nullptr) {
        cond_tr = dynamic_cast<Tr_ex*>(tr_exp)->unCx(temp_map);
    }
    tree::Label* then_label = temp_map->newlabel();
    tree::Label* else_label = temp_map->newlabel();
    tree::Label* end_label = temp_map->newlabel();
    
    cond_tr->true_list->patch(then_label);
    cond_tr->false_list->patch(else_label);
    
    node->stm1->accept(*this);
    Tr_nx* then_tr = dynamic_cast<Tr_nx*>(tr_exp);
    
    Tr_nx* else_tr = nullptr;
    if (node->stm2 != nullptr) {
        node->stm2->accept(*this);
        else_tr = dynamic_cast<Tr_nx*>(tr_exp);
    }
    
    vector<tree::Stm*>* sl = new vector<tree::Stm*>();
    sl->push_back(cond_tr->stm);
    sl->push_back(new tree::LabelStm(then_label));
    sl->push_back(then_tr->stm);
    sl->push_back(new tree::Jump(end_label));
    sl->push_back(new tree::LabelStm(else_label));
    if (else_tr != nullptr) {
        sl->push_back(else_tr->stm);
    }
    sl->push_back(new tree::LabelStm(end_label));
    
    tr_exp = new Tr_nx(new tree::Seq(sl));
}

void ASTToTreeVisitor::visit(fdmj::While* node) {
    DEBUG_PRINT("visit fdmj::While");

    tree::Label* test_label = temp_map->newlabel();  // 循环测试标签
    tree::Label* body_label = temp_map->newlabel();  // 循环体标签
    tree::Label* end_label = temp_map->newlabel();   // 循环结束标签

    while_labels.push_back({test_label, end_label});
    
    node->exp->accept(*this);
    Tr_cx* cond_tr = dynamic_cast<Tr_cx*>(tr_exp);
    if(cond_tr == nullptr) {
        cond_tr = dynamic_cast<Tr_ex*>(tr_exp)->unCx(temp_map);
    }
    cond_tr->true_list->patch(body_label);
    cond_tr->false_list->patch(end_label);
    
    node->stm->accept(*this);
    Tr_nx* body_tr = dynamic_cast<Tr_nx*>(tr_exp);
    
    vector<tree::Stm*>* sl = new vector<tree::Stm*>();
    sl->push_back(new tree::LabelStm(test_label));
    sl->push_back(cond_tr->stm);
    sl->push_back(new tree::LabelStm(body_label));
    sl->push_back(body_tr->stm);
    sl->push_back(new tree::Jump(test_label));
    sl->push_back(new tree::LabelStm(end_label));
    
    while_labels.pop_back();
    
    tr_exp = new Tr_nx(new tree::Seq(sl));
}

void ASTToTreeVisitor::visit(fdmj::Assign* node) {
    DEBUG_PRINT("visit fdmj::Assign");
    node->left->accept(*this);
    Tr_ex* left_tr = dynamic_cast<Tr_ex*>(tr_exp);
    if(left_tr == nullptr) {
        left_tr = dynamic_cast<Tr_cx*>(tr_exp)->unEx(temp_map);
    }
    node->exp->accept(*this);
    Tr_ex* right_tr = dynamic_cast<Tr_ex*>(tr_exp);
    if(right_tr == nullptr) {
        right_tr = dynamic_cast<Tr_cx*>(tr_exp)->unEx(temp_map);
    }
    
    tr_exp = new Tr_nx(new tree::Move(left_tr->exp, right_tr->exp));
}

void ASTToTreeVisitor::visit(fdmj::CallStm* node) {
    tree::Exp* obj_exp = nullptr;
    if (node->obj != nullptr) {
        node->obj->accept(*this);
        Tr_ex* obj_tr = dynamic_cast<Tr_ex*>(tr_exp);
        obj_exp = obj_tr->exp;
    }
    
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    if (node->par != nullptr) {
        for (auto arg : *(node->par)) {
            arg->accept(*this);
            Tr_ex* arg_tr = dynamic_cast<Tr_ex*>(tr_exp);
            if(arg_tr == nullptr) {
                arg_tr = dynamic_cast<Tr_cx*>(tr_exp)->unEx(temp_map);
            }
            if (arg_tr != nullptr) {
                args->push_back(arg_tr->exp);
            }
        }
    }
    tr_exp = new Tr_nx(new tree::ExpStm(new tree::Call(tree::Type::INT, node->name->id, obj_exp, args)));
}

void ASTToTreeVisitor::visit(fdmj::Continue* node) {
    DEBUG_PRINT("visit fdmj::Continue");
    if (!while_labels.empty()) {
        tree::Label* continue_label = while_labels.back().first;
        tr_exp = new Tr_nx(new tree::Jump(continue_label));
    } else {
        DEBUG_PRINT("===while_labels empty===");
        tr_exp = nullptr;
    }
}

void ASTToTreeVisitor::visit(fdmj::Break* node) {
    DEBUG_PRINT("visit fdmj::Break");
    if (!while_labels.empty()) {
        tree::Label* break_label = while_labels.back().second;
        tr_exp = new Tr_nx(new tree::Jump(break_label));
    } else {
        DEBUG_PRINT("===while_labels empty===");
        tr_exp = nullptr;
    }
}

void ASTToTreeVisitor::visit(fdmj::Return* node) {
    DEBUG_PRINT("visit fdmj::Return");
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        Tr_ex* ret_tr = dynamic_cast<Tr_ex*>(tr_exp);
        tr_exp = new Tr_nx(new tree::Return(ret_tr->exp));
    } else {
        tr_exp = new Tr_nx(new tree::Return(new tree::Const(0)));
    }
}

void ASTToTreeVisitor::visit(fdmj::PutInt* node) {
    node->exp->accept(*this);
    Tr_ex* arg_tr = dynamic_cast<Tr_ex*>(tr_exp);
    if(arg_tr == nullptr) {
        arg_tr = dynamic_cast<Tr_cx*>(tr_exp)->unEx(temp_map);
    }
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    args->push_back(arg_tr->exp);
    
    tr_exp = new Tr_nx(new tree::ExpStm(new tree::ExtCall(tree::Type::INT, "putint", args)));
}

void ASTToTreeVisitor::visit(fdmj::PutCh* node) {
    DEBUG_PRINT("visit fdmj::PutCh");
    node->exp->accept(*this);
    Tr_ex* arg_tr = dynamic_cast<Tr_ex*>(tr_exp);
    if(arg_tr == nullptr) {
        arg_tr = dynamic_cast<Tr_cx*>(tr_exp)->unEx(temp_map);
    }
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    args->push_back(arg_tr->exp);
    
    tr_exp = new Tr_nx(new tree::ExpStm(new tree::ExtCall(tree::Type::INT, "putch", args)));
}

void ASTToTreeVisitor::visit(fdmj::PutArray* node) {
    tr_exp = nullptr;
}

void ASTToTreeVisitor::visit(fdmj::Starttime* node) {
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    tree::ExtCall* call = new tree::ExtCall(tree::Type::INT, "starttime", args);
    tr_exp = new Tr_nx(new tree::ExpStm(call));
}

void ASTToTreeVisitor::visit(fdmj::Stoptime* node) {
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    tree::ExtCall* call = new tree::ExtCall(tree::Type::INT, "stoptime", args);
    tr_exp = new Tr_nx(new tree::ExpStm(call));
}

void ASTToTreeVisitor::visit(fdmj::BinaryOp* node) {
    DEBUG_PRINT("visit fdmj::BinaryOp" << " node op: " << node->op->op);
    string op = node->op->op;
    node->left->accept(*this);
    Tr_ex* left_tr = dynamic_cast<Tr_ex*>(tr_exp);
    node->right->accept(*this);
    Tr_ex* right_tr = dynamic_cast<Tr_ex*>(tr_exp);

    if ((op == "+" || op == "-" || op == "*" || op == "/") && 
        left_tr != nullptr && right_tr != nullptr &&
        left_tr->exp->type == tree::Type::PTR && right_tr->exp->type == tree::Type::PTR) {
        // 创建临时变量
        tree::Temp* left_len_temp = temp_map->newtemp();      // 存储数组长度
        tree::Temp* right_len_temp = temp_map->newtemp();     // 存储数组长度
        tree::Temp* result_array = temp_map->newtemp();  // 存储结果数组
        tree::Temp* index_temp = temp_map->newtemp();    // 循环索引
        tree::Temp* bound_temp = temp_map->newtemp();    // 循环边界
        
        // 创建标签
        tree::Label* error_label = temp_map->newlabel();
        tree::Label* ok_label = temp_map->newlabel();
        tree::Label* loop_label = temp_map->newlabel();
        tree::Label* loop_body = temp_map->newlabel();
        tree::Label* loop_exit = temp_map->newlabel();

        vector<tree::Stm*>* stms = new vector<tree::Stm*>();
        
        // 获取左数组长度
        stms->push_back(new tree::Move(
            new tree::TempExp(tree::Type::INT, left_len_temp),
            new tree::Mem(tree::Type::INT, left_tr->exp)
        ));
        
        stms->push_back(new tree::Move(
            new tree::TempExp(tree::Type::INT, right_len_temp),
            new tree::Mem(tree::Type::INT, right_tr->exp)
        ));
        // 检查两个数组长度是否相等
        stms->push_back(new tree::Cjump(
            "!=",
            new tree::TempExp(tree::Type::INT, left_len_temp),
            new tree::TempExp(tree::Type::INT, right_len_temp),
            error_label,
            ok_label
        ));
        
        // 长度不等则退出
        stms->push_back(new tree::LabelStm(error_label));
        stms->push_back(new tree::ExpStm(
            new tree::ExtCall(tree::Type::INT, "exit", 
                new vector<tree::Exp*>{new tree::Const(-1)})
        ));
        
        // 长度相等，分配结果数组空间
        stms->push_back(new tree::LabelStm(ok_label));
        stms->push_back(new tree::Move(
            new tree::TempExp(tree::Type::PTR, result_array),
            new tree::ExtCall(tree::Type::PTR, "malloc",
                new vector<tree::Exp*>{
                    new tree::Binop(tree::Type::INT, "*",
                        new tree::Binop(tree::Type::INT, "+",
                            new tree::TempExp(tree::Type::INT, left_len_temp),
                            new tree::Const(1)
                        ),
                        new tree::Const(4)
                    )
                }
            )
        ));
        
        // 设置结果数组的长度
        stms->push_back(new tree::Move(
            new tree::Mem(tree::Type::INT, new tree::TempExp(tree::Type::PTR, result_array)),
            new tree::TempExp(tree::Type::INT, left_len_temp)
        ));
        
        // 初始化循环变量
        stms->push_back(new tree::Move(
            new tree::TempExp(tree::Type::INT, index_temp),
            new tree::Const(4)  // 从4开始，跳过长度字段
        ));
        
        // 设置循环边界
        stms->push_back(new tree::Move(
            new tree::TempExp(tree::Type::INT, bound_temp),
            new tree::Binop(tree::Type::INT, "*",
                new tree::Binop(tree::Type::INT, "+",
                    new tree::TempExp(tree::Type::INT, left_len_temp),
                    new tree::Const(1)
                ),
                new tree::Const(4)
            )
        ));
        
        // 开始循环
        stms->push_back(new tree::LabelStm(loop_label));
        stms->push_back(new tree::Cjump(
            "<",
            new tree::TempExp(tree::Type::INT, index_temp),
            new tree::TempExp(tree::Type::INT, bound_temp),
            loop_body,
            loop_exit
        ));
        
        // 循环体
        stms->push_back(new tree::LabelStm(loop_body));
        stms->push_back(new tree::Move(
            new tree::Mem(tree::Type::INT,
                new tree::Binop(tree::Type::PTR, "+",
                    new tree::TempExp(tree::Type::PTR, result_array),
                    new tree::TempExp(tree::Type::INT, index_temp)
                )
            ),
            new tree::Binop(tree::Type::INT, op,
                new tree::Mem(tree::Type::INT,
                    new tree::Binop(tree::Type::PTR, "+",
                        left_tr->exp,
                        new tree::TempExp(tree::Type::INT, index_temp)
                    )
                ),
                new tree::Mem(tree::Type::INT,
                    new tree::Binop(tree::Type::PTR, "+",
                        right_tr->exp,
                        new tree::TempExp(tree::Type::INT, index_temp)
                    )
                )
            )
        ));
        
        // 更新循环变量
        stms->push_back(new tree::Move(
            new tree::TempExp(tree::Type::INT, index_temp),
            new tree::Binop(tree::Type::INT, "+",
                new tree::TempExp(tree::Type::INT, index_temp),
                new tree::Const(4)
            )
        ));
        
        // 跳回循环开始
        stms->push_back(new tree::Jump(loop_label));
        stms->push_back(new tree::LabelStm(loop_exit));
        
        // 返回结果数组
        tr_exp = new Tr_ex(new tree::Eseq(
            tree::Type::PTR,
            new tree::Seq(stms),
            new tree::TempExp(tree::Type::PTR, result_array)
        ));
        return;
    }

    Tr_ex* left_ex = nullptr;
    Tr_ex* right_ex = nullptr;
    Tr_cx* left_cx = nullptr;
    Tr_cx* right_cx = nullptr;

    if (op == "+" || op == "-" || op == "*" || op == "/" ||
        op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=") {
        node->left->accept(*this);
        left_ex = dynamic_cast<Tr_ex*>(tr_exp);
        if(left_ex == nullptr) {
            left_ex = dynamic_cast<Tr_cx*>(tr_exp)->unEx(temp_map);
        }
        node->right->accept(*this);
        right_ex = dynamic_cast<Tr_ex*>(tr_exp);
        if(right_ex == nullptr) {
            right_ex = dynamic_cast<Tr_cx*>(tr_exp)->unEx(temp_map);
        }
    }
    else {
        node->left->accept(*this);
        left_cx = dynamic_cast<Tr_cx*>(tr_exp);
        if(left_cx == nullptr) {
            left_cx = dynamic_cast<Tr_ex*>(tr_exp)->unCx(temp_map);
        }
        node->right->accept(*this);
        right_cx = dynamic_cast<Tr_cx*>(tr_exp);
        if(right_cx == nullptr) {
            right_cx = dynamic_cast<Tr_ex*>(tr_exp)->unCx(temp_map);
        }
        DEBUG_PRINT("visit fdmj::BinaryOp" << " tr_exp: " << tr_exp<<"=====");
    }

    if (op == "+" || op == "-" || op == "*" || op == "/") {
        tr_exp = new Tr_ex(new tree::Binop(tree::Type::INT, op, left_ex->exp, right_ex->exp));
    }
    else if (op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=") {
        tree::Label* t_label = temp_map->newlabel();
        tree::Label* f_label = temp_map->newlabel();
        
        Patch_list* true_list = new Patch_list(); 
        true_list->add_patch(t_label);
        
        Patch_list* false_list = new Patch_list(); 
        false_list->add_patch(f_label);
        
        tree::Cjump* cjump = new tree::Cjump(op, left_ex->exp, right_ex->exp, t_label, f_label);
        tr_exp = new Tr_cx(true_list, false_list, cjump);
    }
    else if (op == "||") {
        DEBUG_PRINT("visit fdmj::BinaryOp" << " node op: " << op);
        tree::Label* second_label = temp_map->newlabel();
        
        left_cx->false_list->patch(second_label);
        
        vector<tree::Stm*>* sl = new vector<tree::Stm*>();
        sl->push_back(left_cx->stm);
        sl->push_back(new tree::LabelStm(second_label));
        sl->push_back(right_cx->stm);
        
        Patch_list* true_list = new Patch_list();
        for(auto label : *(right_cx->true_list->patch_list)) {
            true_list->add_patch(label);
        }
        left_cx->true_list->add(true_list);
        
        tr_exp = new Tr_cx(left_cx->true_list, right_cx->false_list, new tree::Seq(sl));
    }
    else if (op == "&&") {
        tree::Label* second_label = temp_map->newlabel();
        
        left_cx->true_list->patch(second_label);
        
        vector<tree::Stm*>* sl = new vector<tree::Stm*>();
        sl->push_back(left_cx->stm);
        sl->push_back(new tree::LabelStm(second_label));
        sl->push_back(right_cx->stm);
        
        Patch_list* false_list = new Patch_list();
        for(auto label : *(right_cx->false_list->patch_list)) {
            false_list->add_patch(label);
        }
        left_cx->false_list->add(false_list);
        
        tr_exp = new Tr_cx(right_cx->true_list, left_cx->false_list, new tree::Seq(sl));
    }
}

void ASTToTreeVisitor::visit(fdmj::UnaryOp* node) {
    // 处理操作数
    node->exp->accept(*this);
    Tr_ex* exp_tr = dynamic_cast<Tr_ex*>(tr_exp);
    if(exp_tr == nullptr) {
        exp_tr = dynamic_cast<Tr_cx*>(tr_exp)->unEx(temp_map);
    }
    if (node->op->op == "-") {
        tr_exp = new Tr_ex(new tree::Binop(tree::Type::INT, "-", new tree::Const(0), exp_tr->exp));
    } else if (node->op->op == "!") {
        tr_exp = new Tr_ex(new tree::Binop(tree::Type::INT, "xor", new tree::Const(1), exp_tr->exp));
    }
}

void ASTToTreeVisitor::visit(fdmj::ArrayExp* node) {
    DEBUG_PRINT("visit fdmj::ArrayExp");
    // 获取数组基地址
    //DEBUG_PRINT("ArrayExp Type: "<<node->arr->type->typeKind);
    node->arr->accept(*this);
    Tr_ex* array_tr = dynamic_cast<Tr_ex*>(tr_exp);
    if(array_tr == nullptr) {
        array_tr = dynamic_cast<Tr_cx*>(tr_exp)->unEx(temp_map);
    }
    
    // 获取索引值
    node->index->accept(*this);
    Tr_ex* index_tr = dynamic_cast<Tr_ex*>(tr_exp);
    if(index_tr == nullptr) {
        index_tr = dynamic_cast<Tr_cx*>(tr_exp)->unEx(temp_map);
    }
    tree::Temp* index_temp = nullptr;
    tree::Stm* index_stm = nullptr;
    // 添加数组下标检查
    tree::Temp* len_temp = temp_map->newtemp();
    vector<tree::Stm*>* check_stms = new vector<tree::Stm*>();

    if(node->index->getASTKind() != ASTKind::IntExp && node->index->getASTKind() != ASTKind::IdExp){ // not constant
        index_temp = temp_map->newtemp();
        index_stm = new tree::Move(
            new tree::TempExp(tree::Type::INT, index_temp),
            index_tr->exp
        );
    }
    // 读取数组长度到临时变量
    check_stms->push_back(new tree::Move(
        new tree::TempExp(tree::Type::INT, len_temp),
        new tree::Mem(tree::Type::INT, array_tr->exp)
    ));
    
    // 检查下标是否越界
    tree::Label* error_label = temp_map->newlabel();
    tree::Label* ok_label = temp_map->newlabel();
    
    
    check_stms->push_back(new tree::Cjump(
        ">=",
        index_temp == nullptr ? index_tr->exp : new tree::TempExp(tree::Type::INT, index_temp),
        new tree::TempExp(tree::Type::INT, len_temp),
        error_label,
        ok_label
    ));
    
    check_stms->push_back(new tree::LabelStm(error_label));
    // 越界时调用exit(-1)
    vector<tree::Exp*>* exit_args = new vector<tree::Exp*>();
    exit_args->push_back(new tree::Const(-1));
    check_stms->push_back(new tree::ExpStm(
        new tree::ExtCall(tree::Type::INT, "exit", exit_args)
    ));
    
    check_stms->push_back(new tree::LabelStm(ok_label));
    
    // 计算实际访问地址
    tree::Exp* offset = new tree::Binop(
        tree::Type::INT, 
        "*",
        new tree::Binop(
            tree::Type::INT,
            "+",
            new tree::Eseq(
                tree::Type::INT,
                new tree::Seq(check_stms),
                index_temp == nullptr ? index_tr->exp : new tree::TempExp(tree::Type::INT, index_temp)
            ),
            new tree::Const(1)  // 加1是因为数组第一个位置存储长度
        ),
        new tree::Const(4)
    );
    
    tree::Exp* addr = new tree::Binop(tree::Type::PTR, "+", array_tr->exp, offset);
    
    if(index_temp != nullptr) {
        std::vector<tree::Stm*> *seq_stms = new std::vector<tree::Stm*>();
        seq_stms->push_back(index_stm);
        tree::Seq* seq = new tree::Seq(seq_stms);
        tree::Eseq* eseq = new tree::Eseq(
            tree::Type::INT,
            seq,
            new tree::Mem(tree::Type::INT, addr)
        );
        tr_exp = new Tr_ex(eseq);
    }
    else {
        tr_exp = new Tr_ex(new tree::Mem(tree::Type::INT, addr));
    }
    
}

void ASTToTreeVisitor::visit(fdmj::CallExp* node) {
    tree::Exp* obj_exp = nullptr;
    if (node->obj != nullptr) {
        node->obj->accept(*this);
        Tr_ex* obj_tr = dynamic_cast<Tr_ex*>(tr_exp);
        obj_exp = obj_tr->exp;
    }
    
    // 处理参数
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    if (node->par != nullptr) {
        for (auto arg : *(node->par)) {
            arg->accept(*this);
            Tr_ex* arg_tr = dynamic_cast<Tr_ex*>(tr_exp);
            if(arg_tr == nullptr) {
                arg_tr = dynamic_cast<Tr_cx*>(tr_exp)->unEx(temp_map);
            }
            if (arg_tr != nullptr) {
                args->push_back(arg_tr->exp);
            }
        }
    }   
    string method_name = node->name->id;
    tr_exp = new Tr_ex(new tree::Call(tree::Type::INT, method_name, obj_exp, args));
}

void ASTToTreeVisitor::visit(fdmj::ClassVar* node) {
    node->obj->accept(*this);
    Tr_ex* obj_tr = dynamic_cast<Tr_ex*>(tr_exp);
    
    int offset = 0;
    
    // 计算地址：obj + offset
    tree::Exp* addr = new tree::Binop(tree::Type::PTR, "+", obj_tr->exp, new tree::Const(offset));
    
    // 创建内存访问
    tr_exp = new Tr_ex(new tree::Mem(tree::Type::INT, addr));
}

void ASTToTreeVisitor::visit(fdmj::BoolExp* node) {
    // 创建布尔常量
    tr_exp = new Tr_ex(new tree::Const(node->val ? 1 : 0));
}

void ASTToTreeVisitor::visit(fdmj::This* node) {
    // 获取this指针
    tree::Temp* temp = temp_map->newtemp();
    tr_exp = new Tr_ex(new tree::TempExp(tree::Type::PTR, temp));
}

void ASTToTreeVisitor::visit(fdmj::IntExp* node) {
    DEBUG_PRINT("visit fdmj::IntExp"<<" node val: "<<node->val);
    // 创建整数常量表达式
    tr_exp = new Tr_ex(new tree::Const(node->val));
}

void ASTToTreeVisitor::visit(fdmj::IdExp* node) {
    DEBUG_PRINT("visit fdmj::IdExp"<<" node id: "<<node->id);
    tree::Temp* temp = current_mvt->var_temp_map->at(node->id);
    tree::Type type = current_mvt->var_type_map->at(node->id);
    tr_exp = new Tr_ex(new tree::TempExp(type, temp));
}

void ASTToTreeVisitor::visit(fdmj::Length* node) {
    // 处理数组长度表达式
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        Tr_ex* array_tr = dynamic_cast<Tr_ex*>(tr_exp);
        // 返回数组长度
        tr_exp = new Tr_ex(new tree::Mem(tree::Type::INT, array_tr->exp));
    } else {
        tr_exp = nullptr;
    }
}

void ASTToTreeVisitor::visit(fdmj::OpExp* node) {
}

void ASTToTreeVisitor::visit(fdmj::Esc* node) {
    DEBUG_PRINT("visit fdmj::Esc");
    // 处理转义序列,需要先处理语句列表,然后处理表达式
    vector<tree::Stm*>* sl = new vector<tree::Stm*>();
    
    if (node->sl != nullptr) {
        for (auto stm : *(node->sl)) {
            stm->accept(*this);
            if (tr_exp != nullptr) {
                tree::Stm* tree_stm = dynamic_cast<Tr_nx*>(tr_exp)->stm;
                if (tree_stm != nullptr) {
                    sl->push_back(tree_stm);
                }
            }
        }
    }
    
    tree::Exp* exp = nullptr;
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        exp = dynamic_cast<Tr_ex*>(tr_exp)->exp;
    }
    tree::Stm* seq = new tree::Seq(sl);
    tr_exp = new Tr_ex(new tree::Eseq(exp->type, seq, exp));
}

void ASTToTreeVisitor::visit(fdmj::GetInt* node) {
    // 创建 getint 外部调用
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    tr_exp = new Tr_ex(new tree::ExtCall(tree::Type::INT, "getint", args));
}

void ASTToTreeVisitor::visit(fdmj::GetCh* node) {
    // 创建 getch 外部调用
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    tr_exp = new Tr_ex(new tree::ExtCall(tree::Type::INT, "getch", args));
}

void ASTToTreeVisitor::visit(fdmj::GetArray* node) {
    // 处理获取数组元素
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        Tr_ex* array_tr = dynamic_cast<Tr_ex*>(tr_exp);
        if(array_tr == nullptr) {
            array_tr = dynamic_cast<Tr_cx*>(tr_exp)->unEx(temp_map);
        }
        vector<tree::Exp*>* args = new vector<tree::Exp*>();
        args->push_back(array_tr->exp);
        tr_exp = new Tr_ex(new tree::ExtCall(tree::Type::PTR, "getarray", args));
    } else {
        tr_exp = nullptr;
    }
}

tree::Program* ast2tree(fdmj::Program* prog, AST_Semant_Map* semant_map) {
    DEBUG_PRINT("start ast2tree");
    ASTToTreeVisitor visitor;

    visitor.name_maps = semant_map->getNameMaps();
    visitor.temp_map = new Temp_map();
    prog->accept(visitor);
    
    return dynamic_cast<tree::Program*>(visitor.getTree());
}