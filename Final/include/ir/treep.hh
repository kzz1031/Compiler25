#ifndef _TREEP
#define _TREEP

#include <vector>
#include <cstdio>
#include <string>
#include "temp.hh"

using namespace std;
using namespace tree;

namespace tree {

//forward declaration
class Tree; //abstract class
class Program; //member: funcdecllist
class FuncDecl; //members: name, args, stm
class Block; //members: name, args, stm
class Stm; //abstract class
class Jump; //members: label
class Cjump; //members: relop, left, right, t, f
class Move; //members: dst, src
class Seq; //members: stmlist
class LabelStm; //members: label
class Return; //members: exp
class Phi; //members: temp, args
class ExpStm; //members: exp
class Exp; //abstract class
class Binop; //members: op, left, right
class Mem; //members: mem
class TempExp; //members: temp
class Eseq; //members: stm, exp
class Name; //members: name
class Const; //members: constVal
class Call; //members: id, obj, args
class ExtCall; //members: extfun, args

enum class Type {INT, PTR};

string typeToString(Type type); 

//possible binary ops: +, -, *. /, &&, ||, xor 
//note we don't need unary ops: "!" (not) is "1 xor", "-" (negate) is "0 -")
//possible relop:  ==, !=, <, >, <=, >=

class Visitor {
  public:
    virtual void visit(Program *prog) = 0;
    virtual void visit(FuncDecl *func) = 0;
    virtual void visit(Block *block) = 0;
    virtual void visit(Jump *jump) = 0;
    virtual void visit(Cjump *cjump) = 0;
    virtual void visit(Move *move) = 0;
    virtual void visit(Seq *seq) = 0;
    virtual void visit(LabelStm *labelstm) = 0;
    virtual void visit(Return *ret) = 0;
    virtual void visit(Phi *phi) = 0;
    virtual void visit(ExpStm *exp) = 0;
    virtual void visit(Binop *binop) = 0;
    virtual void visit(Mem *mem) = 0;
    virtual void visit(TempExp *tempexp) = 0;
    virtual void visit(Eseq *eseq) = 0;
    virtual void visit(Name *name) = 0;
    virtual void visit(Const *const) = 0;
    virtual void visit(Call *call) = 0;
    virtual void visit(ExtCall *extcall) = 0;
};

enum class Kind {
  PROGRAM, 
  FUNCDECL, 
  BLOCK, 
  JUMP, 
  CJUMP, 
  MOVE, 
  SEQ, 
  LABELSTM, 
  RETURN, 
  PHI, 
  EXPSTM, 
  BINOP, 
  MEM, 
  TEMPEXP, 
  ESEQ, 
  NAME, 
  CONST, 
  CALL, 
  EXTCALL
};

//forward declaration (declaration in treep.cc)
string kindToString(Kind kind);

class Tree {
  public:
    ~Tree() {}
    Tree() {}
    virtual Kind getTreeKind() = 0;
    virtual void accept(Visitor &v) = 0;
}; 

class Program : public Tree {
public:
  std::vector<tree::FuncDecl*> *funcdecllist;
  Program(std::vector<tree::FuncDecl*> *funcdecllist) : funcdecllist(funcdecllist) {}
  Kind getTreeKind() { return Kind::PROGRAM; }
  void accept(Visitor &v) { v.visit(this); }
};

class Block : public Tree {
public:
  Label *entry_label;
  std::vector<tree::Label*> *exit_labels;
  std::vector<tree::Stm*> *sl;
  Block(Label *entry_label, std::vector<tree::Label*> *exit_labels, std::vector<tree::Stm*> *sl) : 
      entry_label(entry_label), exit_labels(exit_labels), sl(sl) {}
  Kind getTreeKind() { return Kind::BLOCK; }
  void accept(Visitor &v) { v.visit(this); }
};

class FuncDecl : public Tree {
public:
  string name; //function name (unique name: classname + methodname)
  std::vector<tree::Temp*> *args; //arguments: the first argument is the object pointer (this)
  std::vector<tree::Block*> *blocks; //first block is the entry block (the first label of block[0] is the entry label)
  Type return_type;
  int last_temp_num; //last temp number used in the function
  int last_label_num; //last label number used in the function
  FuncDecl(string name, std::vector<tree::Temp*> *args, std::vector<tree::Block*> *blocks, Type return_type, int lt, int ll) : 
      name(name), args(args), blocks(blocks), return_type(return_type), last_temp_num(lt), last_label_num(ll) {}
  Kind getTreeKind() { return Kind::FUNCDECL; }
  void accept(Visitor &v)  { v.visit(this); }
};

class Stm : public Tree {
public:
  ~Stm() {}
  Stm() {}
  virtual void accept(Visitor &v) = 0;
};

class Seq : public Stm {
  public:
    std::vector<tree::Stm*> *sl = nullptr;
    Seq(std::vector<tree::Stm*> *sl): sl(sl) {}
    Seq() {sl=nullptr;} //if nothing, make sl nullptr
    Kind getTreeKind() { return Kind::SEQ; }
    void accept(Visitor &v) { v.visit(this); }
};

class LabelStm : public Stm {
  public:
    Label *label;
    LabelStm(Label *label) : label(label) {}
    Kind getTreeKind() { return Kind::LABELSTM; }
    void accept(Visitor &v) { v.visit(this); }
};

class Jump : public Stm {
  public:
    Label* label;
    Jump(Label *label) : label(label) {}
    Kind getTreeKind() { return Kind::JUMP; }
    void accept(Visitor &v) { v.visit(this); }
};

class Cjump : public Stm {
  public:
    string relop;
    Exp *left;
    Exp *right; 
    Label *t, *f;
    Cjump(string relop, Exp *left, Exp *right, Label *t, Label *f) :
      relop(relop), left(left), right(right), t(t), f(f) {}
    Kind getTreeKind() { return Kind::CJUMP; }
    void accept(Visitor &v) { v.visit(this); }
};

class Move : public Stm {
  public:
    Exp *dst;
    Exp *src;
    Move(Exp *dst, Exp *src) : dst(dst), src(src) {}
    Kind getTreeKind() { return Kind::MOVE; }
    void accept(Visitor &v) { v.visit(this); }
};

class Phi : public Stm {
  public:
    Temp *temp;
    vector<pair<tree::Temp*, tree::Label*>> *args; 
        //Label is the label of the block where the value is defined
    Phi(Temp *temp, vector<pair<tree::Temp*, tree::Label*>> *args) :
      temp(temp), args(args) {}
    Kind getTreeKind() { return Kind::PHI; }
    void accept(Visitor &v) { v.visit(this); }
};

class ExpStm : public Stm { //an expression with result ignored (only keep the side effects)
  public:
    Exp *exp;
    ExpStm(Exp *exp) : exp(exp) {}
    Kind getTreeKind() { return Kind::EXPSTM; }
    void  accept(Visitor &v) { v.visit(this); }
};

class Return : public Stm {
  public:
    Exp *exp;
    Return(Exp *exp) : exp(exp) {}
    Kind getTreeKind() { return Kind::RETURN; }
    void accept(Visitor &v) { v.visit(this); }
};

class Exp : public Tree {
  public:
    ~Exp() {}
    Exp() {}
    Type type; //each expression has a type (int or pointer)
    Exp(Type type) : type(type) {}
    virtual void accept(Visitor &v) = 0;
};

class Binop : public Exp {
  public:
    string op;
    Exp *left;
    Exp *right;
    Binop(Type t, string op, Exp *left, Exp *right) : Exp(t), op(op), left(left), right(right) {}
    Kind getTreeKind() { return Kind::BINOP; }
    void accept(Visitor &v) { v.visit(this); }
}; 

class Mem : public Exp {
  public:
    Exp *mem;
    Mem(Type t, Exp *mem) : Exp(t), mem(mem) {} 
    Kind getTreeKind() { return Kind::MEM; }
    void accept(Visitor &v) { v.visit(this); }
}; 

class TempExp : public Exp {
  public:
    Temp *temp;
    TempExp(Type t, Temp *temp) : Exp(t), temp(temp) {}
    Kind getTreeKind() { return Kind::TEMPEXP; }
    void accept(Visitor &v) { v.visit(this); }
};

class Eseq : public Exp {
  public:
    Stm *stm;
    Exp *exp;
    Eseq(Type t, Stm *stm, Exp *exp) : Exp(t), stm(stm), exp(exp) {}
    Kind getTreeKind() { return Kind::ESEQ; }
    void accept(Visitor &v) { v.visit(this); }
};

class Name : public Exp { //convert a label to a ptr (address)
  public:
    Label *name;
    String_Label *sname;
    Name(Label *name) : Exp(Type::PTR), name(name), sname(nullptr) {}
    Name(String_Label *sname) : Exp(Type::PTR), name(nullptr), sname(sname) {}
    Kind getTreeKind() { return Kind::NAME; }
    void accept(Visitor &v) { v.visit(this); }
};

class Const : public Exp {
  public:
    int constVal; //we only support integer constants 
    Const(int constVal) : constVal(constVal) {type = Type::INT;}
    Kind getTreeKind() { return Kind::CONST; }
    void accept(Visitor &v) { v.visit(this); }
};

class Call : public Exp {
  public:
    string id;
    Exp *obj;
    std::vector<tree::Exp*> *args;
    Call(tree::Type t, string id, tree::Exp *obj, std::vector<tree::Exp*> *args) : 
      Exp(t), id(id), obj(obj), args(args) {}
    Kind getTreeKind() { return Kind::CALL; }
    void accept(Visitor &v) { v.visit(this); }
};

class ExtCall : public Exp {
  public:
    string extfun;
    std::vector<tree::Exp*> *args;
    ExtCall(tree::Type t, string extfun, std::vector<tree::Exp*> *args) : 
      Exp(t), extfun(extfun), args(args) {}
    Kind getTreeKind() { return Kind::EXTCALL; }
    void accept(Visitor &v) { v.visit(this); }
};

} //namespace tree

#endif
