#define DEBUG
#undef DEBUG

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "treep.hh"
#include "canon.hh"

using namespace std;
using namespace tree;

#define FuncDeclList vector<FuncDecl*>
#define BlockList vector<Block*>
#define ExpList vector<Exp*>
#define StmList vector<Stm*>
#define TempList vector<Temp*> 
#define LabelList vector<Label*> 
#define StmListExpPair pair<StmList*, Exp*>
#define StmListExpPairList vector<pair<StmList*, Exp*>*>
#define StmListExpListPair pair<StmList*, ExpList*>

Program* canon(Program* prog) {
    CanonVisitor v;
    v.visitor_temp_map = new Temp_map();
    prog->accept(v);
    return v.prog_result;
}

//linearize all the nested SEQs in a stmList into one seq
StmList* linearize(StmList* sl) {
#ifdef DEBUG
    cout << "Linearizing" << endl;
#endif
    if (sl == nullptr) return nullptr;
    StmList* sl_result = new StmList();
    for (auto stm : *sl) {
        if (stm->getTreeKind() == Kind::EXPSTM &&
                static_cast<ExpStm*>(stm)->exp->getTreeKind() == Kind::CONST) {
            // skip the statement if it is an expression statement with a constant
            continue;
        }
        if (stm->getTreeKind() == Kind::SEQ) {
            StmList *seq2 = linearize(static_cast<Seq*>(stm)->sl);
            if (seq2 != nullptr) 
                sl_result->insert(sl_result->end(), seq2->begin(), seq2->end());
        } else 
            sl_result->push_back(stm);
    }
    return sl_result;
}

//reorder the statements and expressions in a block: given a stmList+exp pair list
//move all the statements to be executed first, and then use the expressions
//(assume the expressions are themselves containts no statements (in eseq form))
//The basic idea is to use temp to hold the values of the expressions (becomes a move statement)
//and then use the statements in the stmList (and the expression changes to the temp)
//Do this in the reverse order of the pairList seems to be easier.
StmListExpListPair* reorder(Temp_map *map, StmListExpPairList* pairList) {
#ifdef DEBUG
    cout << "Reordering" << endl;
#endif
    if (pairList == nullptr) return nullptr;
    StmList* stmlist = new StmList();
    ExpList *explist = new ExpList();
    for (int i = pairList->size()-1; i >= 0; i--) {
        StmList *stmlist2 = pairList->at(i)->first; //this is the statement list
        Exp *exp = pairList->at(i)->second; //this is the expression
#ifdef DEBUG
        cout << "reorder--stm size " << ((stmlist2 == nullptr)? 0 : stmlist2->size()) << endl;
        cout << "reorder--exp " << ((exp == nullptr)? 0 : kindToString(exp->getTreeKind())) << endl;
#endif
        if (exp == nullptr) {
            cerr << "Error: No expression found in a pair" << endl;
            continue;
        }
        if (stmlist->size() == 0) { //no statement to swap with the exp
            explist->push_back(exp);
            rotate(explist->rbegin(), explist->rbegin()+1, explist->rend());
            if (stmlist2 != nullptr && stmlist2->size()>0) 
                stmlist->insert(stmlist->begin(), stmlist2->begin(), stmlist2->end());
        } else { //need to swap the exp with the last statement
            Temp *t = map->newtemp();
            Stm *stm = new Move(new TempExp(Type::INT, t), exp);
            stmlist->push_back(stm);
            rotate(stmlist->rbegin(), stmlist->rbegin()+1, stmlist->rend());
            if (stmlist2 != nullptr && stmlist2->size()>0)
                stmlist->insert(stmlist->begin(), stmlist2->begin(), stmlist2->end());
            explist->push_back(new TempExp(Type::INT, t));
            rotate(explist->rbegin(), explist->rbegin()+1, explist->rend());
        }
    }
    if (stmlist->size() == 0) stmlist = nullptr;
    return new StmListExpListPair(stmlist, explist);
} 

