#define DEBUG
#undef DEBUG

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <set>
#include "quad.hh"
#include "temp.hh"

using namespace std;
using namespace quad;

string quad::quadKindToString(QuadKind kind) {
    switch (kind) {
        case QuadKind::MOVE: return "MOVE";
        case QuadKind::LOAD: return "LOAD";
        case QuadKind::STORE: return "STORE";
        case QuadKind::MOVE_BINOP: return "MOVE_BINOP";
        case QuadKind::CALL: return "CALL";
        case QuadKind::EXTCALL: return "EXTCALL";
        case QuadKind::MOVE_CALL: return "MOVE_CALL";
        case QuadKind::MOVE_EXTCALL: return "MOVE_EXTCALL";
        case QuadKind::LABEL: return "LABEL";
        case QuadKind::JUMP: return "JUMP";
        case QuadKind::CJUMP: return "CJUMP";
        case QuadKind::PHI: return "PHI";
        case QuadKind::RETURN: return "RETURN";
        default: return "UNKNOWN";
    }
}

std::string QuadTerm::print() {
#ifdef DEBUG
    cout << "In QuadTerm::print" << endl;
#endif
    TempExp *t_tempexp;
    switch (kind) {
        case QuadTermKind::TEMP:
            t_tempexp = get<TempExp*>(term);
            return "t" + to_string(t_tempexp->temp->num) + ":" + (t_tempexp->type == Type::INT?"INT":"PTR"); 
        case QuadTermKind::CONST:
            return "Const:"+to_string(this->get_const());
        case QuadTermKind::MAME:
            return "Name:"+get<std::string>(term);
    }
    return "";
}
TempExp* QuadTerm::get_temp() {
    if (kind == QuadTermKind::TEMP) {
        return get<TempExp*>(term);
    }
    return nullptr;
}

int QuadTerm::get_const() {
    if (kind == QuadTermKind::CONST) {
        return get<int>(term);
    }
    return 0;
}

string QuadTerm::get_name() {
    if (kind == QuadTermKind::MAME) {
        return get<std::string>(term);
    }
    return nullptr;
}

QuadTerm* QuadTerm::clone() const {
    switch (kind) {
        case QuadTermKind::TEMP: {
            TempExp* temp = std::get<TempExp*>(term);
            if (temp) {
                return new QuadTerm(new TempExp(temp->type, new Temp(temp->temp->num)));
            }
            return nullptr;
        }
        case QuadTermKind::CONST:
            return new QuadTerm(std::get<int>(term));
        case QuadTermKind::MAME:
            return new QuadTerm(std::get<string>(term));
        default:
            return nullptr;
    }
}

static string print_indent(int indent) {
#ifdef DEBUG
    cout << "In print_indent" << endl;
#endif
    std::string output_str; output_str.reserve(20);
    for (int i = 0; i < indent; i++) {
        output_str += " ";
    }
    return output_str;
}

string print_def_use(set<Temp*> *def, set<Temp*> *use) {
#ifdef DEBUG
    cout << "In print_def_use" << endl;
#endif
    std::string output_str; output_str.reserve(200);
    output_str += "def: ";
    if (def != nullptr) {
        for (auto t : *def) {
            output_str += to_string(t->name());
            output_str += " ";
        }
    }
    output_str += "use: ";
    if (use != nullptr) {
        for (auto t : *use) {
            output_str += to_string(t->name());
            output_str += " ";
        }
    }
    return output_str;
}

static string print_temp(TempExp *t) {
#ifdef DEBUG
    cout << "In print_temp" << endl;
#endif
    std::string output_str; output_str.reserve(20);
    if (t != nullptr) {
        output_str += "t";
        output_str += to_string(t->temp->num);
        output_str += ":";
        output_str += (t->type == Type::INT?"INT":"PTR");
    }
    return output_str;
}

static std::string print_label(Label *l) {
#ifdef DEBUG
    cout << "In print_label" << endl;
#endif
    std::string str; str.reserve(20);
    if (l != nullptr) {
        str = "L" + to_string(l->num);
    }
    return str;
}

