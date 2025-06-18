#include <iostream>
#include <string>
#include <vector>
#include "quad.hh"
#include "quad2xml.hh"
#include "tinyxml2.hh"

using namespace std;
using namespace quad;
using namespace tinyxml2;

// Function to write Quad program to an XML file
bool quad2xml(QuadProgram* program, const char* filename) {
    XMLDocument* doc = new XMLDocument();
    Quad2XML quad2xml(doc);
    program->accept(quad2xml);
    bool result = (doc->SaveFile(filename) == XML_SUCCESS);
    delete doc;
    return result;
}

// Implementation of Quad2XML methods
Quad2XML::Quad2XML(XMLDocument* doc) : doc(doc), current(nullptr) {}

XMLElement* Quad2XML::createElementWithKind(const char* elemName, QuadKind kind) {
    XMLElement* elem = doc->NewElement(elemName);
    
    // Add kind attribute based on the enum value
    switch (kind) {
        case QuadKind::PROGRAM: elem->SetAttribute("kind", "PROGRAM"); break;
        case QuadKind::FUNCDECL: elem->SetAttribute("kind", "FUNCDECL"); break;
        case QuadKind::BLOCK: elem->SetAttribute("kind", "BLOCK"); break;
        case QuadKind::MOVE: elem->SetAttribute("kind", "MOVE"); break;
        case QuadKind::LOAD: elem->SetAttribute("kind", "LOAD"); break;
        case QuadKind::STORE: elem->SetAttribute("kind", "STORE"); break;
        case QuadKind::MOVE_BINOP: elem->SetAttribute("kind", "MOVE_BINOP"); break;
        case QuadKind::CALL: elem->SetAttribute("kind", "CALL"); break;
        case QuadKind::MOVE_CALL: elem->SetAttribute("kind", "MOVE_CALL"); break;
        case QuadKind::EXTCALL: elem->SetAttribute("kind", "EXTCALL"); break;
        case QuadKind::MOVE_EXTCALL: elem->SetAttribute("kind", "MOVE_EXTCALL"); break;
        case QuadKind::LABEL: elem->SetAttribute("kind", "LABEL"); break;
        case QuadKind::JUMP: elem->SetAttribute("kind", "JUMP"); break;
        case QuadKind::CJUMP: elem->SetAttribute("kind", "CJUMP"); break;
        case QuadKind::PHI: elem->SetAttribute("kind", "PHI"); break;
        case QuadKind::RETURN: elem->SetAttribute("kind", "RETURN"); break;
    }
    
    return elem;
}

void Quad2XML::addTempAttribute(XMLElement* elem, const char* attrName, TempExp* temp) {
    if (temp) {
        elem->SetAttribute(attrName, temp->temp->name());
        elem->SetAttribute("temp_type", typeToString(temp->type).c_str());
    }
}

void Quad2XML::addLabelAttribute(XMLElement* elem, const char* attrName, Label* label) {
    if (label) {
        elem->SetAttribute(attrName, to_string(label->num).c_str());
    }
}

void Quad2XML::addQuadTermElement(XMLElement* parent, const char* elemName, QuadTerm* term) {
    if (!term) return;
    
    XMLElement* termElem = doc->NewElement(elemName);
    
    switch (term->kind) {
        case QuadTermKind::TEMP: {
            termElem->SetAttribute("kind", "TEMP");
            TempExp* temp = std::get<TempExp*>(term->term);
            if (temp) {
                termElem->SetAttribute("id", temp->temp->name());
                termElem->SetAttribute("type", tree::typeToString(temp->type).c_str());
            }
            break;
        }
        case QuadTermKind::CONST: {
            termElem->SetAttribute("kind", "CONST");
            int value = std::get<int>(term->term);
            termElem->SetAttribute("value", value);
            break;
        }
        case QuadTermKind::MAME: {
            termElem->SetAttribute("kind", "NAME");
            const string& name = std::get<string>(term->term);
            termElem->SetAttribute("name", name.c_str());
            break;
        }
    }
    
    parent->InsertEndChild(termElem);
}

