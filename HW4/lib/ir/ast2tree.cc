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
// 生成类表，映射每个类的变量和方法到地址偏移
Name_Maps* name_maps;
Temp_map* temp_map;

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

// 为方法生成变量表，包括形参和局部变量
Method_var_table* generate_method_var_table(string class_name, string method_name, Name_Maps* nm, Temp_map* tm) {
    Method_var_table* mvt = new Method_var_table();
    // 添加this指针
    if (class_name != "_^main^_") {
        tree::Temp* this_temp = tm->newtemp();
        mvt->var_temp_map->insert({"this", this_temp});
        mvt->var_type_map->insert({"this", tree::Type::PTR});
    }
    // 添加形参
    nm->is_method(class_name, method_name);
    if (nm->is_method(class_name, method_name)) {
        auto formal_list = nm->get_method_formal_list(class_name, method_name);
        for (auto& formal_name : *formal_list) {
            tree::Temp* formal_temp = tm->newtemp();
            mvt->var_temp_map->insert({formal_name, formal_temp});
            
            // 获取形参类型
            auto formal = nm->get_method_formal(class_name, method_name, formal_name);
            if (formal->type->typeKind == TypeKind::INT) {
                mvt->var_type_map->insert({formal_name, tree::Type::INT});
            } else {
                mvt->var_type_map->insert({formal_name, tree::Type::PTR});
            }
        }
    }
    
    // 添加局部变量
    if (nm->is_method(class_name, method_name)) {
        auto var_list = nm->get_method_var_list(class_name, method_name);
        for (auto& var_name : *var_list) {
            tree::Temp* var_temp = tm->newtemp();
            mvt->var_temp_map->insert({var_name, var_temp});
            
            // 获取变量类型
            auto var = nm->get_method_var(class_name, method_name, var_name);
            if (var->type->typeKind == TypeKind::INT) {
                mvt->var_type_map->insert({var_name, tree::Type::INT});
            } else {
                mvt->var_type_map->insert({var_name, tree::Type::PTR});
            }
        }
    }
    return mvt;
}