void CanonVisitor::visit(Program* node) {
#ifdef DEBUG
    cout << "Visiting T_program" << endl;
#endif
    if (node == nullptr) {
        prog_result = nullptr;
        return;
    }
    FuncDeclList *fl = new FuncDeclList();
    if (node->funcdecllist != nullptr) {
        for (auto func : *node->funcdecllist) {
            fd_result = nullptr;
            func->accept(*this);
            if (fd_result != nullptr) fl->push_back(fd_result);
        }
    }
    prog_result = new Program(fl);
}

void CanonVisitor::visit(FuncDecl* node) {
#ifdef DEBUG
    cout << "Visiting T_funcDecl" << endl;
#endif
    if (node == nullptr) {
        fd_result = nullptr;
        return;
    }

    if (visitor_temp_map!=nullptr) delete visitor_temp_map;
    visitor_temp_map = new Temp_map();
    visitor_temp_map->next_temp = node->last_temp_num+1;
    visitor_temp_map->next_label = node->last_label_num+1;

    BlockList *bl = new BlockList();
    if (node->blocks != nullptr) {
        for (auto block : *node->blocks) {
            b_result = nullptr;
            block->accept(*this);
            if (b_result != nullptr) bl->push_back(b_result);
        }
    }
    fd_result = new FuncDecl(node->name, node->args, bl, node->return_type, 
        visitor_temp_map->next_temp-1, visitor_temp_map->next_label-1);
}

void CanonVisitor::visit(Block* node) {
#ifdef DEBUG
    cout << "Visiting T_block" << endl;
#endif
    if (node == nullptr) {
        b_result = nullptr;
        return;
    }
    StmList *sl = new StmList();
    if (node->sl != nullptr) {
        for (auto stm : *node->sl) {
            sl_result = nullptr;
            stm->accept(*this);
            if (sl_result!= nullptr) sl->insert(sl->end(), sl_result->begin(), sl_result->end());
        }
    }
    if (sl->size() == 0 ) sl = nullptr;
    else sl = linearize(sl);
    b_result = new Block(node->entry_label, node->exit_labels, sl);
}

void CanonVisitor::visit(Jump* node) {
#ifdef DEBUG
    cout << "Visiting T_jump" << endl;
#endif
    if (node == nullptr) {
        sl_result = nullptr;
        return;
    }
    sl_result = new StmList(1, node);
}

void CanonVisitor::visit(Cjump* node) {
#ifdef DEBUG
    cout << "Visiting T_cjump" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    visit_result = nullptr;
    Exp *left = node->left; 
    if (left == nullptr) {
        cerr << "Error: No left expression found in a CJump" << endl;
        visit_result = nullptr;
        return;
    }
    left->accept(*this);
    StmListExpPair *left_visit_result = visit_result;
    sl_result = nullptr;
    Exp *right = node->right;
    if (right == nullptr) {
        cerr << "Error: No right expression found in a CJump" << endl;
        sl_result = nullptr;
        return;
    }
    right->accept(*this);
    StmListExpPair *right_visit_result = visit_result;

    //form list of pairs (of stmlist and exp) to be passed to reorder
    StmListExpPairList* pairList = new StmListExpPairList();
    pairList->push_back(left_visit_result); pairList->push_back(right_visit_result);
    StmListExpListPair *reorder_result = reorder(visitor_temp_map, pairList);
    if (reorder_result->first == nullptr) {
        sl_result = new StmList();
    } else 
        sl_result = reorder_result->first;
    Cjump *cjump = new Cjump(node->relop, reorder_result->second->at(0), reorder_result->second->at(1), node->t, node->f);
    sl_result->push_back(cjump);
    sl_result = linearize(sl_result);
}