void Quad2XML::processDefUse(XMLElement* elem, set<Temp*>* def, set<Temp*>* use) {
    // Add defined temps
    if (def && !def->empty()) {
        XMLElement* defElem = doc->NewElement("def");
        for (auto temp : *def) {
            XMLElement* tempElem = doc->NewElement("temp");
            tempElem->SetAttribute("id", temp->name());
            defElem->InsertEndChild(tempElem);
        }
        elem->InsertEndChild(defElem);
    }
    
    // Add used temps
    if (use && !use->empty()) {
        XMLElement* useElem = doc->NewElement("use");
        for (auto temp : *use) {
            XMLElement* tempElem = doc->NewElement("temp");
            tempElem->SetAttribute("id", temp->name());
            useElem->InsertEndChild(tempElem);
        }
        elem->InsertEndChild(useElem);
    }
}

void Quad2XML::visit(QuadProgram* program) {
    XMLElement* programElem = createElementWithKind("program", QuadKind::PROGRAM);
    doc->InsertEndChild(programElem);
    
    // Remember the current element so we can restore it later
    XMLElement* oldCurrent = current;
    current = programElem;
    
    // Process function declarations
    if (program->quadFuncDeclList) {
        for (auto func : *program->quadFuncDeclList) {
            if (func) func->accept(*this);
        }
    }
    
    current = oldCurrent;
}

void Quad2XML::visit(QuadFuncDecl* funcdecl) {
    XMLElement* funcElem = createElementWithKind("funcdecl", QuadKind::FUNCDECL);
    funcElem->SetAttribute("name", funcdecl->funcname.c_str());
    
    // Add last_label_num and last_temp_num attributes
    funcElem->SetAttribute("last_label_num", funcdecl->last_label_num);
    funcElem->SetAttribute("last_temp_num", funcdecl->last_temp_num);
    
    // Add parameters
    if (funcdecl->params && !funcdecl->params->empty()) {
        XMLElement* paramsElem = doc->NewElement("params");
        for (auto param : *funcdecl->params) {
            if (param) {
                XMLElement* paramElem = doc->NewElement("param");
                paramElem->SetAttribute("temp", to_string(param->num).c_str());
                paramsElem->InsertEndChild(paramElem);
            }
        }
        funcElem->InsertEndChild(paramsElem);
    }
    
    current->InsertEndChild(funcElem);
    
    // Remember the current element
    XMLElement* oldCurrent = current;
    current = funcElem;
    
    // Process blocks
    if (funcdecl->quadblocklist) {
        for (auto block : *funcdecl->quadblocklist) {
            if (block) block->accept(*this);
        }
    }
    
    current = oldCurrent;
}

void Quad2XML::visit(QuadBlock* block) {
    XMLElement* blockElem = createElementWithKind("block", QuadKind::BLOCK);
    
    // Add entry label
    addLabelAttribute(blockElem, "entry", block->entry_label);
    
    // Add exit labels
    if (block->exit_labels && !block->exit_labels->empty()) {
        XMLElement* exitsElem = doc->NewElement("exits");
        for (auto label : *block->exit_labels) {
            if (label) {
                XMLElement* exitElem = doc->NewElement("exit");
                addLabelAttribute(exitElem, "label", label);
                exitsElem->InsertEndChild(exitElem);
            }
        }
        blockElem->InsertEndChild(exitsElem);
    }
    
    current->InsertEndChild(blockElem);
    
    // Remember the current element
    XMLElement* oldCurrent = current;
    current = blockElem;
    
    // Process statements
    if (block->quadlist) {
        for (auto stmt : *block->quadlist) {
            if (stmt) stmt->accept(*this);
        }
    }
    
    current = oldCurrent;
}

void Quad2XML::visit(QuadMove* move) {
    XMLElement* moveElem = createElementWithKind("move", QuadKind::MOVE);
    
    // Add destination
    addTempAttribute(moveElem, "dst", move->dst);
    
    // Add source term
    if (move->src) {
        addQuadTermElement(moveElem, "src", move->src);
    }
    
    // Add def/use information
    processDefUse(moveElem, move->def, move->use);
    
    current->InsertEndChild(moveElem);
}

