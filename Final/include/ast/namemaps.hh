#ifndef __AST_NAMEMAPS_HH__
#define __AST_NAMEMAPS_HH__

#include <map>
#include <set>
#include "ASTheader.hh"
#include "FDMJAST.hh"

using namespace std;
using namespace fdmj;

//this is for the maps of various names (things) in a program and their relationships
class Name_Maps {
private:
    //build all name maps to facilitate the easy access of the declarations of all names
    set<string> classes; //set of all class names
    map<string, string> classHierachy; //map class name to its direct ancestor class name
    set<pair<string, string>> methods; //set of class name and method name pairs
    map<pair<string, string>, VarDecl*> classVar; //map classname+varname to its declaration AST node (Type*)
    map<tuple<string, string, string>, VarDecl*> methodVar; //map classname + methodname + varname to VarDecl*
    map<tuple<string, string, string>, Formal*> methodFormal; //map classname+methodname+varname to Formal*
    map<pair<string, string>, vector<string>> methodFormalList; //map classname+methodname to formallist of vars
                        //The last is for the return type (pretending to be a Formal)
public:
    bool passed_name_maps = true; //to report if there is any error in setting up the name maps

    bool is_class(string class_name);
    bool add_class(string class_name); //return false if already exists
    set<string>* get_class_list();

    bool detected_loop(map<string, string> classHierachy);
    bool add_class_hiearchy(string class_name, string parent_name); //return false if loop found
    vector<string>* get_ancestors(string class_name);

    bool is_method(string class_name, string method_name); 
    bool add_method(string class_name, string method_name);  //return false if already exists
    set<string>* get_method_list(string class_name);

    bool is_class_var(string class_name, string var_name);
    bool add_class_var(string class_name, string var_name, VarDecl* vd);
    VarDecl* get_class_var(string class_name, string var_name);
    set<string>* get_class_var_list(string class_name);

    bool is_method_var(string class_name, string method_name, string var_name);
    bool add_method_var(string class_name, string method_name, string var_name, VarDecl* vd);
    VarDecl* get_method_var(string class_name, string method_name, string var_name);
    set<string>* get_method_var_list(string class_name, string method_name);

    bool is_method_formal(string class_name, string method_name, string var_name);
    bool add_method_formal(string class_name, string method_name, string var_name, Formal* f);
    Formal* get_method_formal(string class_name, string method_name, string var_name);
    bool add_method_formal_list(string class_name, string method_name, vector<string> vl);
    vector<Formal*>* get_method_formal_list(string class_name, string method_name);

    vector<string>* get_all_classes();
    void print();
};

class AST_Name_Map_Visitor : public AST_Visitor {
private:
    Name_Maps *name_maps; //this is the map for all names in the program
    //you are allowed to add other members here
public:
    AST_Name_Map_Visitor() {
        name_maps = new Name_Maps();
    }
    Name_Maps* getNameMaps() { return name_maps; }

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

#endif