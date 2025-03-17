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

#endif