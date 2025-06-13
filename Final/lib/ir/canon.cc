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
    cout << "Linearizing: in size=" << sl->size() << endl;
#endif
    if (sl == nullptr || sl->size() == 0) {
        return new StmList();
    }
    StmList* sl_result = new StmList();
    for (auto stm : *sl) {
        if (stm == nullptr) continue;
        if (stm->getTreeKind() == Kind::EXPSTM) {
            Exp *exp = static_cast<ExpStm*>(stm)->exp;
            if (exp == nullptr) {
                cerr << "Error: No expression found in an expression statement!" << endl;
                continue;
            }
            if (exp->getTreeKind() == Kind::CONST) 
                continue;
            else 
                sl_result->push_back(stm);
        } else if (stm->getTreeKind() == Kind::SEQ) {
            StmList *seq2 = linearize(static_cast<Seq*>(stm)->sl);
            if (seq2 != nullptr) {
                sl_result->insert(sl_result->end(), seq2->begin(), seq2->end());
            }
        } else 
            sl_result->push_back(stm);
    }
#ifdef DEBUG
    cout << "END Linearized size: " << sl_result->size() << endl;
#endif
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
            Type t_type = exp->type;
            Stm *stm = new Move(new TempExp(t_type, t), exp);
            stmlist->push_back(stm);
            rotate(stmlist->rbegin(), stmlist->rbegin()+1, stmlist->rend());
            if (stmlist2 != nullptr && stmlist2->size()>0)
                stmlist->insert(stmlist->begin(), stmlist2->begin(), stmlist2->end());
            explist->push_back(new TempExp(t_type, t));
            rotate(explist->rbegin(), explist->rbegin()+1, explist->rend());
        }
    }
    if (stmlist->size() == 0) stmlist = nullptr;
    return new StmListExpListPair(stmlist, explist);
} 

void CanonVisitor::visit(Program* node) {
#ifdef DEBUG
    cout << "Canonicalizing T_program" << endl;
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
    cout << "Canonicalizing T_funcDecl" << endl;
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
    cout << "Canonicalizing T_block" << endl;
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
    cout << "Canonicalizing T_jump" << endl;
#endif
    if (node == nullptr) {
        sl_result = nullptr;
        return;
    }
    sl_result = new StmList(1, node);
}

