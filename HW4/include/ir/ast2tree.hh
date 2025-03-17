#ifndef _AST2TREE_HH
#define _AST2TREE_HH

#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "semant.hh"
#include "temp.hh"
#include "treep.hh"

using namespace std;
//using namespace fdmj;
//using namespace tree;

#define ClassDeclList vector<fdmj::ClassDecl*>
#define VarDeclList vector<fdmj::VarDecl*>

class Class_table;
class Var_table;
class Patch_list;

tree::Program* ast2tree(fdmj::Program* prog, AST_Semant_Map* semant_map);
Class_table* generate_class_table(ClassDeclList* cdl);
Var_table* generate_var_table(VarDeclList* vdl);

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

class Var_table {
public:
     map<string, tree::Temp*> *var_temp_map;
     map<string, fdmj::Type*> *var_type_map;
     Var_table() {
          var_temp_map = new map<string, tree::Temp*>();
          var_type_map = new map<string, fdmj::Type*>();
     };
     tree::Temp* get_var_temp(string var_name) {
          return var_temp_map->at(var_name);
     }
     fdmj::Type* get_var_type(string var_name) {
          return var_type_map->at(var_name);
     }
};

class Patch_list {
public:
     vector<tree::Label*> *true_patch_list;
     vector<tree::Label*> *false_patch_list;
     Patch_list() {
          true_patch_list = new vector<tree::Label*>();
          false_patch_list = new vector<tree::Label*>();
     }
     ~Patch_list() {
          delete true_patch_list;
          delete false_patch_list;
     }
     void add_true_patch(tree::Label* label) {
          true_patch_list->push_back(label);
     }
     void add_false_patch(tree::Label* label) {
          false_patch_list->push_back(label);
     }
     void patch_true(tree::Label* label) {
          for (auto l : *true_patch_list) {
               l->num = l->num == -1 ? label->num: l->num;
          }
     }
     void patch_false(tree::Label* label) {
          for (auto l : *false_patch_list) {
               l->num = l->num == -1 ? label->num: l->num;
          }
     }
};

class ASTToTreeVisitor : public fdmj::AST_Visitor {
     tree::Tree* visit_result; //this is to store the result of a visit
     Temp_map *visitor_temp_map;
     Patch_list *visitor_patch_list;
     Var_table *current_variable_map;
     Class_table *current_class_table;
public:
     ~ASTToTreeVisitor() {
          delete visitor_temp_map; 
          delete visitor_patch_list;
          delete current_variable_map;
          delete current_class_table;
     }
     ASTToTreeVisitor() {
          visit_result = nullptr; 
          visitor_temp_map = new Temp_map();
     }

     tree::Tree* getTree() { return visit_result; }
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