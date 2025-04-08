#ifndef _AST2TREE_HH
#define _AST2TREE_HH

#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "semant.hh"
#include "temp.hh"
#include "treep.hh"
#include "tr_exp.hh"
#include "config.hh"

using namespace std;
//using namespace fdmj;
//using namespace tree;

//forward declaration
class Class_table;
class Method_var_table;
class Patch_list;
class ASTToTreeVisitor;

tree::Program* ast2tree(fdmj::Program* prog, AST_Semant_Map* semant_map);
Class_table* generate_class_table(AST_Semant_Map* semant_map);
Method_var_table* generate_method_var_table(string class_name, string method_name, Name_Maps* nm, Temp_map* tm);

//class table is to map each class var and method to an address offset
//this uses a "universal class" method, i.e., all the classes use the 
//same class table, with all the possible vars and methods of all classes
//listed in the same record layout.
class Class_table {
public:
     map<string, int> var_pos_map;
     map<string, int> method_pos_map;
     Class_table() {
          var_pos_map = map<string, int>();
          method_pos_map = map<string, int>();
     };
     ~Class_table() {
          var_pos_map.clear();
          method_pos_map.clear();
     }
     int get_var_pos(string var_name) {
          return var_pos_map[var_name];
     }
     int get_method_pos(string method_name) {
          return method_pos_map[method_name];
     }
};

//For each method, there is a var table, including formal and local var.
//(if a method local has a conflict in var name with formal, then local var
//is used (ignore the formal))
//Hence, local var will override formal in the Method_var_table.
//Note: each local var and formal has a type as well (INT or PTR)
//The return of a method is also taken as a formal, with a special name _^return^_method_name.
class Method_var_table {
public:
     map<string, tree::Temp*> *var_temp_map;
     map<string, tree::Type> *var_type_map;
     Method_var_table() {
          var_temp_map = new map<string, tree::Temp*>();
          var_type_map = new map<string, tree::Type>();
     };
     tree::Temp* get_var_temp(string var_name) {
          return var_temp_map->at(var_name);
     }
     tree::Type get_var_type(string var_name) {
          return var_type_map->at(var_name);
     }
};

class ASTToTreeVisitor : public fdmj::AST_Visitor {
public:
     tree::Tree *visit_tree_result = nullptr;
     //** Here add some "visitor-level members" */
     Tr_Exp* tr_exp = nullptr;
     vector<pair<tree::Label*, tree::Label*>> while_labels;
     
     ~ASTToTreeVisitor() { }

     ASTToTreeVisitor() { }

     tree::Tree* getTree() { return visit_tree_result; } //return the tree from a single visit (program returns a single tree)
     /*
     T_tree* getTree() { return visit_result_node; }
     Temp_map* getTempMap() { return visitor_temp_map; }
     map<string, Temp_temp*>* getVariableMap() { return visitor_variable_map; }
     */ 
     void visit(fdmj::Program* node) override;
     void visit(fdmj::MainMethod* node) override;
     void visit(fdmj::ClassDecl* node) override;
     void visit(fdmj::Type* node) override;
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

#endif
