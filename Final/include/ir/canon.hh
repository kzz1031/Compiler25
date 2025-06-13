#ifndef _CANON_H
#define _CANON_H

#include "temp.hh"
#include "treep.hh"

using namespace tree;

/*
#define StmList vector<Stm*>
#define FuncDeclList vector<FuncDecl*>
#define BlockList vector<Block*>
#define ExpList vector<Exp*>
#define StmList vector<Stm*>
*/

tree::Program* canon(tree::Program* prog);

class CanonVisitor: public tree::Visitor {
public:
    tree::Program* prog_result; //this is to store the result of a visit to the program
    tree::FuncDecl* fd_result; //this is to store the result of a visit to a function declaration
    tree::Block* b_result; //this is to store the result of a visit to a block
    std::vector<tree::Stm*> *sl_result; //this is to store the result of a visit to a statement list
    pair<std::vector<tree::Stm*>*, tree::Exp*> *visit_result; //this is to store the result of a visit to statements and expressions
    Temp_map *visitor_temp_map;

    void visit(tree::Program* node) override;
    void visit(tree::FuncDecl* node)  override;
    void visit(tree::Block* node) override;
    void visit(tree::Jump* node) override;
    void visit(tree::Cjump* node) override;
    void visit(tree::Move* node) override;
    void visit(tree::Seq* node) override;
    void visit(tree::LabelStm* node) override;
    void visit(tree::Return* node) override;
    void visit(tree::Phi* node) override;
    void visit(tree::ExpStm* node) override;
    void visit(tree::Binop* node) override;
    void visit(tree::Mem* node) override;
    void visit(tree::TempExp* node) override;
    void visit(tree::Eseq* node) override;
    void visit(tree::Name* node) override;
    void visit(tree::Const* node) override;
    void visit(tree::Call* node) override;
    void visit(tree::ExtCall* node) override;
};

#endif