void CanonVisitor::visit(Cjump* node) {
#ifdef DEBUG
    cout << "Canonicalizing T_cjump" << endl;
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

StmListExpPair* canon_move_left(Exp* dst, CanonVisitor &visitor) {
#ifdef DEBUG
    cout << "Canonicalizing T_move_left" << endl;
#endif
    //if the left is an ESEQ, special treatment! 
    //we just move the stm part of eseq and cannonicalize the 
    //remaining MOVE, and stick stm (cannonicalized) to the left part
    if (dst->getTreeKind() == Kind::ESEQ) {
#ifdef DEBUG
    cout << " MOVE TO An ESEQ... dst->getTreeKind() = " << kindToString(dst->getTreeKind()) << endl;
#endif
        vector<Stm*> *left_init_sl = new vector<Stm*>();
        Eseq *eseq = static_cast<Eseq*>(dst);
        Stm *stm = eseq->stm;
        if (stm != nullptr) {
            visitor.sl_result = nullptr;
            stm->accept(visitor);    
            if (visitor.sl_result != nullptr) {
                left_init_sl = linearize(visitor.sl_result);
#ifdef DEBUG
    cout << "-------within Canonicalizing left and each statement" << endl;
#endif
            } 
        } 
        //keep going for the left part
        StmListExpPair *rest_of_left = canon_move_left(eseq->exp, visitor);
        //make up the left canon result
        left_init_sl->insert(left_init_sl->end(), rest_of_left->first->begin(), rest_of_left->first->end());
        visitor.visit_result->first = linearize(left_init_sl);
        visitor.visit_result->second = rest_of_left->second; 
        return visitor.visit_result;
    } 

    if (dst->getTreeKind() == Kind::TEMPEXP) {
#ifdef DEBUG
    cout << " MOVE TO A Temp... dst->getTreeKind() = " << kindToString(dst->getTreeKind()) << endl;
#endif
        //in this case, the left is a temp, we keep it as is
        visitor.visit_result = new StmListExpPair();
        visitor.visit_result->first = new StmList();//empty one
        visitor.visit_result->second = dst;
        return visitor.visit_result;
    }
#ifdef DEBUG
    cout << " MOVE TO A MEM ... dst->getTreeKind() = " << kindToString(dst->getTreeKind()) << endl;
#endif
    //now it has to be a mem!
    if (dst->getTreeKind() != Kind::MEM) {
        cerr << "Error: Move left side is not well formed!" << endl;
        visitor.visit_result = nullptr;
        return visitor.visit_result;
    }
    visitor.visit_result = nullptr;
    Mem *left_mem = static_cast<Mem*>(dst);
    left_mem->mem->accept(visitor);
    if (visitor.visit_result == nullptr) {
        cerr << "Error: Move left side conversion failed!" << endl;
        visitor.visit_result = nullptr;
        return visitor.visit_result;
    }
    //the stm part of the visit result doesn't change
    //just need to stick the mem part to the exp part of the left
    visitor.visit_result->second = new Mem(dst->type, visitor.visit_result->second);
    return visitor.visit_result;
}

StmListExpPair* canon_move_right(Exp* src, CanonVisitor &visitor) {
#ifdef DEBUG
    cout << "Canonicalizing T_move_right" << endl;
#endif
    if (src == nullptr) {
        cerr << "Error: No source expression found in a Move" << endl;
        return nullptr;
    }
    //first do the regular canonicalization of the src 
    visitor.visit_result = nullptr;
    src->accept(visitor);
    if (visitor.visit_result == nullptr) {
        cerr << "Error: Move source conversion failed!" << endl;
        return nullptr;
    }
    
    if (src->getTreeKind() != Kind::CALL && src->getTreeKind() != Kind::EXTCALL) 
        return visitor.visit_result;

#ifdef DEBUG
    cout << " MOVE from a CALL or ExtCall ... src->getTreeKind() = " << kindToString(src->getTreeKind()) << endl;
#endif

    //otherwise, it's a call. this need a special treatment
    //the return has to be [(stm, ..., stm, Move(newtemp, call)), newtemp]
    //now rewrite it into [(stm, ..., stm), call]
    StmList *call_list = linearize(visitor.visit_result->first);
    if (call_list == nullptr || call_list->size() == 0) {
        //then it's already in the form of [nullptr, call]
        cout << "no need to do anything, second is =" << kindToString(visitor.visit_result->second->getTreeKind()) << endl;
        return visitor.visit_result;
    }
    //other wise the last one is a move
    Stm* end_call = call_list->back();
    if (end_call->getTreeKind() != Kind::MOVE) {
        cerr << "Error: Call source conversion failed!" << endl;
        visitor.visit_result = nullptr;
        return nullptr;
    }
    call_list->pop_back(); //remove the last call
    visitor.visit_result->first = call_list;
    if (src->getTreeKind()==Kind::CALL) {
        visitor.visit_result->second = static_cast<Call*>(static_cast<Move*>(end_call)->src);
    } else if (src->getTreeKind()==Kind::EXTCALL) {
        visitor.visit_result->second = static_cast<ExtCall*>(static_cast<Move*>(end_call)->src);
    }
    return visitor.visit_result;
}

//Move is the most complicated one since the left and right have different semantics
//if left is a temp or mem or eseq, need a special treatment
//if right is a call, need a special treatment
void CanonVisitor::visit(Move* node) {
#ifdef DEBUG
    cout << "Canonicalizing T_move" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }

    StmListExpPair *dst_visit_result = canon_move_left(node->dst, *this);
    StmListExpPair *src_visit_result = canon_move_right(node->src, *this);

#ifdef DEBUG
    cout << "dst_visit_result->first size: " << ((dst_visit_result->first == nullptr)? 0 : dst_visit_result->first->size()) << endl;
    cout << "src_visit_result->first size: " << ((src_visit_result->first == nullptr)? 0 : src_visit_result->first->size()) << endl;
    cout << "dst_visit_result->second: " << ((dst_visit_result->second == nullptr)? 0 : kindToString(dst_visit_result->second->getTreeKind())) << endl;
    cout << "src_visit_result->second: " << ((src_visit_result->second == nullptr)? 0 : kindToString(src_visit_result->second->getTreeKind())) << endl;
#endif
    //now stick them together (no reorder!)
    StmListExpPairList* pairList = new StmListExpPairList();

    sl_result = dst_visit_result->first;
    if (sl_result == nullptr) {
        sl_result = new StmList();
    } else {
        sl_result = linearize(sl_result);
    }
    if (src_visit_result->first != nullptr) {
        sl_result->insert(sl_result->end(), src_visit_result->first->begin(), src_visit_result->first->end());
    }
    sl_result = linearize(sl_result);
    if (dst_visit_result->second == nullptr || src_visit_result->second == nullptr) {
        cerr << "Error: Move conversion failed!" << endl;
        visit_result = nullptr;
        return;
    }
    sl_result->push_back(new Move(dst_visit_result->second, src_visit_result->second));
    return;
}

