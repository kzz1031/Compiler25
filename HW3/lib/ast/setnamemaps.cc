#define DEBUG
#undef DEBUG

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
    //cerr<<"main method"<<endl;
    current_class = "main";
    current_method = "main";
    name_maps->add_class("main");
    name_maps->add_method("main", "main");
    if (node->vdl != nullptr) {
        for (auto vd : *(node->vdl)) {
            vd->accept(*this);
        }
    }
    
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
    
    // Process formal parameters
    vector<string> formal_names;
    if (node->fl != nullptr) {
        for (auto f : *(node->fl)) {
            formal_names.push_back(f->id->id);
            f->accept(*this);
        }
    }
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
    
    // Handle variable declarations based on context
    if (current_method.empty()) {
        // Class variable
        name_maps->add_class_var(current_class, node->id->id, node);
    } else {
        // Method variable
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
