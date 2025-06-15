#ifndef _AST2XML_HH
//#define _AST2XML_HH

#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "tinyxml2.hh"
#include "semant.hh"

using namespace std;
using namespace fdmj;
using namespace tinyxml2;

XMLDocument* ast2xml(fdmj::Program *node, AST_Semant_Map *semant_map, bool location_flag, bool semant_flag);

class AST2XML : public fdmj::AST_Visitor {
public:
  XMLDocument *doc; //XMLDocument to store the AST
  XMLElement *el; //temp to remember the results during the AST is recursively visited. 
  AST_Semant_Map *semant_map; //store the semantic information of the nodes

public:
  void visit(fdmj::Program *node) override;
  void visit(fdmj::MainMethod *node) override;
  void visit(fdmj::ClassDecl *node) override;
  void visit(fdmj::Type *node) override;
  void visit(fdmj::VarDecl *node) override;
  void visit(fdmj::MethodDecl *node) override;
  void visit(fdmj::Formal *node) override;
  void visit(fdmj::Nested *node) override;
  void visit(fdmj::If *node) override;
  void visit(fdmj::While *node) override;
  void visit(fdmj::Assign *node) override;
  void visit(fdmj::CallStm *node) override;
  void visit(fdmj::Continue *node) override;
  void visit(fdmj::Break *node) override;
  void visit(fdmj::Return *node) override;
  void visit(fdmj::PutInt *node) override;
  void visit(fdmj::PutCh *node) override;
  void visit(fdmj::PutArray *node) override;
  void visit(fdmj::Starttime *node) override;
  void visit(fdmj::Stoptime *node) override;
  void visit(fdmj::BinaryOp *node) override;
  void visit(fdmj::UnaryOp *node) override;
  void visit(fdmj::ArrayExp *node) override;
  void visit(fdmj::CallExp *node) override;
  void visit(fdmj::ClassVar *node) override;
  void visit(fdmj::BoolExp *node) override;
  void visit(fdmj::This *node) override;
  void visit(fdmj::Length *node) override;
  void visit(fdmj::Esc *node) override;
  void visit(fdmj::GetInt *node) override;
  void visit(fdmj::GetCh *node) override;
  void visit(fdmj::GetArray *node) override;
  void visit(fdmj::IdExp *node) override;
  void visit(fdmj::OpExp *node) override;
  void visit(fdmj::IntExp *node) override;
};

#endif