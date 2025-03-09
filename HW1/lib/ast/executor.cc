#include "executor.hh"
#include "ASTheader.hh"
#include "FDMJAST.hh"
#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;
using namespace fdmj;

int execute(Program *root) {
  if (root == nullptr)
    return 0;
  Executor executor;
  root->accept(executor);
  return executor.returnValue;
}

void Executor::visit(Program *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  if (node->main != nullptr) {
    node->main->accept(*this);
  }
}

void Executor::visit(MainMethod *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  bool hasReturn = false;
  if (node->sl != nullptr) {
    for (Stm *stm : *(node->sl)) {
      if (stm != nullptr) {
        stm->accept(*this);
        if (dynamic_cast<Return *>(stm) != nullptr) {
          hasReturn = true;
          break; // break when meet the first return statement
        }
      }
    }
  }
  if (!hasReturn) {
    returnValue = 0; // reload returnValue
  }
}

void Executor::visit(Assign *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  if (node->left != nullptr && node->exp != nullptr) {
    node->exp->accept(*this);
    int value = returnValue;
    IdExp *idExp = dynamic_cast<IdExp *>(node->left);
    if (idExp != nullptr) {
      variableTable[idExp->id] = value;
    }
  }
}

void Executor::visit(Return *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  if (node->exp != nullptr) {
    node->exp->accept(*this);
  }
}

void Executor::visit(BinaryOp *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  int leftValue = 0, rightValue = 0;
  if (node->left != nullptr) {
    node->left->accept(*this);
    leftValue = returnValue;
  }
  if (node->right != nullptr) {
    node->right->accept(*this);
    rightValue = returnValue;
  }
  if (node->op->op == "+") {
    returnValue = leftValue + rightValue;
  } else if (node->op->op == "-") {
    returnValue = leftValue - rightValue;
  } else if (node->op->op == "*") {
    returnValue = leftValue * rightValue;
  } else if (node->op->op == "/") {
    if (rightValue == 0) {
      throw runtime_error("Error: Division by zero");
    } else {
      returnValue = leftValue / rightValue;
    }
  }
}

void Executor::visit(UnaryOp *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  if (node->exp != nullptr) {
    node->exp->accept(*this);
    if (node->op->op == "-") {
      returnValue = -returnValue;
    }
  }
}

void Executor::visit(Esc *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  if (node->sl != nullptr) {
    for (Stm *stm : *(node->sl)) {
      if (stm != nullptr) {
        stm->accept(*this);
      }
    }
  }
  if (node->exp != nullptr) {
    node->exp->accept(*this);
  }
}

void Executor::visit(IdExp *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  if (variableTable.find(node->id) != variableTable.end()) {
    returnValue = variableTable[node->id];
  } else {
    cerr << "Error: Undefined variable " << node->id << endl;
    returnValue = 0;
  }
}

void Executor::visit(OpExp *node) {
  // OpExp nodes are not directly executable, they are part of BinaryOp or UnaryOp
  newNode = (node == nullptr) ? nullptr : static_cast<OpExp *>(node->clone());
}

void Executor::visit(IntExp *node) {
  if (node == nullptr) {
    newNode = nullptr;
    return;
  }
  returnValue = node->val;
}