// 实现ASTToTreeVisitor类的方法
void ASTToTreeVisitor::visit(fdmj::Program* node) {
    // 创建函数声明列表
    DEBUG_PRINT("visit fdmj::Program");
    vector<tree::FuncDecl*>* fdl = new vector<tree::FuncDecl*>();
    
    // 处理主方法
    if (node->main != nullptr) {
        node->main->accept(*this);
        if (visit_tree_result != nullptr) {
            fdl->push_back(static_cast<tree::FuncDecl*>(visit_tree_result));
        }
    }
    
    // 处理类声明
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
    // 使用全局temp_map
    DEBUG_PRINT("visit fdmj::MainMethod");
    // 创建入口标签
    tree::Label* entry_label = temp_map->newlabel();
    tree::LabelStm* label_stm = new tree::LabelStm(entry_label);
    
    // 创建语句列表
    vector<tree::Stm*>* sl = new vector<tree::Stm*>();
    sl->push_back(label_stm);
    
    Method_var_table* mvt = generate_method_var_table("_^main^_", "main", name_maps, temp_map);
    // 处理变量声明
    if (node->vdl != nullptr) {
        for (auto varDecl : *(node->vdl)) {
            DEBUG_PRINT("visit fdmj::MainMethod"<<" varDecl id: "<<varDecl->id->id);
            // 为每个变量创建临时变量
            string var_name = varDecl->id->id;
            tree::Temp* var_temp = temp_map->newtemp();
            
            // 添加到方法变量表
            mvt->var_temp_map->insert({var_name, var_temp});
            
            // 确定变量类型
            if (varDecl->type->typeKind == TypeKind::INT) {
                mvt->var_type_map->insert({var_name, tree::Type::INT});
            } else {
                mvt->var_type_map->insert({var_name, tree::Type::PTR});
            }
            
            // 处理初始化（如果有）
            if (varDecl->init.index() != 0) { // 不是monostate
                if (varDecl->init.index() == 1) { // IntExp*
                    IntExp* init_int = get<IntExp*>(varDecl->init);
                    int value = init_int->val;
                    sl->push_back(new tree::Move(new tree::TempExp(tree::Type::INT, var_temp), new tree::Const(value)));
                }
                // 数组初始化暂不处理
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
    
    // 创建块
    vector<tree::Block*>* bl = new vector<tree::Block*>();
    tree::Block* block = new tree::Block(entry_label, nullptr, sl);
    bl->push_back(block);
    
    // 创建函数声明
    visit_tree_result = new tree::FuncDecl("_^main^_^main", nullptr, bl, tree::Type::INT, temp_map->next_temp, temp_map->next_label);
}

void ASTToTreeVisitor::visit(fdmj::ClassDecl* node) {
    // 类声明会生成多个函数声明（每个方法一个）
    // 这里只处理第一个方法，其他方法需要在Program的visit中处理
    if (node->mdl != nullptr && !node->mdl->empty()) {
        node->mdl->at(0)->accept(*this);
    } else {
        visit_tree_result = nullptr;
    }
}

void ASTToTreeVisitor::visit(fdmj::Type* node) {
    // 不需要直接处理类型节点
    visit_tree_result = nullptr;
}

void ASTToTreeVisitor::visit(fdmj::VarDecl* node) {
    // 变量声明在方法访问中处理
    DEBUG_PRINT("visit fdmj::VarDecl: "<<node->id->id);
    visit_tree_result = nullptr;
}

void ASTToTreeVisitor::visit(fdmj::MethodDecl* node) {
    // 使用全局temp_map
    
    // 创建入口标签
    tree::Label* entry_label = temp_map->newlabel();
    tree::LabelStm* label_stm = new tree::LabelStm(entry_label);
    
    // 创建语句列表
    vector<tree::Stm*>* sl = new vector<tree::Stm*>();
    sl->push_back(label_stm);
    
    // 获取方法变量表
    // 这里需要知道类名，暂时使用空字符串
    Method_var_table* mvt = generate_method_var_table("", node->id->id, nullptr, temp_map);
    
    // 处理形参
    vector<tree::Temp*>* args = new vector<tree::Temp*>();
    if (node->fl != nullptr) {
        for (auto formal : *(node->fl)) {
            formal->accept(*this);
            // 形参处理在方法变量表中完成
        }
    }
    
    // 处理变量声明
    if (node->vdl != nullptr) {
        for (auto varDecl : *(node->vdl)) {
            // 为每个变量创建临时变量
            string var_name = varDecl->id->id;
            tree::Temp* var_temp = temp_map->newtemp();
            
            // 添加到方法变量表
            mvt->var_temp_map->insert({var_name, var_temp});
            
            // 确定变量类型
            if (varDecl->type->typeKind == TypeKind::INT) {
                mvt->var_type_map->insert({var_name, tree::Type::INT});
            } else {
                mvt->var_type_map->insert({var_name, tree::Type::PTR});
            }
            
            // 处理初始化（如果有）
            if (varDecl->init.index() != 0) { // 不是monostate
                if (varDecl->init.index() == 1) { // IntExp*
                    IntExp* init_int = get<IntExp*>(varDecl->init);
                    int value = init_int->val;
                    sl->push_back(new tree::Move(new tree::TempExp(tree::Type::INT, var_temp), new tree::Const(value)));
                }
                // 数组初始化暂不处理
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
    
    // 创建块
    vector<tree::Block*>* bl = new vector<tree::Block*>();
    tree::Block* block = new tree::Block(entry_label, nullptr, sl);
    bl->push_back(block);
    
    // 创建函数声明
    string method_name = "_" + node->id->id; // 临时方法名
    visit_tree_result = new tree::FuncDecl(method_name, args, bl, tree::Type::INT, temp_map->next_temp, temp_map->next_label);
}

void ASTToTreeVisitor::visit(fdmj::Formal* node) {
    // 形参在方法声明中处理
    visit_tree_result = nullptr;
}

void ASTToTreeVisitor::visit(fdmj::Nested* node) {
    // 处理嵌套语句
    if (node->sl != nullptr) {
        vector<tree::Stm*>* sl = new vector<tree::Stm*>();
        
        for (auto stm : *(node->sl)) {
            stm->accept(*this);
            if (visit_tree_result != nullptr) {
                tree::Stm* tree_stm = dynamic_cast<tree::Stm*>(visit_tree_result);
                if (tree_stm != nullptr) {
                    sl->push_back(tree_stm);
                }
            }
        }
        
        visit_tree_result = new tree::Seq(sl);
    } else {
        visit_tree_result = new tree::Seq();
    }
}

void ASTToTreeVisitor::visit(fdmj::If* node) {
    // 使用全局temp_map
    
    // 处理条件表达式
    node->exp->accept(*this);
    tree::Exp* cond_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    
    // 创建标签
    tree::Label* then_label = temp_map->newlabel();
    tree::Label* else_label = temp_map->newlabel();
    tree::Label* end_label = temp_map->newlabel();
    
    // 创建条件跳转
    tree::Cjump* cjump = new tree::Cjump("!=", cond_exp, new tree::Const(0), then_label, else_label);
    
    // 处理then语句
    node->stm1->accept(*this);
    tree::Stm* then_stm = dynamic_cast<tree::Stm*>(visit_tree_result);
    
    // 处理else语句（如果有）
    tree::Stm* else_stm = nullptr;
    if (node->stm2 != nullptr) {
        node->stm2->accept(*this);
        else_stm = dynamic_cast<tree::Stm*>(visit_tree_result);
    }
    
    // 创建语句序列
    vector<tree::Stm*>* sl = new vector<tree::Stm*>();
    sl->push_back(cjump);
    sl->push_back(new tree::LabelStm(then_label));
    sl->push_back(then_stm);
    sl->push_back(new tree::Jump(end_label));
    sl->push_back(new tree::LabelStm(else_label));
    if (else_stm != nullptr) {
        sl->push_back(else_stm);
    }
    sl->push_back(new tree::LabelStm(end_label));
    
    visit_tree_result = new tree::Seq(sl);
}

void ASTToTreeVisitor::visit(fdmj::While* node) {
    // 使用全局temp_map
    
    // 创建标签
    tree::Label* test_label = temp_map->newlabel();
    tree::Label* body_label = temp_map->newlabel();
    tree::Label* end_label = temp_map->newlabel();
    
    // 处理条件表达式
    node->exp->accept(*this);
    tree::Exp* cond_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    
    // 创建条件跳转
    tree::Cjump* cjump = new tree::Cjump("!=", cond_exp, new tree::Const(0), body_label, end_label);
    
    // 处理循环体
    node->stm->accept(*this);
    tree::Stm* body_stm = dynamic_cast<tree::Stm*>(visit_tree_result);
    
    // 创建语句序列
    vector<tree::Stm*>* sl = new vector<tree::Stm*>();
    sl->push_back(new tree::LabelStm(test_label));
    sl->push_back(cjump);
    sl->push_back(new tree::LabelStm(body_label));
    sl->push_back(body_stm);
    sl->push_back(new tree::Jump(test_label));
    sl->push_back(new tree::LabelStm(end_label));
    
    visit_tree_result = new tree::Seq(sl);
}

void ASTToTreeVisitor::visit(fdmj::Assign* node) {
    DEBUG_PRINT("visit fdmj::Assign");
    // 处理左值
    node->left->accept(*this);
    tree::Exp* lhs_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    
    // 处理右值
    node->exp->accept(*this);
    tree::Exp* rhs_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    
    // 创建赋值语句
    visit_tree_result = new tree::Move(lhs_exp, rhs_exp);
}

void ASTToTreeVisitor::visit(fdmj::CallStm* node) {
    // 需要先获取CallExp
    tree::Exp* obj_exp = nullptr;
    if (node->obj != nullptr) {
        node->obj->accept(*this);
        obj_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    }
    
    // 处理参数
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    if (node->par != nullptr) {
        for (auto arg : *(node->par)) {
            arg->accept(*this);
            tree::Exp* arg_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
            if (arg_exp != nullptr) {
                args->push_back(arg_exp);
            }
        }
    }
    
    visit_tree_result = new tree::Call(tree::Type::INT, node->name->id, obj_exp, args);
}

void ASTToTreeVisitor::visit(fdmj::Continue* node) {
    // 创建跳转到循环测试标签的语句
    // 需要知道当前循环的测试标签，这里简化处理
    visit_tree_result = nullptr;
}

void ASTToTreeVisitor::visit(fdmj::Break* node) {
    // 创建跳转到循环结束标签的语句
    // 需要知道当前循环的结束标签，这里简化处理
    visit_tree_result = nullptr;
}

void ASTToTreeVisitor::visit(fdmj::Return* node) {
    // 处理返回表达式
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        tree::Exp* ret_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
        visit_tree_result = new tree::Return(ret_exp);
    } else {
        visit_tree_result = new tree::Return(new tree::Const(0));
    }
}

void ASTToTreeVisitor::visit(fdmj::PutInt* node) {
    // 处理参数表达式
    node->exp->accept(*this);
    tree::Exp* arg_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    
    // 创建参数列表
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    args->push_back(arg_exp);
    
    // 创建外部调用
    tree::ExtCall* call = new tree::ExtCall(tree::Type::INT, "putint", args);
    visit_tree_result = new tree::ExpStm(call);
}

void ASTToTreeVisitor::visit(fdmj::PutCh* node) {
    // 处理参数表达式
    node->exp->accept(*this);
    tree::Exp* arg_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    
    // 创建参数列表
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    args->push_back(arg_exp);
    
    // 创建外部调用
    tree::ExtCall* call = new tree::ExtCall(tree::Type::INT, "putchar", args);
    visit_tree_result = new tree::ExpStm(call);
}

void ASTToTreeVisitor::visit(fdmj::PutArray* node) {
    // 简化处理，实际应该遍历数组并输出每个元素
    visit_tree_result = nullptr;
}

void ASTToTreeVisitor::visit(fdmj::Starttime* node) {
    // 创建外部调用
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    tree::ExtCall* call = new tree::ExtCall(tree::Type::INT, "starttime", args);
    visit_tree_result = new tree::ExpStm(call);
}

void ASTToTreeVisitor::visit(fdmj::Stoptime* node) {
    // 创建外部调用
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    tree::ExtCall* call = new tree::ExtCall(tree::Type::INT, "stoptime", args);
    visit_tree_result = new tree::ExpStm(call);
}

void ASTToTreeVisitor::visit(fdmj::BinaryOp* node) {
    DEBUG_PRINT("visit fdmj::BinaryOp" << " node op: " << node->op->op);
    
    // 处理左操作数
    node->left->accept(*this);
    tree::Exp* lhs_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    
    // 处理右操作数
    node->right->accept(*this);
    tree::Exp* rhs_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    
    string op = node->op->op;
    
    if (op == "+" || op == "-" || op == "*" || op == "/") {
        // 算术运算直接使用 Binop
        visit_tree_result = new tree::Binop(tree::Type::INT, op, lhs_exp, rhs_exp);
    } 
    else if (op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=") {
        // 比较运算使用 Tr_cx
        tree::Label* t_label = temp_map->newlabel();
        tree::Label* f_label = temp_map->newlabel();
        
        Patch_list* true_list = new Patch_list();
        Patch_list* false_list = new Patch_list();
        true_list->add_patch(t_label);
        false_list->add_patch(f_label);
        
        // 创建条件跳转
        tree::Cjump* cjump = new tree::Cjump(op, lhs_exp, rhs_exp, t_label, f_label);
        Tr_cx* cx = new Tr_cx(true_list, false_list, cjump);
        
        // 将条件结果转换为表达式
        visit_tree_result = cx->unEx(temp_map)->exp;
    }
    else if (op == "||") {
        // 处理逻辑或
        // 先将左右操作数转换为 Tr_cx
        Tr_cx* left_cx = (new Tr_ex(lhs_exp))->unCx(temp_map);
        Tr_cx* right_cx = (new Tr_ex(rhs_exp))->unCx(temp_map);
        
        tree::Label* t_label = temp_map->newlabel();
        tree::Label* f_label = temp_map->newlabel();
        tree::Label* second_label = temp_map->newlabel();
        
        // 修改左操作数的false跳转到第二个条件
        left_cx->false_list->patch(second_label);
        
        // 合并true跳转列表
        Patch_list* true_list = new Patch_list();
        Patch_list* false_list = new Patch_list();
        true_list->add_patch(t_label);
        false_list->add_patch(f_label);
        
        vector<tree::Stm*>* sl = new vector<tree::Stm*>();
        sl->push_back(left_cx->stm);
        sl->push_back(new tree::LabelStm(second_label));
        sl->push_back(right_cx->stm);
        
        // 创建新的条件跳转
        Tr_cx* result_cx = new Tr_cx(true_list, false_list, new tree::Seq(sl));
        
        // 转换为表达式
        visit_tree_result = result_cx->unEx(temp_map)->exp;
    }
    else if (op == "&&") {
        // 处理逻辑与
        // 先将左右操作数转换为 Tr_cx
        Tr_cx* left_cx = (new Tr_ex(lhs_exp))->unCx(temp_map);
        Tr_cx* right_cx = (new Tr_ex(rhs_exp))->unCx(temp_map);
        
        tree::Label* t_label = temp_map->newlabel();
        tree::Label* f_label = temp_map->newlabel();
        tree::Label* second_label = temp_map->newlabel();
        
        // 修改左操作数的true跳转到第二个条件
        left_cx->true_list->patch(second_label);
        
        // 合并false跳转列表
        Patch_list* true_list = new Patch_list();
        Patch_list* false_list = new Patch_list();
        true_list->add_patch(t_label);
        false_list->add_patch(f_label);
        
        vector<tree::Stm*>* sl = new vector<tree::Stm*>();
        sl->push_back(left_cx->stm);
        sl->push_back(new tree::LabelStm(second_label));
        sl->push_back(right_cx->stm);
        
        // 创建新的条件跳转
        Tr_cx* result_cx = new Tr_cx(true_list, false_list, new tree::Seq(sl));
        
        // 转换为表达式
        visit_tree_result = result_cx->unEx(temp_map)->exp;
    }
}

void ASTToTreeVisitor::visit(fdmj::UnaryOp* node) {
    // 使用全局temp_map
    
    // 处理操作数
    node->exp->accept(*this);
    tree::Exp* exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    
    // 创建一元操作表达式（通过二元操作实现）
    if (node->op->op == "-") {
        // 负号：0 - exp
        visit_tree_result = new tree::Binop(tree::Type::INT, "-", new tree::Const(0), exp);
    } else if (node->op->op == "!") {
        // 非：1 xor exp
        visit_tree_result = new tree::Binop(tree::Type::INT, "xor", new tree::Const(1), exp);
    }
}

void ASTToTreeVisitor::visit(fdmj::ArrayExp* node) {
    // 处理数组表达式
    node->arr->accept(*this);
    tree::Exp* array_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    
    // 处理索引表达式
    node->index->accept(*this);
    tree::Exp* index_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    
    // 计算偏移量：index * 4（假设每个元素4字节）
    tree::Exp* offset = new tree::Binop(tree::Type::INT, "*", index_exp, new tree::Const(4));
    
    // 计算地址：array + offset
    tree::Exp* addr = new tree::Binop(tree::Type::PTR, "+", array_exp, offset);
    
    // 创建内存访问
    visit_tree_result = new tree::Mem(tree::Type::INT, addr);
}

void ASTToTreeVisitor::visit(fdmj::CallExp* node) {
    // 处理对象表达式
    tree::Exp* obj_exp = nullptr;
    if (node->obj != nullptr) {
        node->obj->accept(*this);
        obj_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    }
    
    // 处理参数
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    if (node->par != nullptr) {
        for (auto arg : *(node->par)) {
            arg->accept(*this);
            tree::Exp* arg_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
            if (arg_exp != nullptr) {
                args->push_back(arg_exp);
            }
        }
    }
    
    // 创建调用表达式
    string method_name = node->name->id;
    visit_tree_result = new tree::Call(tree::Type::INT, method_name, obj_exp, args);
}

void ASTToTreeVisitor::visit(fdmj::ClassVar* node) {
    // 处理对象表达式
    node->obj->accept(*this);
    tree::Exp* obj_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    
    // 获取变量偏移量（需要类表）
    // 这里简化处理，假设偏移量为0
    int offset = 0;
    
    // 计算地址：obj + offset
    tree::Exp* addr = new tree::Binop(tree::Type::PTR, "+", obj_exp, new tree::Const(offset));
    
    // 创建内存访问
    visit_tree_result = new tree::Mem(tree::Type::INT, addr);
}

void ASTToTreeVisitor::visit(fdmj::BoolExp* node) {
    // 创建布尔常量
    visit_tree_result = new tree::Const(node->val ? 1 : 0);
}

void ASTToTreeVisitor::visit(fdmj::This* node) {
    // 使用全局temp_map
    // 获取this指针（需要方法变量表）
    tree::Temp* temp = temp_map->newtemp();
    visit_tree_result = new tree::TempExp(tree::Type::PTR, temp);
}

void ASTToTreeVisitor::visit(fdmj::IntExp* node) {
    DEBUG_PRINT("visit fdmj::IntExp"<<" node val: "<<node->val);
    // 创建整数常量表达式
    visit_tree_result = new tree::Const(node->val);
}

void ASTToTreeVisitor::visit(fdmj::IdExp* node) {
    // 使用全局temp_map
    // 创建变量表达式,需要从方法变量表中获取对应的临时变量
    // 这里简化处理,直接创建一个新的临时变量
    DEBUG_PRINT("visit fdmj::IdExp"<<" node id: "<<node->id);
    tree::Temp* temp = temp_map->newtemp();
    visit_tree_result = new tree::TempExp(tree::Type::INT, temp);
}

void ASTToTreeVisitor::visit(fdmj::Length* node) {
    // 处理数组长度表达式
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        tree::Exp* array_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
        // 返回数组长度
        visit_tree_result = new tree::Mem(tree::Type::INT, array_exp);
    } else {
        visit_tree_result = nullptr;
    }
}

void ASTToTreeVisitor::visit(fdmj::OpExp* node) {
    // OpExp不需要创建新的OpExp，只需返回操作符字符串
    visit_tree_result = new tree::Const(0); // 临时修改，实际应该返回操作符
}

void ASTToTreeVisitor::visit(fdmj::Esc* node) {
    DEBUG_PRINT("visit fdmj::Esc");
    // 处理转义序列,需要先处理语句列表,然后处理表达式
    vector<tree::Stm*>* sl = new vector<tree::Stm*>();
    
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
    
    tree::Exp* exp = nullptr;
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        exp = dynamic_cast<tree::Exp*>(visit_tree_result);
    }
    tree::Stm* seq = new tree::Seq(sl);
    visit_tree_result = new tree::Eseq(exp->type, seq, exp);
}

void ASTToTreeVisitor::visit(fdmj::GetInt* node) {
    // 创建 getint 外部调用
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    visit_tree_result = new tree::ExtCall(tree::Type::INT, "getint", args);
}

void ASTToTreeVisitor::visit(fdmj::GetCh* node) {
    // 创建 getch 外部调用
    vector<tree::Exp*>* args = new vector<tree::Exp*>();
    visit_tree_result = new tree::ExtCall(tree::Type::INT, "getch", args);
}

void ASTToTreeVisitor::visit(fdmj::GetArray* node) {
    // 处理获取数组元素,需要先处理数组表达式
    if (node->exp != nullptr) {
        node->exp->accept(*this);
        tree::Exp* array_exp = dynamic_cast<tree::Exp*>(visit_tree_result);
        // 调用获取数组的外部函数 
        vector<tree::Exp*>* args = new vector<tree::Exp*>();
        args->push_back(array_exp);
        visit_tree_result = new tree::ExtCall(tree::Type::PTR, "getarray", args);
    } else {
        visit_tree_result = nullptr;
    }
}

tree::Program* ast2tree(fdmj::Program* prog, AST_Semant_Map* semant_map) {
    DEBUG_PRINT("start ast2tree");
    temp_map = new Temp_map();
    
    ASTToTreeVisitor visitor;
    name_maps = semant_map->getNameMaps();
    prog->accept(visitor);
    return dynamic_cast<tree::Program*>(visitor.getTree());
}