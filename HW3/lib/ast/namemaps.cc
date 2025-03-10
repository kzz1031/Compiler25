#define DEBUG
#undef DEBUG

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include "semant.hh"
#include "namemaps.hh"

using namespace std;
using namespace fdmj;

Name_Maps* makeNameMaps(Program* node) {
    std::cout << "TODO" << std::endl;
    return nullptr;
    // AST_Name_Map_Visitor name_visitor;
    // node->accept(name_visitor);
    // return name_visitor.getNameMaps();
}

bool Name_Maps::is_class(string class_name) {
    return classes.find(class_name) != classes.end();
}

bool Name_Maps::add_class(string class_name) {
    if (Name_Maps::is_class(class_name)) {
        return false;
    }
    classes.insert(class_name);
    return true;
}

bool detected_loop(map<string, string> classHierachy) {
    for (auto it = classHierachy.begin(); it != classHierachy.end(); it++) {
        string class_name = it->first;
        string parent_name = it->second;
        while (true) {
            if (classHierachy.find(parent_name) == classHierachy.end()) {
                break;
            }
            if (classHierachy[parent_name] == class_name) {
                return true;
            }
            parent_name = classHierachy[parent_name];
        }
    }
    return false;
}

bool Name_Maps::add_class_hiearchy(string class_name, string parent_name) {
    if (!Name_Maps::is_class(class_name) || !Name_Maps::is_class(parent_name)) {
        return false;
    }
    classHierachy[class_name] = parent_name;
    if (detected_loop(classHierachy)) {
        classHierachy.erase(class_name);
        return false;
    }
    return true;
}

set<string> Name_Maps::get_ancestors(string class_name) {
    set<string> ancestors;
    if (classHierachy.find(class_name) == classHierachy.end()) {
        return set<string>();
    }
    //below works if no loop
    string parent_name = classHierachy[class_name];
    while (true) {
        ancestors.insert(parent_name);
        if (classHierachy.find(parent_name) == classHierachy.end()) {
            break;
        }
        parent_name = classHierachy[parent_name];
    }
    return ancestors;
}

bool Name_Maps::is_method(string class_name, string method_name) {
    pair<string, string> p(class_name, method_name);
    return methods.find(p) != methods.end();
}

bool Name_Maps::add_method(string class_name, string method_name) {
    if (Name_Maps::is_method(class_name, method_name)) {
        return false;
    }
    methods.insert(pair<string, string>(class_name, method_name));
    return true;
}

bool Name_Maps::is_class_var(string class_name, string var_name) {
    pair<string, string> p(class_name, var_name);
    return classVar.find(p) != classVar.end();
}

bool Name_Maps::add_class_var(string class_name, string var_name, VarDecl* varDecl) {
    pair<string, string> p(class_name, var_name);
    if (Name_Maps::is_class_var(class_name, var_name)) {
        return false;
    }
    classVar[p] = varDecl;
    return true;
}

VarDecl* Name_Maps::get_class_var(string class_name, string var_name) {
    if (!Name_Maps::is_class_var(class_name, var_name)) {
        return nullptr;
    }
    pair<string, string> p(class_name, var_name);
    return classVar[p];
}

bool Name_Maps::is_method_var(string class_name, string method_name, string var_name) {
    tuple<string, string, string> t(class_name, method_name, var_name);
    return methodVar.find(t) != methodVar.end();
}

bool Name_Maps::add_method_var(string class_name, string method_name, string var_name, VarDecl* vd) {
    if (Name_Maps::is_method_var(class_name, method_name, var_name)) {
        return false;
    }
    methodVar[tuple<string, string, string>(class_name, method_name, var_name)] = vd;
    return true;
}

VarDecl* Name_Maps::get_method_var(string class_name, string method_name, string var_name) {
    if (!Name_Maps::is_method_var(class_name, method_name, var_name)) {
        return nullptr;
    }
    return methodVar[tuple<string, string, string>(class_name, method_name, var_name)];
}

bool Name_Maps::is_method_formal(string class_name, string method_name, string var_name) {
    tuple <string, string, string> t(class_name, method_name, var_name);
    return methodFormal.find(t) != methodFormal.end();
}

bool Name_Maps::add_method_formal(string class_name, string method_name, string var_name, Formal* f) {
    if (Name_Maps::is_method_formal(class_name, method_name, var_name)) {
        return false;
    }
    methodFormal[tuple<string, string, string>(class_name, method_name, var_name)] = f;
    return true;
}

Formal* Name_Maps::get_method_formal(string class_name, string method_name, string var_name) {
    if (!Name_Maps::is_method_formal(class_name, method_name, var_name)) {
        return nullptr;
    }
    return methodFormal[tuple<string, string, string>(class_name, method_name, var_name)];
}

bool Name_Maps::add_method_formal_list(string class_name, string method_name, vector<string> vl) {
    if (!Name_Maps::is_method(class_name, method_name)) {
        return false;
    }
    methodFormalList[pair<string, string>(class_name, method_name)] = vl;
    return true;
}

vector<Formal*>* Name_Maps::get_method_formal_list(string class_name, string method_name) {
    vector<Formal*>* fl = new vector<Formal*>();
    pair<string, string> p(class_name, method_name);
    if (methodFormalList.find(p) == methodFormalList.end()) {
        return nullptr;
    }
    vector<string> vl = methodFormalList[pair<string, string>(class_name, method_name)];
    for (auto v : vl) {
        if (Name_Maps::is_method_formal(class_name, method_name, v)) {
            fl->push_back(Name_Maps::get_method_formal(class_name, method_name, v));
        }
        else {
            cerr << "Error: Method Formal: " << class_name << "->" << method_name << "->" << v << " not found" << endl;
            return nullptr;
        }
    }
    return fl;
}

void Name_Maps::print() {
    cout << "Classes: ";
    for (auto c : classes) {
        cout << c << " ; ";
    }
    cout << endl;
    cout << "Class Hiearchy: ";
    for (auto it = classHierachy.begin(); it != classHierachy.end(); it++) {
        cout << it->first << "->" << it->second << " ; ";
    }
    cout << endl;
    cout << "Methods: ";
    for (auto m : methods) {
        cout << m.first << "->" << m.second << " ; ";
    }
    cout << endl;
    cout << "Class Variables: ";
    for (auto it = classVar.begin(); it != classVar.end(); it++) {
        VarDecl* vd = it->second;
        cout << (it->first).first << "->" << (it->first).second << " with type=" << 
            type_kind_string(vd->type->typeKind) << " ; ";
    }
    cout << endl;
    cout << "Method Variables: ";
    for (auto it = methodVar.begin(); it != methodVar.end(); it++) {
        VarDecl* vd = it->second;
        cout << get<0>(it->first) << "->" << get<1>(it->first) << "->" << get<2>(it->first) << 
            " with type=" << type_kind_string(vd->type->typeKind) <<  " ; ";
    }
    cout << endl;
    cout << "Method Formals: ";
    for (auto it = methodFormal.begin(); it != methodFormal.end(); it++) {
        Type *t = it->second->type;
        cout << get<0>(it->first) << "->" << get<1>(it->first) << "->" << get<2>(it->first) <<
            " with type=" << type_kind_string(t->typeKind) << " ; ";
    }
    cout << endl;
}