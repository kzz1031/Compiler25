#define DEBUG
// #undef DEBUG

#include <iostream>
#include <variant>
#include <map>
#include <vector>
#include <algorithm>
#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "namemaps.hh"

using namespace std;
using namespace fdmj;

#ifdef DEBUG
#define DEBUG_PRINT(msg) std::cerr << msg << std::endl
#else
#define DEBUG_PRINT(msg)
#endif


static string current_class = "";
static string current_method = "";

void AST_Name_Map_Visitor::visit(Program* node) {
    if (node == nullptr) return;
    
    if (node->main != nullptr) {
        node->main->accept(*this);
    }

    if (node->cdl != nullptr) {
        for (auto cl : *(node->cdl)) {
            cl->accept(*this);
        }
    }
}

void AST_Name_Map_Visitor::visit(MainMethod* node) {
    if (node == nullptr) return;
    
    // 将 main 方法视为特殊类 "^_main" 的一个方法
    current_class = "^_main";
    current_method = "main";
    
    // 添加特殊类和方法
    name_maps->add_class("^_main");
    name_maps->add_method("^_main", "main");
    
    // 添加返回类型作为形参
    Formal* return_formal = new Formal(
        node->getPos(),
        new Type(node->getPos(), TypeKind::INT, nullptr, nullptr), // 修复构造函数调用
        new IdExp(node->getPos(), "^_method_return")
    );
    name_maps->add_method_formal("^_main", "main", "^_method_return", return_formal);
    
    // 添加形参列表
    vector<string> formal_names = {"^_method_return"};
    name_maps->add_method_formal_list("^_main", "main", formal_names);
    
    // 处理局部变量
    if (node->vdl != nullptr) {
        for (auto vd : *(node->vdl)) {
            vd->accept(*this);
        }
    }
    
    // 处理语句
    if (node->sl != nullptr) {
        for (auto s : *(node->sl)) {
            s->accept(*this);
        }
    }
}

void AST_Name_Map_Visitor::visit(ClassDecl* node) {
    if (node == nullptr) return;
    
    current_class = node->id->id;
    current_method = "";
    
    // Add class to name maps
    name_maps->add_class(node->id->id);
    
    // Handle inheritance
    if (node->eid != nullptr) {
        name_maps->add_class_hiearchy(node->id->id, node->eid->id);
    }
    
    // Visit class variables
    if (node->vdl != nullptr) {
        for (auto vd : *(node->vdl)) {
            vd->accept(*this);
        }
    }
    
    // Visit methods
    if (node->mdl != nullptr) {
        for (auto md : *(node->mdl)) {
            md->accept(*this);
        }
    }
}

void AST_Name_Map_Visitor::visit(MethodDecl* node) {
    if (node == nullptr) return;
    
    current_method = node->id->id;
    name_maps->add_method(current_class, current_method);
    
    // 添加返回值类型作为形参
    Formal* return_formal = new Formal(
        node->getPos(),
        new Type(node->getPos(), node->type->typeKind, node->type->cid, node->type->arity),
        new IdExp(node->getPos(), "^_method_return")
    );

    vector<string> formal_names;
    if (node->fl != nullptr) {
        for (auto f : *(node->fl)) {
            formal_names.push_back(f->id->id);
            DEBUG_PRINT("Adding formal parameter: " << current_class << "->" << current_method << "->" << f->id->id<< "type: " << static_cast<int>(f->type->typeKind));
            f->accept(*this);
            name_maps->add_method_formal(current_class, current_method, f->id->id, f);
        }
    }
    formal_names.push_back("^_method_return");
    name_maps->add_method_formal(current_class, current_method, "^_method_return", return_formal);
    name_maps->add_method_formal_list(current_class, current_method, formal_names);
    
    // Visit variable declarations
    if (node->vdl != nullptr) {
        for (auto vd : *(node->vdl)) {
            vd->accept(*this);
        }
    }
    
    // Visit statements
    if (node->sl != nullptr) {
        for (auto s : *(node->sl)) {
            s->accept(*this);
        }
    }
}

void AST_Name_Map_Visitor::visit(VarDecl* node) {
    if (node == nullptr) return;
    
    if (current_method.empty()) {
    
        DEBUG_PRINT("Checking class variable: " << current_class << "->" << node->id->id);
        if (name_maps->is_class_var(current_class, node->id->id)) {
            cerr << "Error: Variable " << node->id->id << " already declared in class " << current_class << endl;
            exit(EXIT_FAILURE);
        }
        name_maps->add_class_var(current_class, node->id->id, node);
    } else {

        DEBUG_PRINT("Checking method variable: " << current_class << "->" << current_method << "->" << node->id->id);
        if (name_maps->is_method_var(current_class, current_method, node->id->id) ||
            name_maps->is_method_formal(current_class, current_method, node->id->id)) {
            cerr << "Error: Variable " << node->id->id << " already declared in method " 
                 << current_class << "->" << current_method << endl;
            exit(EXIT_FAILURE);
        }
        name_maps->add_method_var(current_class, current_method, node->id->id, node);
    }
}