void CanonVisitor::visit(Move* node) {
#ifdef DEBUG
    cout << "Visiting T_move" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    Exp *dst = node->dst;
    if (dst == nullptr) {
        cerr << "Error: No destination expression found in a Move" << endl;
        visit_result = nullptr;
        return;
    }
    Exp *dst_mem = nullptr;
    TempExp *dst_temp = nullptr;
    if (dst->getTreeKind() == Kind::TEMPEXP) {
        dst_temp = static_cast<TempExp*>(dst);
    } else { //has to be a MEM
        if (dst->getTreeKind() != Kind::MEM) {
            cerr << "Error: Destination of a Move is not a Temp or Mem" << endl;
            visit_result = nullptr;
            return;
        }
        dst_mem = static_cast<Mem*>(dst)->mem;
    }
    StmListExpPair *dst_visit_result = nullptr;
    if (dst_mem != nullptr) {
        visit_result = nullptr;
        dst_mem->accept(*this);
        dst_visit_result = visit_result;
    }
    Exp *src = node->src;
    if (src == nullptr) {
        cerr << "Error: No source expression found in a Move" << endl;
        sl_result = nullptr;
        return;
    }
    visit_result = nullptr;
    src->accept(*this);
    StmListExpPair *src_visit_result = visit_result;
    //put together and pass to reorder
    StmListExpPairList* pairList = new StmListExpPairList();
    if (dst_visit_result != nullptr) pairList->push_back(dst_visit_result);
    if (src_visit_result != nullptr) pairList->push_back(src_visit_result);
#ifdef DEBUG
    if (dst_visit_result !=nullptr) {
        cout << "in T_move----dst-stm size " << ((dst_visit_result->first == nullptr)? 0 : dst_visit_result->first->size()) << endl;
        cout << "in T_move----dst-exp " << ((dst_visit_result->second == nullptr)? 0 : kindToString(dst_visit_result->second->getTreeKind())) << endl;
    }
    if (src_visit_result != nullptr) {
        cout << "in T_move----src-stm size " << ((src_visit_result->first == nullptr)? 0 : src_visit_result->first->size()) << endl;
        cout << "in T_move----src-exp " << ((src_visit_result->second == nullptr)? 0 : kindToString(src_visit_result->second->getTreeKind())) << endl;
    }
#endif
    StmListExpListPair *reorder_result = reorder(visitor_temp_map, pairList);
#ifdef DEBUG
    cout << "in T_move--after reordering -- stm size" << ((reorder_result->first == nullptr)? 0 : reorder_result->first->size()) << endl;
#endif
    if (reorder_result->first == nullptr)
        sl_result = new StmList();
    else 
        sl_result = reorder_result->first;
    Move *move = nullptr;
    if (dst_mem != nullptr) 
        move = new Move(new Mem(reorder_result->second->at(0)), reorder_result->second->at(1));
    else 
        move = new Move(dst_temp, reorder_result->second->at(0));
    sl_result->push_back(move);
    sl_result = linearize(sl_result);
}

void CanonVisitor::visit(Seq* node) {
#ifdef DEBUG
    cout << "Visiting T_seq" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    visit_result = nullptr;
    StmList *sl = node->sl;
    if (sl == nullptr) {
        cerr << "Error: No statement list found in a Seq" << endl;
        visit_result = nullptr;
        return;
    }
    StmList* seq_result = new StmList();
    for (auto stm : *sl) {
        sl_result = nullptr;
        stm->accept(*this);
        if (sl_result != nullptr) seq_result->insert(seq_result->end(), sl_result->begin(), sl_result->end());
    }
    if (seq_result->size() == 0) seq_result = nullptr;
    sl_result = linearize(seq_result);
}

void CanonVisitor::visit(LabelStm* node) {
#ifdef DEBUG
    cout << "Visiting T_label" << endl;
#endif
    if (node == nullptr) {
        sl_result = nullptr;
        return;
    }
    sl_result = new StmList(1, node);
}

