#define DEBUG
// #undef DEBUG

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "treep.hh"
#include "quad.hh"
#include "tree2quad.hh"

using namespace std;
using namespace tree;
using namespace quad;

#ifdef DEBUG
#define DEBUG_PRINT(x) cout << x << endl
#endif
/*
We use an instruction selection method (pattern matching) to convert the IR tree to Quad.
The Quad is a simplified tree node/substructure, each Quad is a tree pattern:
Move:  temp <- temp
Load:  temp <- mem(temp)
Store: mem(temp) <- temp
MoveBinop: temp <- temp op temp
Call:  ExpStm(call) //ignore the result
ExtCall: ExpStm(extcall) //ignore the result
MoveCall: temp <- call
MoveExtCall: temp <- extcall
Label: label
Jump: jump label
CJump: cjump relop temp, temp, label, label
Phi:  temp <- list of {temp, label} //same as the Phi in the tree
*/

QuadProgram* tree2quad(Program* prog) {
#ifdef DEBUG
    cout << "in Tree2Quad::Converting IR to Quad" << endl;
#endif
    Tree2Quad visitor;
    visitor.temp_map = new Temp_map();
    visitor.quadprog = nullptr;
    visitor.visit_result = new vector<QuadStm*>();
    visitor.output_term = nullptr;
    
    prog->accept(visitor);
    
    return visitor.quadprog;
}

void Tree2Quad::visit(Program* prog) {
#ifdef DEBUG
    cout << "Converting to Quad: Program" << endl;
#endif
    if (!prog || !prog->funcdecllist) {
        visit_result = nullptr;
        output_term = nullptr;
        return;
    }

    vector<QuadFuncDecl*> *funcList = new vector<QuadFuncDecl*>();
    
    for (FuncDecl* func : *prog->funcdecllist) {
        temp_map = new Temp_map(); 
        temp_map->next_label = func->last_label_num + 1;
        temp_map->next_temp = func->last_temp_num + 1;  
        func->accept(*this);
        if (quad_func_decl) {
            funcList->push_back(quad_func_decl);
        }
    }
    if(funcList == nullptr) {
        cerr << "Error: funcList is nullptr" << endl;
    }
    quadprog = new QuadProgram(prog, funcList);
    visit_result = nullptr;
    output_term = nullptr;
}

void Tree2Quad::visit(FuncDecl* node) {
#ifdef DEBUG
    cout << "Converting to Quad: FunctionDeclaration" << endl;
#endif
    if (!node) {
        visit_result = nullptr;
        return;
    }

    // Create new block for function body
    vector<QuadBlock*> *blocks = new vector<QuadBlock*>();
    // Handle function body
    for(auto block : *node->blocks) {
        block->accept(*this);
        QuadBlock* quad_block = new QuadBlock(block, visit_result, block->entry_label, block->exit_labels);
        blocks->push_back(quad_block);
    }

    // Create function declaration
    DEBUG_PRINT("func name"<< node->name);
    QuadFuncDecl* func_decl = new QuadFuncDecl(
        node, 
        node->name,
        node->args,
        blocks,
        temp_map->next_label - 1,
        temp_map->next_temp - 1
    );
    quad_func_decl = func_decl;
    visit_result = nullptr;
    output_term = nullptr;
    DEBUG_PRINT("finish QuadFuncDecl");
}

void Tree2Quad::visit(Block *block) {
#ifdef DEBUG
    cout << "Converting to Quad: Block" << endl;
#endif
    if (!block) {
        visit_result = nullptr;
        return;
    }

    vector<QuadStm*> *result = new vector<QuadStm*>();
    
    // Process each statement in the block
    for (auto stm : *block->sl) {
        stm->accept(*this);
        if (visit_result) {
            result->insert(result->end(), visit_result->begin(), visit_result->end());
            visit_result = nullptr;
        }
    }

    visit_result = result;
    output_term = nullptr;
}

