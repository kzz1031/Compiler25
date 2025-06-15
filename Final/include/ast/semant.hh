#ifndef __AST_SEMANT_HH__
#define __AST_SEMANT_HH__

#include <map>
#include <set>
#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "namemaps.hh"

using namespace std;
using namespace fdmj;

//this is for denote the semantic information of an AST node
//mostly for expressions and variables.
//Useful for type checking and IR (tree) generation
class AST_Semant {
public:
    enum Kind {Value, MethodName, ClassName};
    //Value - a value, has a typeKind (any calculation result)
    //MethodName - a method name, has a typeKind (need: class id)
    //ClassName - a class name, has a typeKind (need class id)
    //ClassVarName - a class variable name (type determined by class)
private:
    Kind s_kind;
    TypeKind typeKind; //enum class TypeKind {CLASS/OBJECT = 0, INT = 1, ARRAY = 2};
    variant<monostate, string, int> type_par; //string for class name, int for array arity
    bool lvalue; //if the expression is an lvalue
public:
    AST_Semant(AST_Semant::Kind s_kind, TypeKind typeKind, variant<monostate, string, int> type_par, bool lvalue) :
            s_kind(s_kind), typeKind(typeKind), type_par(type_par), lvalue(lvalue) {}
    Kind get_kind() { return s_kind; }
    TypeKind get_type() { return typeKind; }
    variant<monostate, string, int> get_type_par() { return type_par; }
    bool is_lvalue() { return lvalue; }
    static string s_kind_string(Kind s_kind) {
        switch (s_kind) {
            case AST_Semant::Kind::Value:
                return "Value";
            case AST_Semant::Kind::MethodName:
                return "MethodName";
            case AST_Semant::Kind::ClassName:
                return "ClassName";
            default:
                return "Unknown";
        }
    }
};

//this is to map an AST node to its semantic information
class AST_Semant_Map {
private:
    Name_Maps *name_maps;
    map<AST*, AST_Semant*> semant_map;
public:
    AST_Semant_Map() {
        semant_map = map<AST*, AST_Semant*>();
    }
    ~AST_Semant_Map() {
        semant_map.clear();
    }

    void setNameMaps(Name_Maps* nm) { name_maps = nm; }
    Name_Maps* getNameMaps() { return name_maps; }

    AST_Semant* getSemant(AST *node) {
        if (node == nullptr) {
            return nullptr;
        } 
        if (semant_map.find(node) == semant_map.end()) {
            return nullptr;
        }
        return semant_map[node];
    }
    void setSemant(AST *node, AST_Semant* semant) {
        if (node == nullptr) {
            cerr << "Error: setting semantic information for a null node" << endl;
            return;
        }
        semant_map[node] = semant;
    }
};

class AST_Semant_Visitor : public AST_Visitor {
private:
    AST_Semant_Map *semant_map; //this is the semantic information map for all (sub)expressions
    Name_Maps* const name_maps; //this is the map for all names in the program
    //you may add other members here 
public:
    //Change this constructor if more members are added above (if necessary)
    AST_Semant_Visitor(Name_Maps* name_maps) : name_maps(name_maps) {
        semant_map = new AST_Semant_Map();
    }
    AST_Semant_Map* getSemantMap() { return semant_map; }

    void visit(fdmj::Program* node) override;
    void visit(fdmj::MainMethod* node) override;
    void visit(fdmj::ClassDecl* node) override;
    void visit(fdmj::Type *node) override;
    void visit(fdmj::VarDecl* node) override;
    void visit(fdmj::MethodDecl* node) override;
    void visit(fdmj::Formal* node) override;
    void visit(fdmj::Nested* node) override;
    void visit(fdmj::If* node) override;
    void visit(fdmj::While* node) override;
    void visit(fdmj::Assign* node) override;
    void visit(fdmj::CallStm* node) override;
    void visit(fdmj::Continue* node) override;
    void visit(fdmj::Break* node) override;
    void visit(fdmj::Return* node) override;
    void visit(fdmj::PutInt* node) override;
    void visit(fdmj::PutCh* node) override;
    void visit(fdmj::PutArray* node) override;
    void visit(fdmj::Starttime* node) override;
    void visit(fdmj::Stoptime* node) override;
    void visit(fdmj::BinaryOp* node) override;
    void visit(fdmj::UnaryOp* node) override;
    void visit(fdmj::ArrayExp* node) override;
    void visit(fdmj::CallExp* node) override;
    void visit(fdmj::ClassVar* node) override;
    void visit(fdmj::BoolExp* node) override;
    void visit(fdmj::This* node) override;
    void visit(fdmj::Length* node) override;
    void visit(fdmj::Esc* node) override;
    void visit(fdmj::GetInt* node) override;
    void visit(fdmj::GetCh* node) override;
    void visit(fdmj::GetArray* node) override;
    void visit(fdmj::IdExp* node) override;
    void visit(fdmj::OpExp* node) override;
    void visit(fdmj::IntExp* node) override;
};

Name_Maps* makeNameMaps(fdmj::Program* node);
AST_Semant_Map* semant_analyze(fdmj::Program* node);

#endif