static std::string print_call(QuadCall *call) {
#ifdef DEBUG
    cout << "In print_call" << endl;
#endif
    std::string use_str; use_str.reserve(100);
    std::string id = call->name;
    QuadTerm *obj_term = call->obj_term;
    vector<QuadTerm*> *args = call->args;
    use_str += call->name;
    use_str += "[";
    if (obj_term)
        use_str +=  obj_term->print();
    use_str += "] (";
    bool first = true;
    if (args)
        for (auto arg : *args) {
            use_str += (first?"":", ");
            use_str += arg->print();
            first = false;
        }
    use_str += "); " ;
    return use_str;
}

string print_extcall(QuadExtCall *call) {
#ifdef DEBUG
    cout << "In print_extcall" << endl;
#endif
    string use_str; use_str.reserve(100);
    vector<QuadTerm*> *args = call->args;
    use_str += call->extfun;
    use_str += "(";
    bool first = true;
    for (auto arg : *args) {
        use_str += (first?"":", ");
        use_str += arg->print();
        first = false;
    }
    use_str += "); " ;
    return use_str;
}

set<Temp*>* QuadStm::cloneTemps(const set<Temp*>* temps) const {
    if (!temps) return nullptr;
    
    set<Temp*>* newTemps = new set<Temp*>();
    for (auto temp : *temps) {
        if (temp) {
            newTemps->insert(new Temp(temp->num));
        }
    }
    return newTemps;
}

void QuadProgram::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadProgram::print: " <<  (this->kind == QuadKind::PROGRAM?"Program!":"Oh?") << endl;
#endif
    if (quadFuncDeclList == nullptr) {
        return ;
    }

    for (auto func : *quadFuncDeclList) {
        if (func == nullptr) {
            cerr << "Error: QuadFuncDecl is null!" << endl;
            continue;
        }
        func->print(use_str, indent, to_print_def_use);
    }
    return ;
}

QuadProgram* QuadProgram::clone() const {
    vector<QuadFuncDecl*>* newFuncList = new vector<QuadFuncDecl*>();
    if (quadFuncDeclList) {
        for (auto funcDecl : *quadFuncDeclList) {
            if (funcDecl) {
                newFuncList->push_back(static_cast<QuadFuncDecl*>(funcDecl->clone()));
            }
        }
    }
    return new QuadProgram(static_cast<tree::Program*>(node), newFuncList);
}

void QuadFuncDecl::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadFuncDecl::print" << endl;
#endif
    if (this->quadblocklist == nullptr) {
        return;
    }
    use_str += "Function "; use_str += this->funcname;
    if (params != nullptr) {
        use_str += "(";
        bool first = true;
        for (auto param : *this->params) {
            if (!first) {
                use_str += ", ";
            }
            use_str += "t"+to_string(param->num);
            first = false;
        }
        use_str += ")";
    }
    use_str += " last_label=";
    use_str += to_string(this->last_label_num);
    use_str += " last_temp=";
    use_str += to_string(this->last_temp_num);
    use_str += ":\n";
    for (auto block : *this->quadblocklist) {
        block->print(use_str, indent+2, to_print_def_use);
    }
    return ;
}

QuadFuncDecl* QuadFuncDecl::clone() const {
    vector<Temp*>* newParams = new vector<Temp*>();
    if (params) {
        for (auto param : *params) {
            if (param) {
                newParams->push_back(new Temp(param->num));
            }
        }
    }
    
    vector<QuadBlock*>* newBlockList = new vector<QuadBlock*>();
    if (quadblocklist) {
        for (auto block : *quadblocklist) {
            if (block) {
                newBlockList->push_back(static_cast<QuadBlock*>(block->clone()));
            }
        }
    }
    
    return new QuadFuncDecl(node, funcname, newParams, newBlockList, last_label_num, last_temp_num);
}

