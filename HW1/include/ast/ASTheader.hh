#ifndef _ASTHEADER_HH
#define _ASTHEADER_HH
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace fdmj {

class Pos;        // members: sline, scolumn, eline, ecolumn
class ASTVisitor; // Abstract Syntax Tree Visitor (with a templated return
                  // type).

// Abstract Syntax Tree. Root is AST, which is the base class for all nodes in
// the AST. All have a member: pos, which is of type Pos for position info
class AST;     // AST is a base class for all nodes in the AST
class Program; // members: main, cdl
class MainMethod;
class Stm;      //  Stm is a super class for all statements
class Assign;   // members: lexp, rexp
class Return;   // member: exp
class Exp;      // Exp is a class for all expressions
class BinaryOp; // l, op, r
class UnaryOp;  // op, exp
class Esc;      // statementlist, exp
class IdExp;    // val
class IntExp;   // val
class OpExp;    // val

// Program *fdmjParser(const std::string &filename, const bool debug);
// Program *fdmjParser(std::ifstream &fp, const bool debug);

class Pos {
public:
  size_t sline = 0, scolumn = 0, eline = 0,
         ecolumn = 0; // start and end line and column
  Pos(size_t sline, size_t scolumn, size_t eline, size_t ecolumn)
      : sline(sline), scolumn(scolumn), eline(eline), ecolumn(ecolumn) {}
  Pos *clone() { return new Pos(sline, scolumn, eline, ecolumn); }
};

enum class ASTKind; // forwards declaration

class AST {
public:
  ~AST() { delete pos; }
  AST(Pos *pos) : pos(pos) {}
  Pos *getPos() { return pos; }
  virtual void accept(ASTVisitor &v) = 0;
  virtual ASTKind getASTKind() = 0;
  virtual AST *clone() = 0;

protected:
  Pos *pos = nullptr;
};

class ASTVisitor {
public:
  virtual void visit(Program *node) = 0;
  virtual void visit(MainMethod *node) = 0;
  virtual void visit(Assign *node) = 0;
  virtual void visit(Return *node) = 0;
  virtual void visit(BinaryOp *node) = 0;
  virtual void visit(UnaryOp *node) = 0;
  virtual void visit(Esc *node) = 0;
  virtual void visit(IdExp *node) = 0;
  virtual void visit(OpExp *node) = 0;
  virtual void visit(IntExp *node) = 0;
};

enum class ASTKind {
  Program,
  MainMethod,
  Assign,
  Return,
  BinaryOp,
  UnaryOp,
  Esc,
  IdExp,
  OpExp,
  IntExp,
}; // Two basic types (string, integer, with position information)

// some helper functions for AST
Program *fdmjParser(const std::string &filename, const bool debug);
Program *fdmjParser(std::ifstream &fp, const bool debug);
std::string stringASTKind(ASTKind k);
template <class T> std::vector<T *> *cloneList(std::vector<T *> *tl);

} // namespace fdmj

#endif