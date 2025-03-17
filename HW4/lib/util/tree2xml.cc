#define DEBUG
#undef DEBUG

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "treep.hh"
#include "tinyxml2.hh"
#include "tree2xml.hh"

using namespace std;
using namespace tree;
using namespace tinyxml2;

#define FuncDeclList vector<FuncDecl*>

XMLDocument* tree2xml(Program* prog) {
#ifdef DEBUG
    cout << "in tree2xml::Converting IR to XML" << endl;
#endif
    Tree2XML v;
    v.doc = new XMLDocument();
    v.visit_result = nullptr;
    prog->accept(v);
    if (v.visit_result != nullptr) 
        v.doc->InsertEndChild(v.visit_result);
    return v.doc;
}

void Tree2XML::visit(Program* node) {
#ifdef DEBUG
    cout << "Visiting Program" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    XMLElement* element = doc->NewElement("Program");
    if (node->funcdecllist != nullptr) {
        for (auto func : *node->funcdecllist) {
            func->accept(*this);
            if (visit_result != nullptr) element->InsertEndChild(visit_result);
        }
    }
    visit_result = element;
}

void Tree2XML::visit(FuncDecl* node) {
#ifdef DEBUG
    cout << "Visiting FunctionDeclaration" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    XMLElement* element = doc->NewElement("FunctionDeclaration");
    element->SetAttribute("name", node->name.c_str());
    element->SetAttribute("return_type", tree::typeToString(node->return_type).c_str());
    element->SetAttribute("last_temp", node->last_temp_num);
    element->SetAttribute("last_label", node->last_label_num);
    if (node->args != nullptr) {
        for (auto arg : *node->args) {
            element->SetAttribute("arg_temp", arg->name());
        }
    }
    XMLElement *blocksElement = doc->NewElement("Blocks");
    if (node->blocks != nullptr) {
        for (auto block : *node->blocks) {
            block->accept(*this);
            if (visit_result != nullptr) blocksElement->InsertEndChild(visit_result);
        }
    }
    element->InsertEndChild(blocksElement);
    visit_result = element;
#ifdef DEBUG
    cout << "FunctionDeclaration visited" << endl;  
#endif
}

void Tree2XML::visit(Block *block) {
#ifdef DEBUG
    cout << "Visiting Block" << endl;
#endif
    if (block == nullptr) {
        visit_result = nullptr;
        return;
    }
    XMLElement* element = doc->NewElement("Block");
    element->SetAttribute("entry_label", block->entry_label->name());
    //XMLElement* exitLabelsElement = doc->NewElement("ExitLabels");
    if (block->exit_labels != nullptr) {
        for (auto label : *block->exit_labels) {
            element->SetAttribute("exit_label", label->name());
        }
    }
    XMLElement* stmsElement = doc->NewElement("Statements");
    if (block->sl != nullptr) {
        for (auto stm : *block->sl) {
            stm->accept(*this);
            if (visit_result != nullptr) stmsElement->InsertEndChild(visit_result);
        }
    }
    element->InsertEndChild(stmsElement);
    visit_result = element;
}

void Tree2XML::visit(Jump* node) {
#ifdef DEBUG
    cout << "Visiting Jump" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    XMLElement* element = doc->NewElement("Jump");
    element->SetAttribute("label", node->label->name());
    visit_result = element;
}

void Tree2XML::visit(Cjump* node) {
#ifdef DEBUG
    cout << "Visiting CJump" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    XMLElement* element = doc->NewElement("CJump");
    element->SetAttribute("relop", node->relop.c_str());
    element->SetAttribute("true", node->t->name());
    element->SetAttribute("false", node->t->name());
    node->left->accept(*this);
    if (visit_result != nullptr) element->InsertEndChild(visit_result);
    node->right->accept(*this);
    if (visit_result != nullptr) element->InsertEndChild(visit_result);
    visit_result = element;
}

void Tree2XML::visit(Move* node) {
#ifdef DEBUG
    cout << "Visiting Move" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    XMLElement* element = doc->NewElement("Move");
    node->dst->accept(*this);
    if (visit_result != nullptr) element->InsertEndChild(visit_result);
    node->src->accept(*this);
    if (visit_result != nullptr) element->InsertEndChild(visit_result);
    visit_result = element;
}

void Tree2XML::visit(Seq* node) {
#ifdef DEBUG
    cout << "Visiting Sequence" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    tinyxml2::XMLElement* element = doc->NewElement("Sequence");
    for (auto stm : *node->sl) {
        stm->accept(*this);
        if (visit_result != nullptr) element->InsertEndChild(visit_result);
    }
    visit_result = element;
}