void CanonVisitor::visit(Seq* node) {
#ifdef DEBUG
    cout << "Canonicalizing T_seq" << endl;
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
    cout << "Canonicalizing T_label" << endl;
#endif
    if (node == nullptr) {
        sl_result = nullptr;
        return;
    }
    sl_result = new StmList(1, node);
}

void CanonVisitor::visit(Return* node) {
#ifdef DEBUG
    cout << "Canonicalizing T_return" << endl;
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
    cout << "Canonicalizing T_phi" << endl;
#endif
    if (node == nullptr) {
        sl_result = nullptr;
        return;
    }
    sl_result = new StmList(1, node);
}

void CanonVisitor::visit(ExpStm* node) {
#ifdef DEBUG
    cout << "Canonicalizing T_expStm" << endl;
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
    } else if (visit_result->first == nullptr || visit_result->first->size() == 0) {
        if (visit_result->second->getTreeKind() != Kind::TEMPEXP) //if it's a temp, don't need to add a new statement
            sl_result = new StmList(1, new ExpStm(visit_result->second));
    } else if (node->exp->getTreeKind() == Kind::CALL || node->exp->getTreeKind() == Kind::EXTCALL) {
        //if it's a call, need to strip off the move (for the call)
        if (sl_result == nullptr || sl_result->size() == 0) {
            cerr << "Error: No statement list found in a ExpStm when it's a expstm(call)!" << endl;
            sl_result = nullptr;
            return;
        } else {
            Move *mc = static_cast<Move*>(sl_result->at(sl_result->size()-1));
            sl_result->pop_back();
            sl_result->push_back(new ExpStm(mc->src));
        }
    }
    else {
        sl_result = visit_result->first;
        if (visit_result->second->getTreeKind() != Kind::TEMPEXP) //if it's a temp, don't need to add a new statement
            sl_result->push_back(new ExpStm(visit_result->second));
    }
}

void CanonVisitor::visit(Binop* node) {
#ifdef DEBUG
    cout << "Canonicalizing T_binop" << endl;
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
    cout << "Canonicalizing T_mem" << endl;
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
    //keep the node type
    visit_result = new StmListExpPair(sl_result, new Mem(node->type, visit_result->second));
}

void CanonVisitor::visit(TempExp* node) {
#ifdef DEBUG
    cout << "Canonicalizing T_temp" << endl;
#endif
    visit_result = new StmListExpPair(nullptr, node);
    return;
}

void CanonVisitor::visit(Eseq* node) {
#ifdef DEBUG
    cout << "Canonicalizing T_eseq" << endl;
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
    cout << "Canonicalizing T_name" << endl;
#endif
    if (node == nullptr) {
        visit_result = nullptr;
        return;
    }
    if (node->name == nullptr && node->sname == nullptr) {
        cerr << "Error: No name found in a Name" << endl;
        visit_result = nullptr;
        return;
    }
    visit_result = new StmListExpPair(nullptr, node);
}

