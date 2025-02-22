#ifndef _EXECUTOR_H
#define _EXECUTOR_H

#include "ASTheader.hh"
#include "FDMJAST.hh"
#include <unordered_map>
#include <string>

using namespace std;
using namespace fdmj;

int execute(Program *root);

class Executor : public ASTVisitor {
public:
  unordered_map<string, int> variableTable;
  int returnValue = 0;
  AST *newNode = nullptr;

  Executor() {}
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