void Tree2Quad::visit(Jump* node) {
#ifdef DEBUG
    cout << "Converting to Quad: Jump" << endl;
#endif
    if (!node || !node->label) {
        visit_result = nullptr;
        return;
    }
    
    QuadJump* jump = new QuadJump(node, node->label, nullptr, nullptr);
    visit_result = new vector<QuadStm*>{jump};
    output_term = nullptr;
}

void Tree2Quad::visit(tree::Cjump* node) {
#ifdef DEBUG
    cout << "Converting to Quad: CJump" << endl;
#endif
    if (!node || !node->left || !node->right) {
        visit_result = nullptr;
        return;
    }
    vector<QuadStm*>* result = new vector<QuadStm*>();
    node->left->accept(*this);
    QuadTerm* left_term = output_term;
    if(visit_result){
        result->insert(result->end(), visit_result->begin(), visit_result->end());
        visit_result = nullptr;
    }
    node->right->accept(*this);
    if(visit_result){
        result->insert(result->end(), visit_result->begin(), visit_result->end());
        visit_result = nullptr;
    }
    QuadTerm* right_term = output_term;
    set<Temp*>* def = new set<Temp*>();
    if(auto temp = left_term->get_temp()) {
        def->insert(temp->temp);
    }
    if(auto temp = right_term->get_temp()) {
        def->insert(temp->temp);
    }

    QuadCJump* cjump = new QuadCJump(node, node->relop, left_term, right_term, 
                                    node->t, node->f, nullptr, def);
    result->push_back(cjump);
    visit_result = result;
    output_term = nullptr;
}