void AST_Name_Map_Visitor::visit(Formal* node) {
    if (node == nullptr) return;
    name_maps->add_method_formal(current_class, current_method, node->id->id, node);
}

// For statements and expressions, we just need to traverse the AST
void AST_Name_Map_Visitor::visit(Nested* node) {
    if (node == nullptr || node->sl == nullptr) return;
    for (auto s : *(node->sl)) {
        if (s != nullptr) s->accept(*this);
    }
}

void AST_Name_Map_Visitor::visit(If* node) {
    if (node == nullptr) return;
    if (node->exp != nullptr) node->exp->accept(*this);
    if (node->stm1 != nullptr) node->stm1->accept(*this);
    if (node->stm2 != nullptr) node->stm2->accept(*this);
}

void AST_Name_Map_Visitor::visit(While* node) {
    if (node == nullptr) return;
    if (node->exp != nullptr) node->exp->accept(*this);
    if (node->stm != nullptr) node->stm->accept(*this);
}

void AST_Name_Map_Visitor::visit(Assign* node) {
    if (node == nullptr) return;
    if (node->left != nullptr) node->left->accept(*this);
    if (node->exp != nullptr) node->exp->accept(*this);
}

void AST_Name_Map_Visitor::visit(CallStm* node) {
    if (node == nullptr) return;
    if (node->obj != nullptr) node->obj->accept(*this);
    if (node->name != nullptr) node->name->accept(*this);
    if (node->par != nullptr) {
        for (auto p : *(node->par)) {
            if (p != nullptr) p->accept(*this);
        }
    }
}

// 以下是一些简单的遍历方法
void AST_Name_Map_Visitor::visit(Continue* node) {}
void AST_Name_Map_Visitor::visit(Break* node) {}
void AST_Name_Map_Visitor::visit(Return* node) {
    if (node != nullptr && node->exp != nullptr) node->exp->accept(*this);
}

void AST_Name_Map_Visitor::visit(PutInt* node) {
    if (node != nullptr && node->exp != nullptr) node->exp->accept(*this);
}

void AST_Name_Map_Visitor::visit(PutCh* node) {
    if (node != nullptr && node->exp != nullptr) node->exp->accept(*this);
}

void AST_Name_Map_Visitor::visit(PutArray* node) {
    if (node == nullptr) return;
    if (node->n != nullptr) node->n->accept(*this);
    if (node->arr != nullptr) node->arr->accept(*this);
}

void AST_Name_Map_Visitor::visit(Starttime* node) {}
void AST_Name_Map_Visitor::visit(Stoptime* node) {}

void AST_Name_Map_Visitor::visit(BinaryOp* node) {
    if (node == nullptr) return;
    if (node->left != nullptr) node->left->accept(*this);
    if (node->right != nullptr) node->right->accept(*this);
}

void AST_Name_Map_Visitor::visit(UnaryOp* node) {
    if (node != nullptr && node->exp != nullptr) node->exp->accept(*this);
}

void AST_Name_Map_Visitor::visit(ArrayExp* node) {
    if (node == nullptr) return;
    if (node->arr != nullptr) node->arr->accept(*this);
    if (node->index != nullptr) node->index->accept(*this);
}

void AST_Name_Map_Visitor::visit(CallExp* node) {
    if (node == nullptr) return;
    if (node->obj != nullptr) node->obj->accept(*this);
    if (node->name != nullptr) node->name->accept(*this);
    if (node->par != nullptr) {
        for (auto p : *(node->par)) {
            if (p != nullptr) p->accept(*this);
        }
    }
}

void AST_Name_Map_Visitor::visit(ClassVar* node) {
    if (node == nullptr) return;
    if (node->obj != nullptr) node->obj->accept(*this);
    if (node->id != nullptr) node->id->accept(*this);
}

void AST_Name_Map_Visitor::visit(BoolExp* node) {}
void AST_Name_Map_Visitor::visit(This* node) {}

void AST_Name_Map_Visitor::visit(Length* node) {
    if (node != nullptr && node->exp != nullptr) node->exp->accept(*this);
}

void AST_Name_Map_Visitor::visit(Esc* node) {
    if (node == nullptr) return;
    if (node->sl != nullptr) {
        for (auto s : *(node->sl)) {
            if (s != nullptr) s->accept(*this);
        }
    }
    if (node->exp != nullptr) node->exp->accept(*this);
}

void AST_Name_Map_Visitor::visit(GetInt* node) {}
void AST_Name_Map_Visitor::visit(GetCh* node) {}

void AST_Name_Map_Visitor::visit(GetArray* node) {
    if (node != nullptr && node->exp != nullptr) node->exp->accept(*this);
}

void AST_Name_Map_Visitor::visit(IdExp* node) {}
void AST_Name_Map_Visitor::visit(IntExp* node) {}
void AST_Name_Map_Visitor::visit(OpExp* node) {}
void AST_Name_Map_Visitor::visit(Type* node) {}
