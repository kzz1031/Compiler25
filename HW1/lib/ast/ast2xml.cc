// This visitor is used to convert AST to XML
#define DEBUG
#undef DEBUG

#include <iostream>
#include <string>
#include <variant>
#include <vector>

#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "ast2xml.hh"
#include "tinyxml2.hh"

using namespace std;
using namespace fdmj;
using namespace tinyxml2;

static bool _location_flag = true;

XMLDocument *ast2xml(Program *node, bool location_flag) {
  _location_flag = location_flag;
  AST2XML v;
  v.doc = new XMLDocument();
  XMLDeclaration *decl =
      v.doc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
  v.doc->InsertFirstChild(decl);
#ifdef DEBUG
  std::cout << "Start Visitor: " << endl;
#endif
  node->accept(v);
  v.doc->InsertEndChild(v.el);
  return v.doc;
}

static void set_position(XMLElement *el, const Pos *pos) {
  if (pos == nullptr) {
    cerr << "Error: No position found on " << el->Name() << endl;
    return;
  }
  if (!_location_flag)
    return; // do not set position
  if (pos->sline == 0 || pos->scolumn == 0 || pos->eline == 0 ||
      pos->ecolumn == 0) {
    // if any is zero, do not set position
    return;
  }
  el->SetAttribute("bline", to_string(pos->sline).c_str());
  el->SetAttribute("bpos", to_string(pos->scolumn).c_str());
  el->SetAttribute("eline", to_string(pos->eline).c_str());
  el->SetAttribute("epos", to_string(pos->ecolumn).c_str());
}

template <class T>
XMLElement *visitList(XMLDocument *doc, AST2XML &v, vector<T *> *nl,
                      string tag) {
#ifdef DEBUG
  cout << "visitList: tag = " << tag << endl;
#endif
  XMLElement *cn = doc->NewElement(tag.c_str());
  if (nl == nullptr || nl->size() == 0)
    return cn; // empty list
  // set_position(cn, new Pos(0,0,0,0)); //no position for any list, using
  // 0,0,0,0
  for (T *n : *nl) {
    if (n == nullptr) {
      cerr << "Error: Null element in " << tag << endl;
      continue;
    }
    n->accept(v);
    if (v.el != nullptr)
      cn->InsertEndChild(v.el);
  }
  return cn;
}

void AST2XML::visit(Program *node) {
  if (!node)
    return;
#ifdef DEBUG
  cout << "Program" << endl;
#endif
  XMLElement *cn = doc->NewElement("Program");
  set_position(cn, node->getPos());
  if (node->main == nullptr) {
    cerr << "Error: No MainMethod found" << endl;
    el = nullptr;
    return;
  }
  node->main->accept(*this);
  if (el != nullptr)
    cn->InsertEndChild(el); // insert the main method
  el = cn;
}

void AST2XML::visit(MainMethod *node) {
  if (!node)
    return;
#ifdef DEBUG
  cout << "MainMethod" << endl;
#endif
  XMLElement *cn = doc->NewElement("MainMethod");
  set_position(cn, node->getPos());
  XMLElement *cn2 = visitList<Stm>(doc, *this, node->sl, "StmList");
  if (cn2 != nullptr)
    cn->InsertEndChild(cn2); // insert the statement
  el = cn;
}

void AST2XML::visit(Assign *node) {
  if (!node)
    return;
#ifdef DEBUG
  cout << "Assign" << endl;
#endif
  XMLElement *cn = doc->NewElement("Assign");
  set_position(cn, node->getPos());
  if (node->left == nullptr) {
    cerr << "Error: No left expression found in an Assign" << endl;
    el = nullptr;
    return;
  }
  node->left->accept(*this);
  if (el != nullptr)
    cn->InsertEndChild(el); // insert the left expression
  if (node->exp == nullptr) {
    cerr << "Error: No right expression found in an Assign" << endl;
    el = nullptr;
    return;
  }
  node->exp->accept(*this);
  if (el != nullptr)
    cn->InsertEndChild(el); // insert the right expression
  el = cn;
}