void Tree2Quad::visit(tree::Move *move) {
    DEBUG_PRINT("Converting to Quad: Move");

    Tree *dst = move->dst;
    Tree *src = move->src;
    vector<QuadStm*> *result;
    if (dst->getTreeKind() == Kind::MEM) {
        result = new vector<QuadStm*>();
        tree::Mem *mem = static_cast<tree::Mem*>(dst);
        DEBUG_PRINT("Mem <- Temp/Const/Name/Mem");
        mem->mem->accept(*this);
        QuadTerm *dst_term = output_term;
        if(visit_result){
            result->insert(result->end(), visit_result->begin(), visit_result->end());
            visit_result = nullptr;
        }
        src->accept(*this);
        QuadTerm *src_term = output_term;
        if(visit_result){
            result->insert(result->end(), visit_result->begin(), visit_result->end());
            visit_result = nullptr;
        }
        
        auto def = new set<Temp*>();
        auto use = new set<Temp*>();

        if (src_term->kind == QuadTermKind::TEMP)
            use->insert(src_term->get_temp()->temp);
        if (dst_term->kind == QuadTermKind::TEMP)
            use->insert(dst_term->get_temp()->temp);
        result->push_back(new QuadStore(move, src_term, dst_term, def, use));
        visit_result = result;
        return;
    }
    if(dynamic_cast<TempExp*>(dst) == nullptr) {
        DEBUG_PRINT("dst is not TempExp");
        visit_result = nullptr;
        return;
    }
    TempExp *dst_temp = static_cast<TempExp*>(dst);
    

    if (src->getTreeKind() == Kind::CALL) {
        src->accept(*this);
        QuadCall *call = static_cast<QuadCall*>(visit_result->back());
        visit_result->pop_back();
        auto def = new set<Temp*>();
        auto use = new set<Temp*>();
        def->insert(dst_temp->temp);
        if(call->obj_term->get_temp()) {
            use->insert(call->obj_term->get_temp()->temp);
        }
        for(auto arg : *(call->args)) {
            if(arg->get_temp()) {
                use->insert(arg->get_temp()->temp);
            }
        }
        visit_result->push_back(new QuadMoveCall(move, dst_temp, call, def, use));
    }
    else if (src->getTreeKind() == Kind::EXTCALL) {
        DEBUG_PRINT("Temp <- ExtCall");
        src->accept(*this);
        QuadExtCall *extcall = static_cast<QuadExtCall*>(visit_result->back());
        visit_result->pop_back();
        
        auto def = new set<Temp*>();
        auto use = new set<Temp*>();
        def->insert(dst_temp->temp);
        for(auto arg : *(extcall->args)) {
            if(arg->get_temp()) {
                use->insert(arg->get_temp()->temp);
            }
        }
        visit_result = new vector<QuadStm*>();
        visit_result->push_back(new QuadMoveExtCall(move, dst_temp, extcall, def, use));
        DEBUG_PRINT("finish Temp <- ExtCall");
    }
    else if (src->getTreeKind() == Kind::MEM) {
        result = new vector<QuadStm*>();
        DEBUG_PRINT("Temp <- Mem");
        Mem* mem = static_cast<Mem*>(src);
        mem->mem->accept(*this);
        if(visit_result){
            result->insert(result->end(), visit_result->begin(), visit_result->end());
            visit_result = nullptr;
        }
        QuadTerm *src_term = output_term;
        
        auto def = new set<Temp*>();
        auto use = new set<Temp*>();
        def->insert(dst_temp->temp);
        if (src_term->kind == QuadTermKind::TEMP)
            use->insert(src_term->get_temp()->temp);
        result->push_back(new QuadLoad(move, dst_temp, src_term, def, use));
        visit_result = result;
    }
    else if(src->getTreeKind() == Kind::BINOP){
        DEBUG_PRINT("Temp <- Binop");
        result = new vector<QuadStm*>();
        Binop* binop = static_cast<Binop*>(src);
        binop->left->accept(*this);
        QuadTerm* left = output_term;
        if(visit_result){
            result->insert(result->end(), visit_result->begin(), visit_result->end());
            visit_result = nullptr;
        }
        binop->right->accept(*this);
        QuadTerm* right = output_term;
        if(visit_result){
            result->insert(result->end(), visit_result->begin(), visit_result->end());
            visit_result = nullptr;
        }  
        set<Temp*>* def = new set<Temp*>();
        def->insert(dst_temp->temp);
        set<Temp*>* use = new set<Temp*>();
        
        if (auto t = left->get_temp()) {
            use->insert(t->temp);
        }
        if (auto t = right->get_temp()) {
            use->insert(t->temp);
        }
        result->push_back(new QuadMoveBinop(move, dst_temp, left, binop->op, right, def, use));
        visit_result = result;
    }
    else {
        DEBUG_PRINT("Temp <- Temp");
        src->accept(*this);
        QuadTerm *src_term = output_term;
        if(dst_temp == nullptr) {
            DEBUG_PRINT("dst_temp is nullptr");
        }
        auto def = new set<Temp*>();
        auto use = new set<Temp*>();
        def->insert(dst_temp->temp);
        if (src_term->kind == QuadTermKind::TEMP)
            use->insert(src_term->get_temp()->temp);
        visit_result = new vector<QuadStm*>();
        visit_result->push_back(new QuadMove(move, dst_temp, src_term, def, use));
    }
}


void Tree2Quad::visit(Seq* node) {
#ifdef DEBUG
    cout << "Converting to Quad: Sequence" << endl;
#endif
    if (!node || !node->sl) {
        visit_result = nullptr;
        return;
    }

    vector<QuadStm*>* result = new vector<QuadStm*>();
    for (auto stm : *(node->sl)) {
        stm->accept(*this);
        if (visit_result) {
            result->insert(result->end(), visit_result->begin(), visit_result->end());
        }
    }
    visit_result = result;
    output_term = nullptr;
}

void Tree2Quad::visit(LabelStm* node) {
#ifdef DEBUG
    cout << "Converting to Quad: Label" << endl;
#endif
    if (!node || !node->label) {
        visit_result = nullptr;
        return;
    }
    DEBUG_PRINT("Label name "<<node->label->name());
    QuadLabel* label = new QuadLabel(node, node->label, nullptr, nullptr);
    visit_result = new vector<QuadStm*>{label};
    output_term = nullptr;
}

