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
    XMLDeclaration *decl = v.doc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    v.doc->InsertFirstChild(decl);
    v.visit_result = nullptr;
    if (prog != nullptr) {
        prog->accept(v);
    }
    else 
        v.visit_result = nullptr;
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
    if (node->args != nullptr && node->args->size() > 0) {
        int i=0;
        for (auto arg : *node->args) {
            element->SetAttribute(("arg_temp_"+std::to_string(i)).c_str(), arg->name());
            i++;
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
        int i=0;
        for (auto label : *block->exit_labels) {
            element->SetAttribute(("exit_label_"+to_string(i)).c_str(), label->name());
        }
    }
    XMLElement* stmsElement = doc->NewElement("Sequence");
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
    element->SetAttribute("false", node->f->name());
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
    if (node->sl == nullptr || node->sl->size() == 0) {
        //empty block
        visit_result = nullptr;
        return;
    }
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
    int i = 0;
    for (auto arg : *node->args) {
        element->SetAttribute(("temp_"+to_string(i)).c_str(), arg.first->name());
        element->SetAttribute(("from_"+to_string(i)).c_str(), arg.second->name());
        i = i + 1;
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
    if (node->exp == nullptr) {
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
    element->SetAttribute("type", node->type == Type::INT ? "INT" : "PTR");
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
    element->SetAttribute("type", node->type == Type::INT ? "INT" : "PTR");
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
    element->SetAttribute("type", node->type == Type::INT ? "INT" : "PTR");
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
    Label *label = node->name;
    String_Label *slabel = node->sname;
    string name;
    if (label != nullptr) { //then it's a string_label
        name = label->name();
    } else if (slabel != nullptr) {
        name = slabel->str();
    } else {
        cerr << "Error: Name has no label or string label!" << endl;
        visit_result = nullptr;
        return;
    }   
    element->SetAttribute("name", name.c_str());
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
    element->SetAttribute("id", node->id.c_str());
    element->SetAttribute("type", node->type == Type::INT ? "INT" : "PTR");
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
    cout << "Visiting ExtCall=" << ((!node)?"NULL!":(static_cast<ExtCall*>(node)->extfun).c_str()) << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    tinyxml2::XMLElement* element = doc->NewElement("ExtCall");
    element->SetAttribute("extfun", node->extfun.c_str());
    element->SetAttribute("type", node->type == Type::INT ? "INT" : "PTR");
    tinyxml2::XMLElement* argsElement = doc->NewElement("Arguments");
    if (node->args != nullptr) {
        for (auto arg : *node->args) {
            arg->accept(*this);
            if (visit_result != nullptr) argsElement->InsertEndChild(visit_result);
        }
    }
    element->InsertEndChild(argsElement);
    visit_result = element;
}