void Quad2XML::visit(QuadLoad* load) {
    XMLElement* loadElem = createElementWithKind("load", QuadKind::LOAD);
    
    // Add destination
    addTempAttribute(loadElem, "dst", load->dst);
    
    // Add source term (memory address)
    if (load->src) {
        addQuadTermElement(loadElem, "src", load->src);
    }
    
    // Add def/use information
    processDefUse(loadElem, load->def, load->use);
    
    current->InsertEndChild(loadElem);
}

void Quad2XML::visit(QuadStore* store) {
    XMLElement* storeElem = createElementWithKind("store", QuadKind::STORE);
    
    // Add source term (value being stored)
    if (store->src) {
        addQuadTermElement(storeElem, "src", store->src);
    }
    
    // Add destination term (memory address)
    if (store->dst) {
        addQuadTermElement(storeElem, "dst", store->dst);
    }
    
    // Add def/use information
    processDefUse(storeElem, store->def, store->use);
    
    current->InsertEndChild(storeElem);
}

void Quad2XML::visit(QuadMoveBinop* movebinop) {
    XMLElement* binopElem = createElementWithKind("movebinop", QuadKind::MOVE_BINOP);
    
    // Add destination
    addTempAttribute(binopElem, "dst", movebinop->dst);
    
    // Add binop operator
    binopElem->SetAttribute("op", movebinop->binop.c_str());
    
    // Add left and right operands
    if (movebinop->left) {
        addQuadTermElement(binopElem, "left", movebinop->left);
    }
    
    if (movebinop->right) {
        addQuadTermElement(binopElem, "right", movebinop->right);
    }
    
    // Add def/use information
    processDefUse(binopElem, movebinop->def, movebinop->use);
    
    current->InsertEndChild(binopElem);
}

void Quad2XML::visit(QuadCall* call) {
    XMLElement* callElem = createElementWithKind("call", QuadKind::CALL);
    
    // Add function name
    callElem->SetAttribute("name", call->name.c_str());
    
    // Add object term for method calls
    if (call->obj_term) {
        addQuadTermElement(callElem, "object", call->obj_term);
    }
    
    // Add arguments
    if (call->args && !call->args->empty()) {
        XMLElement* argsElem = doc->NewElement("args");
        int argIndex = 0;
        for (auto arg : *call->args) {
            if (arg) {
                XMLElement* argElem = doc->NewElement("arg");
                argElem->SetAttribute("index", argIndex++);
                addQuadTermElement(argElem, "value", arg);
                argsElem->InsertEndChild(argElem);
            }
        }
        callElem->InsertEndChild(argsElem);
    }
    
    // Add def/use information
    processDefUse(callElem, call->def, call->use);
    
    current->InsertEndChild(callElem);
}

void Quad2XML::visit(QuadMoveCall* movecall) {
    XMLElement* moveCallElem = createElementWithKind("movecall", QuadKind::MOVE_CALL);
    
    // Add destination
    addTempAttribute(moveCallElem, "dst", movecall->dst);
    
    // Remember the current element
    XMLElement* oldCurrent = current;
    current = moveCallElem;
    
    // Process the call
    if (movecall->call) {
        movecall->call->accept(*this);
    }
    
    // Add def/use information
    processDefUse(moveCallElem, movecall->def, movecall->use);
    
    current = oldCurrent;
    current->InsertEndChild(moveCallElem);
}

void Quad2XML::visit(QuadExtCall* extcall) {
    XMLElement* extCallElem = createElementWithKind("extcall", QuadKind::EXTCALL);
    
    // Add external function name
    extCallElem->SetAttribute("name", extcall->extfun.c_str());
    
    // Add arguments
    if (extcall->args && !extcall->args->empty()) {
        XMLElement* argsElem = doc->NewElement("args");
        int argIndex = 0;
        for (auto arg : *extcall->args) {
            if (arg) {
                XMLElement* argElem = doc->NewElement("arg");
                argElem->SetAttribute("index", argIndex++);
                addQuadTermElement(argElem, "value", arg);
                argsElem->InsertEndChild(argElem);
            }
        }
        extCallElem->InsertEndChild(argsElem);
    }
    
    // Add def/use information
    processDefUse(extCallElem, extcall->def, extcall->use);
    
    current->InsertEndChild(extCallElem);
}