void Tree2Quad::visit(Return* node) {
#ifdef DEBUG
    cout << "Converting to Quad: Return" << endl;
#endif
    vector<QuadStm*>* result = new vector<QuadStm*>();
    if (!node || !node->exp) {
        visit_result = nullptr;
        return;
    }

    node->exp->accept(*this);
    if(visit_result){
        result->insert(result->end(), visit_result->begin(), visit_result->end());
        visit_result = nullptr;
    }
    set<Temp*>* def = new set<Temp*>();
    if(auto temp = output_term->get_temp()) {
        def->insert(temp->temp);
    }
    QuadReturn* ret = new QuadReturn(node, output_term, nullptr, def);
    result->push_back(ret);
    visit_result = result;
    output_term = nullptr;
}

void Tree2Quad::visit(Phi* node) {
#ifdef DEBUG
    cout << "Converting to Quad: Phi" << endl;
#endif
    if (!node) {
        output_term = nullptr;
        return;
    }

    // QuadPhi* phi = new QuadPhi(node->temp, node->args);
    // visit_result = new vector<QuadStm*>{phi};
    // output_term = nullptr;
}

void Tree2Quad::visit(ExpStm* node) {
#ifdef DEBUG
    cout << "Converting to Quad: ExpressionStatement" << endl;
#endif
    if (!node || !node->exp) {
        visit_result = nullptr;
        return;
    }
    
    node->exp->accept(*this);
    if (!visit_result) {
        visit_result = new vector<QuadStm*>();
    }
    output_term = nullptr;
}

void Tree2Quad::visit(Binop* node) {
#ifdef DEBUG
    cout << "Converting to Quad: BinaryOperation" << endl;
#endif
    if (!node || !node->left || !node->right) {
        output_term = nullptr;
        return;
    }
    vector<QuadStm*>* result = new vector<QuadStm*>();
    node->left->accept(*this);
    QuadTerm* left = output_term;
    if(visit_result){
        DEBUG_PRINT("left has result");
        result->insert(result->end(), visit_result->begin(), visit_result->end());
        visit_result = nullptr;
    }
    node->right->accept(*this);
    QuadTerm* right = output_term;
    if(visit_result){
        DEBUG_PRINT("right has result");
        result->insert(result->end(), visit_result->begin(), visit_result->end());
        visit_result = nullptr;
    }
    Temp* temp = temp_map->newtemp();
    DEBUG_PRINT("new temp: " << temp->name());
    TempExp* dst = new TempExp(node->type, temp);
    
    set<Temp*>* def = new set<Temp*>{temp};
    set<Temp*>* use = new set<Temp*>();
    
    if (auto t = left->get_temp()) {
        use->insert(t->temp);
    }
    if (auto t = right->get_temp()) {
        use->insert(t->temp);
    }
    
    QuadMoveBinop* binop = new QuadMoveBinop(node, dst, left, node->op, right, def, use);
    result->push_back(binop);
    visit_result = result;
    output_term = new QuadTerm(dst);
}

void Tree2Quad::visit(Mem* node) {
#ifdef DEBUG
    cout << "Converting to Quad: Memory" << endl;
#endif
    if (!node || !node->mem) {
        output_term = nullptr;
        return;
    }
    vector<QuadStm*>* result = new vector<QuadStm*>();
    node->mem->accept(*this);
    if(visit_result){
        result->insert(result->end(), visit_result->begin(), visit_result->end());
        visit_result = nullptr;
    }
    QuadTerm* addr = output_term;
    
    Temp* temp = temp_map->newtemp();
    TempExp* dst = new TempExp(node->type, temp);
    
    set<Temp*>* def = new set<Temp*>{temp};
    set<Temp*>* use = new set<Temp*>();
    if (auto t = addr->get_temp()) {
        use->insert(t->temp);
    }
    
    QuadLoad* load = new QuadLoad(node, dst, addr, def, use);
    result->push_back(load);
    visit_result = result;
    output_term = new QuadTerm(dst);
    DEBUG_PRINT("finish Mem");
}

