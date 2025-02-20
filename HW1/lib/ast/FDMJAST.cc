#define DEBUG
#undef DEBUG

// This file defines all the AST node clone methods
#include "FDMJAST.hh"
#include "ASTheader.hh"
#include <string>
#include <variant>
#include <vector>

using namespace std;
using namespace fdmj;

namespace fdmj {

string stringASTKind(ASTKind kind) {
  switch (kind) {
  case ASTKind::Program:
    return "Program";
  case ASTKind::MainMethod:
    return "MainMethod";
  case ASTKind::Assign:
    return "Assign";
  case ASTKind::Return:
    return "Return";
  case ASTKind::BinaryOp:
    return "BinaryOp";
  case ASTKind::UnaryOp:
    return "UnaryOp";
  case ASTKind::Esc:
    return "Esc";
  case ASTKind::IdExp:
    return "IdExp";
  case ASTKind::OpExp:
    return "OpExp";
  case ASTKind::IntExp:
    return "IntExp";
  default:
    return "Unknown";
  }
}

template <class T> vector<T *> *cloneList(vector<T *> *tl) {
  if (tl == nullptr || tl->size() == 0)
    return nullptr;
  vector<T *> *v = new vector<T *>();
  for (auto x : *tl) {
    ASTKind i = x->getASTKind();
    v->push_back(static_cast<T *>(x->clone()));
  }
  return v;
}

} // namespace fdmj

Program *Program::clone() {
  MainMethod *m =
      (this->main) ? static_cast<MainMethod *>(main->clone()) : nullptr;
  return new Program(pos->clone(), m);
}

MainMethod *MainMethod::clone() {
  vector<Stm *> *sl_clone = (this->sl) ? cloneList<Stm>(this->sl) : nullptr;
  return new MainMethod(pos->clone(), sl_clone);
}

Assign *Assign::clone() {
  Exp *l = (this->left) ? static_cast<Exp *>(left->clone()) : nullptr;
  Exp *e = (this->exp) ? static_cast<Exp *>(exp->clone()) : nullptr;
  return new Assign(pos->clone(), l, e);
}

Return *Return::clone() {
  Exp *e = (this->exp) ? static_cast<Exp *>(exp->clone()) : nullptr;
  return new Return(pos->clone(), e);
}

BinaryOp *BinaryOp::clone() {
  Exp *l = (this->left) ? static_cast<Exp *>(left->clone()) : nullptr;
  Exp *r = (this->right) ? static_cast<Exp *>(right->clone()) : nullptr;
  OpExp *o = (this->op) ? static_cast<OpExp *>(op->clone()) : nullptr;
  return new BinaryOp(pos->clone(), l, o, r);
}

UnaryOp *UnaryOp::clone() {
  Exp *e = (this->exp) ? static_cast<Exp *>(exp->clone()) : nullptr;
  OpExp *o = (this->op) ? static_cast<OpExp *>(op->clone()) : nullptr;
  return new UnaryOp(pos->clone(), o, e);
}

Esc *Esc::clone() {
  Exp *e = (this->exp) ? static_cast<Exp *>(exp->clone()) : nullptr;
  vector<Stm *> *s = (this->sl) ? cloneList<Stm>(this->sl) : nullptr;
  return new Esc(pos->clone(), s, e);
}

IdExp *IdExp::clone() { return new IdExp(pos->clone(), this->id); }

IntExp *IntExp::clone() { return new IntExp(pos->clone(), val); }

OpExp *OpExp::clone() { return new OpExp(pos->clone(), op); }