void Tree2XML::visit(LabelStm* node) {
#ifdef DEBUG
    cout << "Visiting Label" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    XMLElement* element = doc->NewElement("Label");
    element->SetAttribute("label", node->label->name());
    visit_result = element;
}

void Tree2XML::visit(Return* node) {
#ifdef DEBUG
    cout << "Visiting Return" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    XMLElement* element = doc->NewElement("Return");
    node->exp->accept(*this);
    if (visit_result != nullptr) element->InsertEndChild(visit_result); 
    visit_result = element;
}

void Tree2XML::visit(Phi* node) {
#ifdef DEBUG
    cout << "Visiting Phi" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    XMLElement* element = doc->NewElement("Phi");
    element->SetAttribute("define", node->temp->name());
    for (auto arg : *node->args) {
        element->SetAttribute("arg", arg.first->name());
        element->SetAttribute("from", arg.second->name());
    }
    visit_result = element;
}

void Tree2XML::visit(ExpStm* node) {
#ifdef DEBUG
    cout << "Visiting ExpressionStatement" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    XMLElement* element = doc->NewElement("ExpressionStatement");
    node->exp->accept(*this);
    if (visit_result != nullptr) element->InsertEndChild(visit_result);
    visit_result = element;
}

void Tree2XML::visit(Binop* node) {
#ifdef DEBUG
    cout << "Visiting BinaryOperation" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    tinyxml2::XMLElement* element = doc->NewElement("BinOp");
    element->SetAttribute("op", node->op.c_str());
    node->left->accept(*this);
    if (visit_result != nullptr) element->InsertEndChild(visit_result);
    node->right->accept(*this);
    if (visit_result != nullptr) element->InsertEndChild(visit_result);
    visit_result = element;
}

void Tree2XML::visit(Mem* node) {
#ifdef DEBUG
    cout << "Visiting Memory" << endl;
#endif
    XMLElement* element = doc->NewElement("Memory");
    node->mem->accept(*this);
    if (visit_result != nullptr) element->InsertEndChild(visit_result);
    visit_result = element;
}

void Tree2XML::visit(TempExp* node) {
#ifdef DEBUG
    cout << "Visiting Temp" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    XMLElement* element = doc->NewElement("Temp");
    element->SetAttribute("temp", node->temp->name());
    visit_result = element;
}

void Tree2XML::visit(Eseq* node) {
#ifdef DEBUG
    cout << "Visiting ESeq" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    } 
    XMLElement* element = doc->NewElement("ESeq");
    node->stm->accept(*this);
    if (visit_result != nullptr) element->InsertEndChild(visit_result);
    node->exp->accept(*this);
    if (visit_result != nullptr) element->InsertEndChild(visit_result);
    visit_result = element;
}

void Tree2XML::visit(Name* node) {
#ifdef DEBUG
    cout << "Visiting Name" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    XMLElement* element = doc->NewElement("Name");
    element->SetAttribute("name", node->name->name());
    visit_result = element;
}

void Tree2XML::visit(Const* node) {
#ifdef DEBUG
    cout << "Visiting Const" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    XMLElement* element = doc->NewElement("Const");
    element->SetAttribute("value", node->constVal);
    visit_result = element;
}

void Tree2XML::visit(Call* node) {
#ifdef DEBUG
    cout << "Visiting Call" << endl;
#endif
    XMLElement* element = doc->NewElement("Call");
    element->SetAttribute("id", node->id->c_str());
    node->obj->accept(*this);
    if (visit_result != nullptr) element->InsertEndChild(visit_result);
    XMLElement* argsElement = doc->NewElement("Arguments");
    for (auto arg : *node->args) {
        arg->accept(*this);
        if (visit_result != nullptr) argsElement->InsertEndChild(visit_result);
    }
    element->InsertEndChild(argsElement);
    visit_result = element;
}

void Tree2XML::visit(ExtCall* node) {
#ifdef DEBUG
    cout << "Visiting ExtCall" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    tinyxml2::XMLElement* element = doc->NewElement("ExtCall");
    element->SetAttribute("extfun", node->extfun->c_str());
    tinyxml2::XMLElement* argsElement = doc->NewElement("Arguments");
    for (auto arg : *node->args) {
        arg->accept(*this);
        if (visit_result != nullptr) argsElement->InsertEndChild(visit_result);
    }
    element->InsertEndChild(argsElement);
    visit_result = element;
}