void Tree2Quad::visit(TempExp* node) {
#ifdef DEBUG
    cout << "Converting to Quad: Temp" << endl;
#endif
    DEBUG_PRINT("visiting temp: "<<node->temp->name());
    if (!node) {
        output_term = nullptr;
        return;
    }
    output_term = new QuadTerm(node);
    visit_result = nullptr;
}

void Tree2Quad::visit(Name* node) {
#ifdef DEBUG
    cout << "Converting to Quad: Name" << endl;
#endif
    // if (!node || !node->name) {
    //     output_term = nullptr;
    //     return;
    // }

    output_term = new QuadTerm(node->sname->name);
}

void Tree2Quad::visit(Const* node) {
#ifdef DEBUG
    cout << "Converting to Quad: Const" << endl;
#endif
    if (!node) {
        visit_result = nullptr;
        return;
    }

    output_term = new QuadTerm(node->constVal);
}

void Tree2Quad::visit(Call* node) {
#ifdef DEBUG
    cout << "Converting to Quad: Call" << endl;
#endif
    if (!node) {
        visit_result = nullptr;
        return;
    }

    vector<QuadTerm*>* args = new vector<QuadTerm*>();
    for (auto arg : *(node->args)) {
        arg->accept(*this);
        args->push_back(output_term);
    }

    node->obj->accept(*this);
    QuadTerm* obj_term = output_term;

    // 为函数调用创建新的临时变量（如果需要返回值）
    TempExp* dst = nullptr;
    if (node->type != Type::INT && node->type != Type::PTR) {
        Temp* temp = temp_map->newtemp();
        dst = new TempExp(node->type, temp);
    }

    // 收集使用的寄存器
    set<Temp*>* def = new set<Temp*>();
    set<Temp*>* use = new set<Temp*>();
    
    if (obj_term->get_temp()) {
        use->insert(obj_term->get_temp()->temp);
    }
    
    for (auto arg : *args) {
        if (auto temp = arg->get_temp()) {
            use->insert(temp->temp);
        }
    }

    if (dst) {
        def->insert(dst->temp);
    }
    
    // 创建函数调用四元式
    QuadCall* call = new QuadCall(node, dst, node->id, obj_term, args, def, use);
    
    if (dst) {
        QuadMoveCall* move_call = new QuadMoveCall(node, dst, call, def, use);
        visit_result = new vector<QuadStm*>{move_call};
        output_term = new QuadTerm(dst);
    } else {
        visit_result = new vector<QuadStm*>{call};
        output_term = nullptr;
    }
}

void Tree2Quad::visit(ExtCall* node) {
#ifdef DEBUG
    cout << "Converting to Quad: ExtCall" << endl;
#endif
    if (!node) {
        visit_result = nullptr;
        return;
    }

    // 处理所有参数
    vector<QuadStm*>* result = new vector<QuadStm*>();
    vector<QuadTerm*>* args = new vector<QuadTerm*>();
    for (auto arg : *(node->args)) {
        arg->accept(*this);
        args->push_back(output_term);
        if(visit_result) {
            result->insert(result->end(), visit_result->begin(), visit_result->end());
        }
    }

    set<Temp*>* def = new set<Temp*>();
    set<Temp*>* use = new set<Temp*>();
    
    for (auto arg : *args) {
        if (auto temp = arg->get_temp()) {
            use->insert(temp->temp);
        }
    }

    QuadExtCall* extcall = new QuadExtCall(node, nullptr, node->extfun, args, def, use);
    visit_result = new vector<QuadStm*>{extcall};
    output_term = nullptr;
}

void Tree2Quad::visit(Eseq* node) {

}