void QuadBlock::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadBlock::print" << endl;
#endif
    use_str += print_indent(indent);
    use_str += "Block: Entry Label: ";
    use_str += print_label(this->entry_label);
    use_str +=+ "\n";
    if (this->exit_labels != nullptr) {
        use_str += print_indent(indent+2);
        use_str += "Exit labels: ";
        for (auto label : *this->exit_labels) {
            use_str += print_label(label);
            use_str += " ";
        }
        use_str += "\n";
    }
    for (auto quad : *this->quadlist) {
        quad->print(use_str, indent+2, to_print_def_use);
    }
    return ;
}

QuadBlock* QuadBlock::clone() const {
    Label* newEntryLabel = entry_label ? new Label(entry_label->num) : nullptr;
    
    vector<tree::Label*>* newExitLabels = new vector<tree::Label*>();
    if (exit_labels) {
        for (auto label : *exit_labels) {
            if (label) {
                newExitLabels->push_back(new Label(label->num));
            }
        }
    }
    
    vector<QuadStm*>* newQuadList = new vector<QuadStm*>();
    if (quadlist) {
        for (auto stmt : *quadlist) {
            if (stmt) {
                newQuadList->push_back(static_cast<QuadStm*>(stmt->clone()));
            }
        }
    }
    
    return new QuadBlock(node, newQuadList, newEntryLabel, newExitLabels);
}

void QuadMove::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadMove::print" << endl;
#endif
    TempExp *dst_temp = this->dst;
    QuadTerm *src_term = this->src;
    use_str += print_indent(indent);
    use_str += "MOVE ";
    use_str += print_temp(dst_temp);
    use_str += " <- ";
    use_str += src_term->print();
    use_str += "; ";
    use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
    use_str += "\n";
    return ;
}

QuadMove* QuadMove::clone() const {
    TempExp* newDst = dst ? new TempExp(dst->type, new Temp(dst->temp->num)) : nullptr;
    QuadTerm* newSrc = src ? src->clone() : nullptr;
    return new QuadMove(node, newDst, newSrc, cloneTemps(def), cloneTemps(use));
}

void QuadLoad::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadLoad::print" << endl;
#endif
    TempExp *dst_temp = this->dst;
    QuadTerm *src_term = this->src;
    use_str += print_indent(indent);
    use_str += "LOAD ";
    use_str +=  print_temp(dst_temp);
    use_str += " <- Mem(";
    use_str += src_term->print() + "); ";
    use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
    use_str += "\n";
    return;
}

QuadLoad* QuadLoad::clone() const {
    TempExp* newDst = dst ? new TempExp(dst->type, new Temp(dst->temp->num)) : nullptr;
    QuadTerm* newSrc = src ? src->clone() : nullptr;
    return new QuadLoad(node, newDst, newSrc, cloneTemps(def), cloneTemps(use));
}

//store is term->mem(term)
void QuadStore::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadStore::print" << endl;
#endif
    QuadTerm *src_term = this->src;
    QuadTerm *dst_term = this->dst;
    use_str += print_indent(indent);
    use_str += "STORE ";
    use_str += src_term->print();
    use_str += " -> ";
    use_str += "Mem(";
    use_str += dst_term->print();
    use_str += "); ";
    use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
    use_str += "\n";
    return;
}

QuadStore* QuadStore::clone() const {
    QuadTerm* newSrc = src ? src->clone() : nullptr;
    QuadTerm* newDst = dst ? dst->clone() : nullptr;
    return new QuadStore(node, newSrc, newDst, cloneTemps(def), cloneTemps(use));
}
        
void QuadMoveBinop::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadMoveBinop::print" << endl;
#endif
    TempExp *dst_temp = this->dst;
    string binop = this->binop;
    QuadTerm *left_term = this->left;
    QuadTerm *right_term = this->right;
    use_str += print_indent(indent);
    use_str += "MOVE_BINOP ";
    use_str += print_temp(dst_temp);
    use_str += " <- ";
    use_str += "(";
    use_str += binop;
    use_str += ", ";
    use_str += left_term->print();
    use_str += ", ";
    use_str += right_term->print();
    use_str += "); ";
    use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
    use_str += "\n";
    return ;
}