void AST2XML::visit(Return *node) {
  if (!node)
    return;
#ifdef DEBUG
  cout << "Return" << endl;
#endif
  XMLElement *cn = doc->NewElement("Return");
  set_position(cn, node->getPos());
  if (node->exp != nullptr) {
    node->exp->accept(*this);
    if (el != nullptr)
      cn->InsertEndChild(el); // insert the expression
  }
  el = cn;
}

void AST2XML::visit(BinaryOp *node) {
  if (!node)
    return;
#ifdef DEBUG
  cout << "BinaryOp" << endl;
#endif
  XMLElement *cn = doc->NewElement("BinaryOp");
  set_position(cn, node->getPos());
  if (node->left == nullptr) {
    cerr << "Error: No left expression found in a BinaryOp" << endl;
    el = nullptr;
    return;
  }
  node->left->accept(*this);
  if (el != nullptr)
    cn->InsertEndChild(el); // insert the left expression
  if (node->op == nullptr) {
    cerr << "Error: No operator found in a BinaryOp" << endl;
    el = nullptr;
    return;
  }
  XMLElement *cn1 = doc->NewElement("OpExp");
  cn1->SetAttribute("op", node->op->op.c_str());
  set_position(cn1, node->op->getPos());
  cn->InsertEndChild(cn1);
  if (node->right == nullptr) {
    cerr << "Error: No right expression found in a BinaryOp" << endl;
    el = nullptr;
    return;
  }
  node->right->accept(*this);
  if (el != nullptr)
    cn->InsertEndChild(el); // insert the right expression
  el = cn;
}

void AST2XML::visit(UnaryOp *node) {
  if (!node)
    return;
#ifdef DEBUG
  cout << "UnaryOp" << endl;
#endif
  XMLElement *cn = doc->NewElement("UnaryOp");
  set_position(cn, node->getPos());
  XMLElement *cn1 = doc->NewElement("OpExp");
  cn1->SetAttribute("op", node->op->op.c_str());
  set_position(cn1, node->op->getPos());
  cn->InsertEndChild(cn1);
  if (node->exp == nullptr) {
    cerr << "Error: No expression found in a UnaryOp" << endl;
    el = nullptr;
    return;
  }
  node->exp->accept(*this);
  if (el != nullptr)
    cn->InsertEndChild(el); // insert the expression
  el = cn;
}

void AST2XML::visit(Esc *node) {
  if (!node)
    return;
#ifdef DEBUG
  cout << "Esc" << endl;
#endif
  XMLElement *cn = doc->NewElement("Esc");
  set_position(cn, node->getPos());
  if (node->sl != nullptr && node->sl->size() > 0) {
    XMLElement *cn1 = visitList<Stm>(doc, *this, node->sl, "StmList");
    if (cn1 != nullptr)
      cn->InsertEndChild(cn1); // insert the statement list
  }
  if (node->exp != nullptr) {
    node->exp->accept(*this);
    if (el != nullptr)
      cn->InsertEndChild(el); // insert the expression
  }
  el = cn;
}

void AST2XML::visit(IdExp *node) {
  if (!node)
    return;
#ifdef DEBUG
  cout << "IdExp" << endl;
#endif
  XMLElement *cn = doc->NewElement("IdExp");
  set_position(cn, node->getPos());
  if (node->id.empty()) {
    cerr << "Error: No id found in an IdExp" << endl;
    el = nullptr;
    return;
  }
  cn->SetAttribute("id", node->id.c_str());
  set_position(cn, node->getPos());
  el = cn;
}

void AST2XML::visit(IntExp *node) {
  if (!node)
    return;
#ifdef DEBUG
  cout << "IntExp" << endl;
#endif
  XMLElement *cn = doc->NewElement("IntExp");
  set_position(cn, node->getPos());
  cn->SetAttribute("val", to_string(node->val).c_str());
  el = cn;
}

void AST2XML::visit(OpExp *node) {
  if (!node)
    return;
#ifdef DEBUG
  cout << "OpExp" << endl;
#endif
  XMLElement *cn = doc->NewElement("OpExp");
  set_position(cn, node->getPos());
  cn->SetAttribute("op", node->op.c_str());
  el = cn;
}