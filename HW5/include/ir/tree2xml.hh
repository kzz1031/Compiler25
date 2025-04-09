#ifndef _TREE2XML_HH
#define _TREE2XML_HH

#include "treep.hh"
#include "tinyxml2.hh"

using namespace std;
using namespace tree;
using namespace tinyxml2;

class Tree2XML : public Visitor {
public:
    XMLDocument* doc;
    XMLElement* visit_result;

    void visit(tree::Program *prog) override;
    void visit(tree::FuncDecl *func) override;
    void visit(tree::Block *block) override;
    void visit(tree::Jump *jump) override;
    void visit(tree::Cjump *cjump) override;
    void visit(tree::Move *move) override;
    void visit(tree::Seq *seq) override;
    void visit(tree::LabelStm *labelstm) override;
    void visit(tree::Return *ret) override;
    void visit(tree::Phi *phi) override;
    void visit(tree::ExpStm *exp) override;
    void visit(tree::Binop *binop) override;
    void visit(tree::Mem *mem) override;
    void visit(tree::TempExp *tempexp) override;
    void visit(tree::Eseq *eseq) override;
    void visit(tree::Name *name) override;
    void visit(tree::Const *const) override;
    void visit(tree::Call *call) override;
    void visit(tree::ExtCall *extcall) override;
};

XMLDocument* tree2xml(tree::Program* prog);

#endif