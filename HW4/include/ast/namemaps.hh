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
    set<string>* get_ancestors(string class_name);

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
    vector<string>* get_method_formal_list(string class_name, string method_name);

    void print();
};

#endif