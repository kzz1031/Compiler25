#define DEBUG
#undef DEBUG

#include <iostream>
#include <variant>
#include <vector>
#include <string>
#include "temp.hh"
#include "treep.hh"
#include "tr_exp.hh"

using namespace std;

Tr_ex* Tr_ex::unEx(Temp_map* tm) {
    return this;
}

Tr_nx* Tr_ex::unNx(Temp_map* tm) {
    return new Tr_nx(new tree::ExpStm(this->exp));
}

Tr_cx* Tr_ex::unCx(Temp_map* tm) {
    tree::Exp* e = exp;
    Patch_list* t = new Patch_list();
    Patch_list* f = new Patch_list();
    tree::Label* tl = tm->newlabel();
    tree::Label* fl = tm->newlabel();
    t->add_patch(tl);
    f->add_patch(fl);
    if(e == NULL) {
        cerr<<"===expression is NULL==="<<endl;
    }
    return new Tr_cx(t, f, new tree::Cjump("!=", e, new tree::Const(0), tl, fl));
}

Tr_ex* Tr_nx::unEx(Temp_map* tm) {
    return new Tr_ex(new tree::Eseq(tree::Type::INT, stm, new tree::Const(0)));
}

Tr_nx* Tr_nx::unNx(Temp_map* tm) {
    return this;
}

Tr_cx* Tr_nx::unCx(Temp_map* tm) {
    return nullptr; //should never be called
}

Tr_ex* Tr_cx::unEx(Temp_map* tm) {
    tree::Label* t = tm->newlabel();
    true_list->patch(t);
    tree::Label* f = tm->newlabel();
    false_list->patch(f);
    tree::Label* o = tm->newlabel();
    //now create the conditional jump to t or f for 1 or 0 respectively
    vector<tree::Stm*>* sl  = new vector<tree::Stm*>();
    TempExp *te = new TempExp(tree::Type::INT, tm->newtemp());
    sl->push_back(new Move(te, new Const(0)));
    sl->push_back(this->stm);
    sl->push_back(new tree::LabelStm(t));
    sl->push_back(new Move(te, new Const(1)));
    sl->push_back(new tree::LabelStm(f));
    return new Tr_ex(new tree::Eseq(tree::Type::INT, new tree::Seq(sl), te));
}

Tr_nx* Tr_cx::unNx(Temp_map* tm) {
    tree::Label* t = tm->newlabel();
    true_list->patch(t);
    false_list->patch(t);
    vector<tree::Stm*>* sl = new vector<tree::Stm*>();
    sl->push_back(this->stm);
    sl->push_back(new tree::LabelStm(t));
    return new Tr_nx(new tree::Seq(sl));
}

Tr_cx* Tr_cx::unCx(Temp_map* tm) {
    return this;
}