void CanonVisitor::visit(Return* node) {
#ifdef DEBUG
    cout << "Visiting T_return" << endl;
#endif
    if (node == nullptr) {
        sl_result = nullptr;
        return;
    }
    if (node->exp == nullptr) {
        cerr << "Error: No expression found in a Return" << endl;
        sl_result = nullptr;
        return;
    }
    node->exp->accept(*this);
    if (visit_result == nullptr) {
        sl_result = nullptr;
        return;
    } else if (visit_result->first== nullptr) {
        sl_result = new StmList(1, new Return(visit_result->second));
    } else {
        sl_result = visit_result->first;
        sl_result->push_back(new Return(visit_result->second));
    }
}

void CanonVisitor::visit(Phi* node) {
#ifdef DEBUG
    cout << "Visiting T_phi" << endl;
#endif
    if (node == nullptr) {
        sl_result = nullptr;
        return;
    }
    sl_result = new StmList(1, node);
}

void CanonVisitor::visit(ExpStm* node) {
#ifdef DEBUG
    cout << "Visiting T_expStm" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    Exp *exp = node->exp;
    if (exp == nullptr) {
        sl_result = nullptr;
        return;
    }
    exp->accept(*this);
    if (visit_result == nullptr) {
        sl_result = nullptr;
        return;
    } else if (visit_result->first == nullptr) {
        sl_result = new StmList(1, new ExpStm(visit_result->second));
    } else {
        sl_result = visit_result->first;
        sl_result->push_back(new ExpStm(visit_result->second));
    }
}

void CanonVisitor::visit(Binop* node) {
#ifdef DEBUG
    cout << "Visiting T_binop" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    Exp *left = node->left;
    if (left == nullptr) {
        cerr << "Error: No left expression found in a Binop" << endl;
        visit_result = nullptr;
        return;
    }
    visit_result = nullptr;
    left->accept(*this);
    StmListExpPair *left_visit_result = visit_result;
    Exp *right = node->right;
    if (right == nullptr) {
        cerr << "Error: No right expression found in a Binop" << endl;
        sl_result = nullptr;
        return;
    }
    visit_result = nullptr;
    right->accept(*this);
    StmListExpPair *right_visit_result = visit_result;
    StmListExpPairList* pairList = new StmListExpPairList();
    pairList->push_back(left_visit_result); pairList->push_back(right_visit_result);
    StmListExpListPair *reorder_result = reorder(visitor_temp_map, pairList);
    if (reorder_result->first == nullptr)
        sl_result = new StmList();
    else 
        sl_result = reorder_result->first;
    sl_result = linearize(sl_result);
    Binop *binop = new Binop(node->type, node->op, reorder_result->second->at(0), reorder_result->second->at(1));
    visit_result = new StmListExpPair(sl_result, binop);
}

void CanonVisitor::visit(Mem* node) {
#ifdef DEBUG
    cout << "Visiting T_mem" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    Exp *mem = node->mem;
    if (mem == nullptr) {
        cerr << "Error: No memory expression found in a Mem" << endl;
        visit_result = nullptr;
        return;
    }
    visit_result = nullptr;
    mem->accept(*this);
    if (visit_result == nullptr) {
        sl_result = nullptr;
        return;
    } else if (visit_result->first == nullptr) {
        sl_result = nullptr;
    } else {
        sl_result = visit_result->first;
    }
    visit_result = new StmListExpPair(sl_result, new Mem(visit_result->second));
}

void CanonVisitor::visit(TempExp* node) {
#ifdef DEBUG
    cout << "Visiting T_temp" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    visit_result = nullptr;
    Temp *temp = node->temp;
    if (temp == nullptr) {
        cerr << "Error: No temp found in a Temp" << endl;
        visit_result = nullptr;
        return;
    }
    visit_result = new StmListExpPair(nullptr, new TempExp(node->type, temp));   
}