QuadMoveBinop* QuadMoveBinop::clone() const {
    TempExp* newDst = dst ? new TempExp(dst->type, new Temp(dst->temp->num)) : nullptr;
    QuadTerm* newLeft = left ? left->clone() : nullptr;
    QuadTerm* newRight = right ? right->clone() : nullptr;
    return new QuadMoveBinop(node, newDst, newLeft, binop, newRight, cloneTemps(def), cloneTemps(use));
}

void QuadCall::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadCall::print" << endl;
#endif
        string call_string = print_call(this);
        use_str += print_indent(indent);
        use_str += "CALL ";
        use_str += call_string;
        use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
        use_str += "\n";
        return;
}

QuadCall* QuadCall::clone() const {
    QuadTerm* newObjTerm = obj_term ? obj_term->clone() : nullptr;
    
    vector<QuadTerm*>* newArgs = new vector<QuadTerm*>();
    if (args) {
        for (auto arg : *args) {
            if (arg) {
                newArgs->push_back(arg->clone());
            }
        }
    }
    
    return new QuadCall(node, name, newObjTerm, newArgs, cloneTemps(def), cloneTemps(use));
}

void QuadMoveCall::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadMoveCall::print" << endl;
#endif
    TempExp *dst_temp = this->dst;
    QuadCall *call = this->call;
    use_str += print_indent(indent);
    use_str += "MOVE_CALL ";
    use_str += print_temp(dst_temp);
    use_str += " <- ";
    use_str += print_call(call);
    use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
    use_str += "\n";
    return;
}

QuadMoveCall* QuadMoveCall::clone() const {
    TempExp* newDst = dst ? new TempExp(dst->type, new Temp(dst->temp->num)) : nullptr;
    QuadCall* newCall = call ? static_cast<QuadCall*>(call->clone()) : nullptr;
    return new QuadMoveCall(node, newDst, newCall, cloneTemps(def), cloneTemps(use));
}

void QuadExtCall::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadExtCall::print" << endl;
#endif
    string extcall_str = print_extcall(this);
    use_str += print_indent(indent);
    use_str += "EXTCALL ";
    use_str += extcall_str;
    use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
    use_str += "\n";
    return;
}

QuadExtCall* QuadExtCall::clone() const {
    vector<QuadTerm*>* newArgs = new vector<QuadTerm*>();
    if (args) {
        for (auto arg : *args) {
            if (arg) {
                newArgs->push_back(arg->clone());
            }
        }
    }
    
    return new QuadExtCall(node, extfun, newArgs, cloneTemps(def), cloneTemps(use));
}
        
void QuadMoveExtCall::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadMoveExtCall::print" << endl;
#endif
    TempExp *dst_temp = this->dst;
    QuadExtCall *extcall = this->extcall;
    use_str += print_indent(indent);
    use_str += "MOVE_EXTCALL ";
    use_str += print_temp(dst_temp);
    use_str += " <- ";
    use_str += print_extcall(extcall);
    use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
    use_str += "\n";
    return;
}

QuadMoveExtCall* QuadMoveExtCall::clone() const {
    TempExp* newDst = dst ? new TempExp(dst->type, new Temp(dst->temp->num)) : nullptr;
    QuadExtCall* newExtCall = extcall ? static_cast<QuadExtCall*>(extcall->clone()) : nullptr;
    return new QuadMoveExtCall(node, newDst, newExtCall, cloneTemps(def), cloneTemps(use));
}

void QuadLabel::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadLabel::print" << endl;
#endif
    use_str += print_indent(indent);
    use_str += "LABEL ";
    use_str += print_label(this->label);
    use_str += "; ";
    use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
    use_str += "\n";
    return;
}

QuadLabel* QuadLabel::clone() const {
    Label* newLabel = label ? new Label(label->num) : nullptr;
    return new QuadLabel(node, newLabel, cloneTemps(def), cloneTemps(use));
}

void QuadJump::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadJump::print" << endl;
#endif
    use_str += print_indent(indent);
    use_str += "JUMP ";
    use_str += print_label(this->label);
    use_str += "; ";
    use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
    use_str += "\n";
    return;
}

