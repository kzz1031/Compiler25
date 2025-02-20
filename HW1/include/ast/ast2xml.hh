#ifndef _AST2XML_HH
#define _AST2XML_HH

#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "tinyxml2.hh"

using namespace std;
using namespace fdmj;
using namespace tinyxml2;

XMLDocument *ast2xml(fdmj::Program *root, bool location_flag);

class AST2XML : public fdmj::ASTVisitor {
public:
  XMLDocument *doc; // XMLDocument to store the AST
  XMLElement *
      el; // temp to remember the results during the AST is recursively visited.

public:
  void visit(Program *node) override;
  void visit(MainMethod *node) override;
  void visit(Assign *node) override;
  void visit(Return *node) override;
  void visit(BinaryOp *node) override;
  void visit(UnaryOp *node) override;
  void visit(Esc *node) override;
  void visit(IdExp *node) override;
  void visit(OpExp *node) override;
  void visit(IntExp *node) override;
};

#endif