void CanonVisitor::visit(Const* node) {
#ifdef DEBUG
    cout << "Canonicalizing T_const" << endl;
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
    cout << "Canonicalizing T_call" << endl;
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
    if (visit_result == nullptr) {
        cerr << "Error: Call object conversion failed!" << endl;
        visit_result = nullptr;
        return;
    }
    //if the obj is not a temp not const, then add [(move(newtem, obj), newtemp)]
    if (visit_result->second->getTreeKind() != Kind::TEMPEXP && 
                visit_result->second->getTreeKind() != Kind::CONST) {
        Temp *t = visitor_temp_map->newtemp();
        Type t_type = visit_result->second->type;
        Stm *move = new Move(new TempExp(t_type, t), visit_result->second);
        if (visit_result->first == nullptr) {
            visit_result->first = new StmList();
        }
        visit_result->first->push_back(move);
        visit_result->second = new TempExp(t_type, t);
        to_reorder->push_back(visit_result);
    } else
        to_reorder->push_back(visit_result);

    ExpList *args = node->args;
    if (args != nullptr) {
        for (auto arg : *args) {
            visit_result = nullptr;
            arg->accept(*this);
            if (visit_result != nullptr) {
                //if an arg is not a const and temp, then add [(move(newtemp, arg), newtemp)]
                if (visit_result->second->getTreeKind() != Kind::TEMPEXP && 
                        visit_result->second->getTreeKind() != Kind::CONST) {
                    Temp *t = visitor_temp_map->newtemp();
                    Type t_type = visit_result->second->type;
                    Stm *move = new Move(new TempExp(t_type, t), visit_result->second);
                    if (visit_result->first == nullptr) {
                        visit_result->first = new StmList();
                    }
                    StmListExpPair *vt = new StmListExpPair();
                    vt->first = new StmList(); 
                    vt->first->insert(vt->first->end(), visit_result->first->begin(), visit_result->first->end());
                    vt->first->push_back(move);
                    vt->second = new TempExp(t_type, t);
                    to_reorder->push_back(vt);
                } else
                    to_reorder->push_back(visit_result);
            }
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
    if (reorder_result->second == nullptr) {
        cerr << "Error: No expression found in a Call" << endl;
        visit_result = nullptr;
        return;
    }
    Exp *e = reorder_result->second->at(0); //the first one has to be the object
    reorder_result->second->erase(reorder_result->second->begin()); //the rest are args
    Type call_ret_type = node->type;
    Call* c = new Call(call_ret_type, node->id, e, reorder_result->second);
    sl_result->push_back(new Move(new TempExp(call_ret_type, t), c));
    visit_result = new StmListExpPair(sl_result, new TempExp(call_ret_type, t));
    //visit_result = new StmListExpPair(sl_result, c);
}

void CanonVisitor::visit(ExtCall* node) {
#ifdef DEBUG
    cout << "Canonicalizing T_extCall" << endl;
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
            if (visit_result->second->getTreeKind() != Kind::TEMPEXP &&
                        visit_result->second->getTreeKind() != Kind::CONST) {
                Temp *t = visitor_temp_map->newtemp();
                Type t_type = visit_result->second->type;
                Stm *move = new Move(new TempExp(t_type, t), visit_result->second);
                if (visit_result->first == nullptr) {
                    visit_result->first = new StmList();
                }
                visit_result->first->push_back(move);
                visit_result->second = new TempExp(t_type, t);
                to_reorder->push_back(visit_result);
            } else
                to_reorder->push_back(visit_result);
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
    Type call_ret_type = node->type;
    ExtCall* c = new ExtCall(node->type, node->extfun, reorder_result->second);
    TempExp *temp = new TempExp(call_ret_type, t);
    sl_result->push_back(new Move(temp, c));
    visit_result = new StmListExpPair(sl_result, temp);
    //visit_result = new StmListExpPair(sl_result, c);
}