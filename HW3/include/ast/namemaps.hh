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
    bool is_class(string class_name);
    bool add_class(string class_name); //return false if already exists
    bool add_class_hiearchy(string class_name, string parent_name); //return false if loop found
    set<string> get_ancestors(string class_name);
    bool is_method(string class_name, string method_name); 
    bool add_method(string class_name, string method_name);  //return false if already exists
    //MethodDecl* get_method(string method_name, string class_name);
    bool is_class_var(string class_name, string var_name);
    bool add_class_var(string class_name, string var_name, VarDecl* vd);
    VarDecl* get_class_var(string class_name, string var_name);
    bool is_method_var(string class_name, string method_name, string var_name);
    bool add_method_var(string class_name, string method_name, string var_name, VarDecl* vd);
    VarDecl* get_method_var(string class_name, string method_name, string var_name);
    bool is_method_formal(string class_name, string method_name, string var_name);
    bool add_method_formal(string class_name, string method_name, string var_name, Formal* f);
    Formal* get_method_formal(string class_name, string method_name, string var_name);
    bool add_method_formal_list(string class_name, string method_name, vector<string> vl);
    vector<Formal*>* get_method_formal_list(string class_name, string method_name);

    Formal* get_method_return_type(string class_name, string method_name) {
        return get_method_formal(class_name, method_name, "^_method_return");
    }
    set<string> get_all_classes();
    void print();
    
};

//this visitor is to set up the name maps for the program

class AST_Name_Map_Visitor : public AST_Visitor {
private:
    Name_Maps *name_maps; //this is the map for all names in the program
    //you are allowed to add other members here
public:
    AST_Name_Map_Visitor() {
        name_maps = new Name_Maps();
    }
    Name_Maps* getNameMaps() { return name_maps; }

    void visit(Program* node) override;
    void visit(MainMethod* node) override;
    void visit(ClassDecl* node) override;
    void visit(Type *node) override;
    void visit(VarDecl* node) override;
    void visit(MethodDecl* node) override;
    void visit(Formal* node) override;
    void visit(Nested* node) override;
    void visit(If* node) override;
    void visit(While* node) override;
    void visit(Assign* node) override;
    void visit(CallStm* node) override;
    void visit(Continue* node) override;
    void visit(Break* node) override;
    void visit(Return* node) override;
    void visit(PutInt* node) override;
    void visit(PutCh* node) override;
    void visit(PutArray* node) override;
    void visit(Starttime* node) override;
    void visit(Stoptime* node) override;
    void visit(BinaryOp* node) override;
    void visit(UnaryOp* node) override;
    void visit(ArrayExp* node) override;
    void visit(CallExp* node) override;
    void visit(ClassVar* node) override;
    void visit(BoolExp* node) override;
    void visit(This* node) override;
    void visit(Length* node) override;
    void visit(Esc* node) override;
    void visit(GetInt* node) override;
    void visit(GetCh* node) override;
    void visit(GetArray* node) override;
    void visit(IdExp* node) override;
    void visit(OpExp* node) override;
    void visit(IntExp* node) override;
};

Name_Maps* makeNameMaps(Program* node);

#endif