QuadJump* QuadJump::clone() const {
    Label* newLabel = label ? new Label(label->num) : nullptr;
    return new QuadJump(node, newLabel, cloneTemps(def), cloneTemps(use));
}

void QuadCJump::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadCJump::print" << endl;
#endif
    Label *true_label = this->t;
    Label *false_label = this->f;
    QuadTerm *left_term = this->left;
    QuadTerm *right_term = this->right;
    use_str += print_indent(indent);
    use_str += "CJUMP ";
    use_str += this->relop;
    use_str += " ";
    use_str += left_term->print();
    use_str += " ";
    use_str += right_term->print();
    use_str += "? ";
    use_str += print_label(true_label);
    use_str += " : ";
    use_str += print_label(false_label);
    use_str += "; ";
    use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
    use_str += "\n";
    return;
}

QuadCJump* QuadCJump::clone() const {
    QuadTerm* newLeft = left ? left->clone() : nullptr;
    QuadTerm* newRight = right ? right->clone() : nullptr;
    Label* newTrue = t ? new Label(t->num) : nullptr;
    Label* newFalse = f ? new Label(f->num) : nullptr;
    
    return new QuadCJump(node, relop, newLeft, newRight, newTrue, newFalse, cloneTemps(def), cloneTemps(use));
}

void QuadPhi::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadPhi::print" << endl;
#endif
    TempExp *phi_temp = this->temp;
    use_str += print_indent(indent);
    use_str += "PHI ";
    use_str += print_temp(phi_temp);
    use_str += " <- (";
    bool first = true;
    if (this->args == nullptr) {
        use_str += "NULL";
        use_str += "); ";
        use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
        use_str += "\n";
        return ;
    }
    for (int i=0; i< this->args->size(); i++) {
        Temp *arg_temp = this->args->at(i).first;
        Label *arg_label = this->args->at(i).second;
        use_str += (first?"":"; ");
        use_str += "t";
        use_str += to_string(arg_temp->num);
        use_str +=  ", ";
        use_str +=  print_label(arg_label);
        first = false;
    }
    use_str += "); ";
    use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
    use_str += "\n";
    return ;
}

QuadPhi* QuadPhi::clone() const {
    TempExp* newTemp = temp ? new TempExp(temp->type, new Temp(temp->temp->num)) : nullptr;
    
    vector<pair<Temp*, Label*>>* newArgs = new vector<pair<Temp*, Label*>>();
    if (args) {
        for (auto& arg : *args) {
            Temp* newArgTemp = arg.first ? new Temp(arg.first->num) : nullptr;
            Label* newArgLabel = arg.second ? new Label(arg.second->num) : nullptr;
            newArgs->push_back(make_pair(newArgTemp, newArgLabel));
        }
    }
    
    return new QuadPhi(node, newTemp, newArgs, cloneTemps(def), cloneTemps(use));
}

void QuadReturn::print(string &use_str, int indent, bool to_print_def_use) {
#ifdef DEBUG
    cout << "In QuadReturn::print" << endl;  
#endif
    QuadTerm *ret_term = this->value;
    use_str += print_indent(indent);
    use_str += "RETURN ";
    use_str += ret_term->print();
    use_str += "; ";
    use_str += (to_print_def_use? print_def_use(this->def, this->use) : "");
    use_str += "\n";
    return ;
}

QuadReturn* QuadReturn::clone() const {
    QuadTerm* newValue = value ? value->clone() : nullptr;
    return new QuadReturn(node, newValue, cloneTemps(def), cloneTemps(use));
}

void quad2file(Quad *quad, string filename, bool print_def_use) {
#ifdef DEBUG
    cout << "In quad2file, writing to: " << filename << endl;
#endif
    if (quad == nullptr) {
        cerr << "Error: Quad is null!" << endl;
        return;
    }
    std::string output_str; output_str.reserve(10000);
    quad->print(output_str, 0, print_def_use);
    std::ofstream out(filename);
    if (out.is_open()) {
        out<< output_str;
        out.close();
    } else {
        cerr << "Error: Unable to open file " << filename << endl;
    }
    return;
}