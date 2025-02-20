#ifndef _FDMJAST_HH
#define _FDMJAST_HH

// This file defines all the AST node classes

#include "ASTheader.hh"
#include "FDMJAST.hh"
#include <string>
#include <vector>

using namespace std;

namespace fdmj {

class Program : public AST {
public:
  MainMethod *main;
  Program(Pos *pos, MainMethod *main) : AST(pos), main(main){};
  ASTKind getASTKind() override { return ASTKind::Program; }
  Program *clone() override;
  void accept(ASTVisitor &v) override { v.visit(this); }
};

class MainMethod : public AST {
public:
  vector<Stm *> *sl;
  MainMethod(Pos *pos, vector<Stm *> *sl) : AST(pos), sl(sl) {}
  ASTKind getASTKind() override { return ASTKind::MainMethod; }
  MainMethod *clone() override;
  void accept(ASTVisitor &v) override { v.visit(this); }
};

class Stm : public AST { // this is the class for all statements
public:
  Stm(Pos *pos) : AST(pos) {}
  virtual ASTKind getASTKind() = 0;
};

class Assign : public Stm {
public:
  Exp *left = nullptr;
  Exp *exp = nullptr;
  Assign(Pos *pos, Exp *left, Exp *exp) : Stm(pos), left(left), exp(exp) {}
  ASTKind getASTKind() override { return ASTKind::Assign; }
  Assign *clone() override;
  void accept(ASTVisitor &v) override { v.visit(this); }
};

class Return : public Stm {
public:
  Exp *exp = nullptr;
  Return(Pos *pos, Exp *exp) : Stm(pos), exp(exp) {}
  ASTKind getASTKind() override { return ASTKind::Return; }
  Return *clone() override;
  void accept(ASTVisitor &v) override { v.visit(this); }
};

class Exp : public AST { // this is the class for all expressions
public:
  Exp(Pos *pos) : AST(pos) {}
  virtual ASTKind getASTKind() = 0;
};

class BinaryOp : public Exp {
public:
  Exp *left = nullptr;
  OpExp *op = nullptr;
  Exp *right = nullptr;
  BinaryOp(Pos *pos, Exp *left, OpExp *op, Exp *right)
      : Exp(pos), left(left), op(op), right(right) {}
  ASTKind getASTKind() override { return ASTKind::BinaryOp; }
  BinaryOp *clone() override;
  void accept(ASTVisitor &v) override { v.visit(this); }
};

class UnaryOp : public Exp {
public:
  OpExp *op = nullptr;
  Exp *exp = nullptr;
  UnaryOp(Pos *pos, OpExp *op, Exp *exp) : Exp(pos), op(op), exp(exp) {}
  ASTKind getASTKind() override { return ASTKind::UnaryOp; }
  UnaryOp *clone() override;
  void accept(ASTVisitor &v) override { v.visit(this); }
};

class Esc : public Exp {
public:
  vector<Stm *> *sl = new vector<Stm *>();
  Exp *exp = nullptr;

  Esc(Pos *pos, vector<Stm *> *sl, Exp *exp) : Exp(pos), sl(sl), exp(exp) {}
  ASTKind getASTKind() override { return ASTKind::Esc; }
  Esc *clone() override;
  void accept(ASTVisitor &v) override { v.visit(this); }
};

class IdExp : public Exp {
public:
  string id;
  IdExp(Pos *pos, string id) : Exp(pos), id(id) {}
  ASTKind getASTKind() override { return ASTKind::IdExp; }
  IdExp *clone() override;
  void accept(ASTVisitor &v) override { v.visit(this); }
};

class IntExp : public Exp {
public:
  int val;
  IntExp(Pos *pos, int val) : Exp(pos), val(val) {}
  ASTKind getASTKind() override { return ASTKind::IntExp; }
  IntExp *clone() override;
  void accept(ASTVisitor &v) override { v.visit(this); }
};

class OpExp : public Exp {
public:
  string op;
  OpExp(Pos *pos, string op) : Exp(pos), op(op) {}
  ASTKind getASTKind() override { return ASTKind::OpExp; }
  OpExp *clone() override;
  void accept(ASTVisitor &v) override { v.visit(this); }
};

} // namespace fdmj

#endif