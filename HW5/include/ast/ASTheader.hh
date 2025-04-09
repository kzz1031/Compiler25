#ifndef _ASTHEADER_HH
#define _ASTHEADER_HH
#include <iostream>
#include <fstream>
#include <string>
#include <variant>
#include <vector>
#include <memory>

namespace fdmj {

    class Pos; //members: sline, scolumn, eline, ecolumn
    class AST_Visitor; // Abstract Syntax Tree Visitor (with a templated return type). 

// Abstract Syntax Tree. Root is AST, which is the base class for all nodes in the AST.
// All have a member: pos, which is of type Pos for position info
    class AST; // AST is a base class for all nodes in the AST
    class Program; // members: main, cdl
    class MainMethod; // members: vdl, sl
    class ClassDecl; // members: id, eid, vdl, mdl
    class Type; //members: typeKind, cid, arity. including all types: class, int, array with and without arity. 
    class VarDecl; // members: type, id, init (a variant). Including all types: class, int, array with and without arity
    class MethodDecl; // members: type, id, fl, vdl, sl. Including all types: class, int, array with and without arity
    class Formal; //members: type, id
    class Stm; //  Stm is a super class for all statements
    class Nested; //members: stamentList
    class If; //members: exp and (then) stm1 and (else) stm2 (stm2 optional)
    class While; //members: exp and stm (stm optional)
    class Assign; //members: lexp, rexp
    class CallStm; //members: obj, id, parameters
    class Continue; //no member
    class Break; //no member
    class Return; //member: exp
    class PutInt; //member: exp
    class PutCh; //member: exp
    class PutArray; //member: n (how many to put), arr (array)
    class Starttime; //no member
    class Stoptime; //no member
    class Exp; // Exp is a class for all expressions
    class BinaryOp; //l, op, r
    class UnaryOp; //op, exp
    class ArrayExp; //arr, index
    class CallExp; //obj, id, parameters
    class ClassVar; //obj, id
    class BoolExp; //val: true/false
    class This; //no member
    class Length; //exp
    class Esc; // statementlist, exp
    class GetInt; //no member
    class GetCh; //no member
    class GetArray; //arr
    class IdExp; //val
    class IntExp; //val
    class OpExp; //val

//Program *fdmjParser(const std::string &filename, const bool debug);
//Program *fdmjParser(std::ifstream &fp, const bool debug);

class Pos {
  public:
    size_t sline=0, scolumn=0, eline=0, ecolumn=0; //start and end line and column 
    Pos(size_t sline, size_t scolumn, size_t eline, size_t ecolumn) : 
        sline(sline), scolumn(scolumn), eline(eline), ecolumn(ecolumn) {}
    Pos* clone() {return new Pos(sline, scolumn, eline, ecolumn);}
    std::string print() {
        return "Position(sline: " + std::to_string(sline) + ", scolumn: " +
            std::to_string(scolumn) + ", eline: " + std::to_string(eline) + 
            ", ecolumn: " + std::to_string(ecolumn) + ")";
    }
};

enum class ASTKind; //forwards declaration 

class AST {
    public:
        ~AST() { delete pos; }
        AST(Pos *pos) : pos(pos) { }
        Pos* getPos() {return pos;}
        virtual void accept(AST_Visitor& v) = 0;
        virtual ASTKind getASTKind()=0;
        virtual AST* clone() = 0;
    protected:
        Pos* pos=nullptr;
};

class AST_Visitor {
    public:
    virtual void visit(Program* node) = 0;
    virtual void visit(MainMethod* node) = 0;
    virtual void visit(ClassDecl* node) = 0;
    virtual void visit(Type* node) = 0;
    virtual void visit(VarDecl* node) = 0;
    virtual void visit(MethodDecl* node) = 0;
    virtual void visit(Formal* node) = 0;
    virtual void visit(Nested* node) = 0;
    virtual void visit(If* node) = 0;
    virtual void visit(While* node) = 0;
    virtual void visit(Assign* node) = 0;
    virtual void visit(CallStm* node) = 0;
    virtual void visit(Continue* node) = 0;
    virtual void visit(Break* node) = 0;
    virtual void visit(Return* node) = 0;
    virtual void visit(PutInt* node) = 0;
    virtual void visit(PutCh* node) = 0;
    virtual void visit(PutArray* node) = 0;
    virtual void visit(Starttime* node) = 0;
    virtual void visit(Stoptime* node) = 0;
    virtual void visit(BinaryOp* node) = 0;
    virtual void visit(UnaryOp* node) = 0;
    virtual void visit(ArrayExp* node) = 0;
    virtual void visit(CallExp* node) = 0;
    virtual void visit(ClassVar* node) = 0;
    virtual void visit(BoolExp* node) = 0;
    virtual void visit(This* node) = 0;
    virtual void visit(Length* node) = 0;
    virtual void visit(Esc* node) = 0;
    virtual void visit(GetInt* node) = 0;
    virtual void visit(GetCh* node) = 0;
    virtual void visit(GetArray* node) = 0;
    virtual void visit(IdExp* node) = 0;
    virtual void visit(OpExp* node) = 0;
    virtual void visit(IntExp* node) = 0;
};

enum class ASTKind {
    Program = 0,
    MainMethod = 1,
    ClassDecl = 2,
    Type = 3,
    VarDecl = 4,
    MethodDecl = 5,
    Formal = 6,
    Nested = 7,
    If = 8,
    While = 9,
    Assign = 10,
    CallStm = 11,
    Continue = 12,
    Break = 13,
    Return = 14,
    PutInt = 15,
    PutCh = 16,
    PutArray = 17,
    Starttime = 18,
    Stoptime = 19,
    BinaryOp = 20,
    UnaryOp = 21,
    ArrayExp = 22,
    CallExp = 23,
    ClassVar = 24,
    BoolExp = 25,
    This = 26, 
    Length = 27,
    Esc = 28,
    GetInt = 29,
    GetCh = 30,
    GetArray = 31,
    IdExp= 32,
    OpExp= 33,
    IntExp = 34,
};    //Two basic types (string, integer, with position information)

enum class TypeKind {
    CLASS = 0,
    INT = 1,
    ARRAY = 2
};

// some helper functions for AST
fdmj::Program* fdmjParser(const std::string &filename, const bool debug);
fdmj::Program* fdmjParser(std::ifstream &fp, const bool debug);
std::string ASTKind_string(fdmj::ASTKind k);
std::string type_kind_string(fdmj::TypeKind typekind);
template<class T> std::vector<T*>* cloneList(std::vector<T*>* tl);

} // namespace fdmj

#endif