void CanonVisitor::visit(Eseq* node) {
#ifdef DEBUG
    cout << "Visiting T_eseq" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    Stm *stm = node->stm;
    if (stm == nullptr) {
        cerr << "Error: No statement found in an Eseq" << endl;
        visit_result = nullptr;
        return;
    }
    sl_result = nullptr;
    stm->accept(*this);
    StmList*stm_visit_result = sl_result;
    Exp *exp = node->exp;
    if (exp == nullptr) {
        cerr << "Error: No expression found in an Eseq" << endl;
        sl_result = nullptr;
        return;
    }
    visit_result = nullptr;
    exp->accept(*this);
    StmListExpPair *exp_visit_result = visit_result;
    if (stm_visit_result == nullptr) {
        sl_result = new StmList();
    } else {
        sl_result = stm_visit_result;
    }
    if (exp_visit_result->first != nullptr) {
        sl_result->insert(sl_result->end(), exp_visit_result->first->begin(), exp_visit_result->first->end());
    }
    sl_result = linearize(sl_result);
    visit_result = new StmListExpPair(sl_result, exp_visit_result->second);
}

void CanonVisitor::visit(Name* node) {
#ifdef DEBUG
    cout << "Visiting T_name" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    if (node->name == nullptr) {
        cerr << "Error: No name found in a Name" << endl;
        visit_result = nullptr;
        return;
    }
    visit_result = new StmListExpPair(nullptr, node);
}

void CanonVisitor::visit(Const* node) {
#ifdef DEBUG
    cout << "Visiting T_const" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    visit_result = new StmListExpPair(nullptr, node);
}

//call needs to be taken out of expressions
//so we always make call to assign to a temp and make it a statement
//and replace the call with a temp in the expression
void CanonVisitor::visit(Call* node) {
#ifdef DEBUG
    cout << "Visiting T_call" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    StmListExpPairList *to_reorder = new StmListExpPairList();
    visit_result = nullptr;
    Exp *obj = node->obj;
    if (obj == nullptr) { 
        cerr << "Error: No object found in a Call" << endl;
        visit_result = nullptr;
        return;
    }
    obj->accept(*this);
    to_reorder->push_back(visit_result);
    ExpList *args = node->args;
    if (args != nullptr) {
        for (auto arg : *args) {
            visit_result = nullptr;
            arg->accept(*this);
            if (visit_result != nullptr) to_reorder->push_back(visit_result);
        }
    }
    StmListExpListPair *reorder_result = reorder(visitor_temp_map, to_reorder);
    if (reorder_result->first == nullptr) {
        sl_result = new StmList();
    } else {
        sl_result = reorder_result->first;
    }
    sl_result = linearize(sl_result);
    Temp *t = visitor_temp_map->newtemp();
    Exp *e = reorder_result->second->at(0);
    reorder_result->second->erase(reorder_result->second->begin()); 
    Call* c = new Call(node->type, node->id, e, reorder_result->second);
    sl_result->push_back(new Move(new TempExp(Type::INT, t), c));
    visit_result = new StmListExpPair(sl_result, new TempExp(Type::INT, t));
}

void CanonVisitor::visit(ExtCall* node) {
#ifdef DEBUG
    cout << "Visiting T_extCall" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    StmListExpPairList *to_reorder = new StmListExpPairList();
    ExpList *args = node->args;
    if (args != nullptr) {
        for (auto arg : *args) {
            visit_result = nullptr;
            arg->accept(*this);
            if (visit_result != nullptr) to_reorder->push_back(visit_result);
        }
    }
    StmListExpListPair *reorder_result = reorder(visitor_temp_map, to_reorder);
    if (reorder_result->first == nullptr) {
        sl_result = new StmList();
    } else {
        sl_result = reorder_result->first;
    }
    sl_result = linearize(sl_result);
    Temp *t = visitor_temp_map->newtemp();
    ExtCall* c = new ExtCall(node->type, node->extfun, reorder_result->second);
    TempExp *temp = new TempExp(Type::INT, t);
    sl_result->push_back(new Move(temp, c));
    visit_result = new StmListExpPair(sl_result, temp);
}