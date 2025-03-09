#include "constantPropagation.hh"
#include "MinusIntConverter.hh"
#include "ASTheader.hh"
#include "FDMJAST.hh"
#include <iostream>
#include <variant>
#include <vector>

using namespace std;
using namespace fdmj;

#define StmList vector<Stm *>
#define ExpList vector<Exp *>

Program *constantPropagationRewrite(Program *root) {
  //printf("Constant Propagation Rewrite\n");
  if (root == nullptr)
    return nullptr;
  ConstantPropagation v(nullptr);
  root->accept(v);
  return dynamic_cast<Program *>(v.newNode);
}

template <typename T>
static vector<T *> *visitList(ConstantPropagation &v, vector<T *> *tl) {
  if (tl == nullptr || tl->size() == 0)
    return nullptr;
  vector<T *> *vt = new vector<T *>();
  for (T *x : *tl) {
    if (x == nullptr)
      continue;
    x->accept(v);
    if (v.newNode == nullptr)
      continue;
    vt->push_back(static_cast<T *>(v.newNode));
  }
  if (vt->size() == 0) {
    delete vt;
    vt = nullptr;
  }
  return vt;
}

void ConstantPropagation::visit(Program *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  MainMethod *m;
  if (node->main == nullptr)
    m = nullptr;
  else {
    node->main->accept(*this);
    if (newNode == nullptr)
      m = nullptr;
    else
      m = static_cast<MainMethod *>(newNode);
  }
  newNode = new Program(node->getPos()->clone(), m);
}

void ConstantPropagation::visit(MainMethod *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  StmList *sl = nullptr;
  if (node->sl != nullptr)
    sl = visitList<Stm>(*this, node->sl);
  newNode = new MainMethod(node->getPos()->clone(), sl);
}

void ConstantPropagation::visit(Assign *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  Exp *l = nullptr;
  if (node->left != nullptr) {
    node->left->accept(*this);
    l = static_cast<Exp *>(newNode);
  } else {
    cerr << "Error: No left expression found in the Assign statement" << endl;
    newNode = nullptr;
    return;
  }
  Exp *r = nullptr;
  if (node->exp != nullptr) {
    node->exp->accept(*this);
    r = static_cast<Exp *>(newNode);
  } else {
    cerr << "Error: No right expression found in the Assign statement" << endl;
    newNode = nullptr;
    return;
  }
  newNode = new Assign(node->getPos()->clone(), l, r);
}

void ConstantPropagation::visit(Return *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  Exp *e = nullptr;
  if (node->exp != nullptr) {
    node->exp->accept(*this);
    e = static_cast<Exp *>(newNode);
  } else {
    cerr << "Error: No expression found in the Return statement" << endl;
    newNode = nullptr;
    return;
  }
  newNode = new Return(node->getPos()->clone(), e);
}

void ConstantPropagation::visit(BinaryOp *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  Exp *l = nullptr;
  if (node->left != nullptr) {
    node->left->accept(*this);
    l = static_cast<Exp *>(newNode);
  } else {
    cerr << "Error: No left expression found in the BinaryOp statement" << endl;
    newNode = nullptr;
    return;
  }
  Exp *r = nullptr;
  if (node->right != nullptr) {
    node->right->accept(*this);
    r = static_cast<Exp *>(newNode);
  } else {
    cerr << "Error: No right expression found in the BinaryOp statement"
         << endl;
    newNode = nullptr;
    return;
  }
  // Constant propagation logic
  if (l->getASTKind() == ASTKind::IntExp && r->getASTKind() == ASTKind::IntExp) {
    int leftVal = static_cast<IntExp *>(l)->val;
    int rightVal = static_cast<IntExp *>(r)->val;
    int result;
    if (node->op->op == "+") {
      result = leftVal + rightVal;
    } else if (node->op->op == "-") {
      result = leftVal - rightVal;
    } else if (node->op->op == "*") {
      result = leftVal * rightVal;
    } else if (node->op->op == "/") {
      if (rightVal == 0) {
        throw runtime_error("Error: Division by zero");        
        newNode = new BinaryOp(node->getPos()->clone(), l, node->op->clone(), r);
        return;
      }
      result = leftVal / rightVal;
    } else {
      newNode = new BinaryOp(node->getPos()->clone(), l, node->op->clone(), r);
      return;
    }
    newNode = new IntExp(node->getPos()->clone(), result);
  } else {
    newNode = new BinaryOp(node->getPos()->clone(), l, node->op->clone(), r);
  }
}

void ConstantPropagation::visit(UnaryOp *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  Exp *e = nullptr;
  if (node->exp != nullptr) {
    node->exp->accept(*this);
    e = static_cast<Exp *>(newNode);
  } else {
    cerr << "Error: No expression found in the UnaryOp statement" << endl;
    newNode = nullptr;
    return;
  }
  if(node->op->op == "-" && e->getASTKind() == ASTKind::IntExp) {
    int val = -(static_cast<IntExp *>(e)->val);
    newNode = new IntExp(node->getPos()->clone(), val);
    return;
  }
  newNode = new UnaryOp(node->getPos()->clone(), node->op->clone(), e);
}

void ConstantPropagation::visit(Esc *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  StmList *sl = nullptr;
  if (node->sl != nullptr)
    sl = visitList<Stm>(*this, node->sl);
  Exp *e = nullptr;
  if (node->exp != nullptr) {
    node->exp->accept(*this);
    e = static_cast<Exp *>(newNode);
  }
  newNode = new Esc(node->getPos()->clone(), sl, e);
}

void ConstantPropagation::visit(IdExp *node) {
  newNode = (node == nullptr) ? nullptr : static_cast<IdExp *>(node->clone());
}

void ConstantPropagation::visit(OpExp *node) {
  newNode = (node == nullptr) ? nullptr : static_cast<OpExp *>(node->clone());
}

void ConstantPropagation::visit(IntExp *node) {
  newNode = (node == nullptr) ? nullptr : static_cast<IntExp *>(node->clone());
}