void Quad2XML::visit(QuadMoveExtCall* moveextcall) {
    XMLElement* moveExtCallElem = createElementWithKind("moveextcall", QuadKind::MOVE_EXTCALL);
    
    // Add destination
    addTempAttribute(moveExtCallElem, "dst", moveextcall->dst);
    
    // Remember the current element
    XMLElement* oldCurrent = current;
    current = moveExtCallElem;
    
    // Process the extcall
    if (moveextcall->extcall) {
        moveextcall->extcall->accept(*this);
    }
    
    // Add def/use information
    processDefUse(moveExtCallElem, moveextcall->def, moveextcall->use);
    
    current = oldCurrent;
    current->InsertEndChild(moveExtCallElem);
}

void Quad2XML::visit(QuadLabel* label) {
    XMLElement* labelElem = createElementWithKind("label", QuadKind::LABEL);
    
    // Add label
    addLabelAttribute(labelElem, "name", label->label);
    
    // Add def/use information
    processDefUse(labelElem, label->def, label->use);
    
    current->InsertEndChild(labelElem);
}

void Quad2XML::visit(QuadJump* jump) {
    XMLElement* jumpElem = createElementWithKind("jump", QuadKind::JUMP);
    
    // Add target label
    addLabelAttribute(jumpElem, "target", jump->label);
    
    // Add def/use information
    processDefUse(jumpElem, jump->def, jump->use);
    
    current->InsertEndChild(jumpElem);
}

void Quad2XML::visit(QuadCJump* cjump) {
    XMLElement* cjumpElem = createElementWithKind("cjump", QuadKind::CJUMP);
    
    // Add relational operator
    cjumpElem->SetAttribute("relop", cjump->relop.c_str());
    
    // Add left and right operands
    if (cjump->left) {
        addQuadTermElement(cjumpElem, "left", cjump->left);
    }
    
    if (cjump->right) {
        addQuadTermElement(cjumpElem, "right", cjump->right);
    }
    
    // Add true and false labels
    addLabelAttribute(cjumpElem, "true", cjump->t);
    addLabelAttribute(cjumpElem, "false", cjump->f);
    
    // Add def/use information
    processDefUse(cjumpElem, cjump->def, cjump->use);
    
    current->InsertEndChild(cjumpElem);
}

void Quad2XML::visit(QuadPhi* phi) {
    XMLElement* phiElem = createElementWithKind("phi", QuadKind::PHI);
    
    // Add destination temp
    addTempAttribute(phiElem, "dst", phi->temp);
    
    // Add arguments (pairs of temp and label)
    if (phi->args && !phi->args->empty()) {
        XMLElement* argsElem = doc->NewElement("args");
        for (auto& arg : *phi->args) {
            XMLElement* argElem = doc->NewElement("arg");
            if (arg.first) {
                argElem->SetAttribute("temp", arg.first->name());
            }
            if (arg.second) {
                argElem->SetAttribute("label", to_string(arg.second->num).c_str());
            }
            argsElem->InsertEndChild(argElem);
        }
        phiElem->InsertEndChild(argsElem);
    }
    
    // Add def/use information
    processDefUse(phiElem, phi->def, phi->use);
    
    current->InsertEndChild(phiElem);
}

void Quad2XML::visit(QuadReturn* returnQuad) {
    XMLElement* returnElem = createElementWithKind("return", QuadKind::RETURN);
    
    // Add return value
    if (returnQuad->value) {
        addQuadTermElement(returnElem, "value", returnQuad->value);
    }
    
    // Add def/use information
    processDefUse(returnElem, returnQuad->def, returnQuad->use);
    
    current->InsertEndChild(returnElem);
}