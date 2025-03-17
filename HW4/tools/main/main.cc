#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "xml2ast.hh"
#include "temp.hh"
#include "treep.hh"
#include "namemaps.hh"
#include "semant.hh"
#include "ast2tree.hh"
#include "tree2xml.hh"
#include "canon.hh"

using namespace std;
using namespace fdmj;
using namespace tree;
using namespace tinyxml2;

tree::Program* generate_a_testIR(); //forward declaration

int main(int argc, const char *argv[]) {
    string file;

    const bool debug = argc > 1 && std::strcmp(argv[1], "--debug") == 0;

    if ((!debug && argc != 2) || (debug && argc != 3)) {
        cerr << "Usage: " << argv[0] << " [--debug] filename" << endl;
        return EXIT_FAILURE;
    }
    file = argv[argc - 1];

    // boilerplate output filenames (used throughout the compiler pipeline)
    string file_ast = file + ".2.ast"; // ast in xml
    string file_irp = file + ".3.irp";
    string file_irp_canon = file + ".3-canon.irp";

    cout << "------Reading AST from : " << file_ast << "------------" << endl;
    AST_Semant_Map *semant_map = new AST_Semant_Map();
    fdmj::Program *root = xml2ast(file_ast, &semant_map);
    if (root == nullptr) {
        cerr << "Error reading AST from: " << file_ast << endl;
        return EXIT_FAILURE;
    }
    //semant_map->getNameMaps()->print();
    cout << "Converting AST to IR" << endl;
    //tree::Program *ir = ast2tree(root, semant_map);
    tree::Program *ir = generate_a_testIR();
    cout << "Saving IR (XML) to: " << file_irp << endl;
    XMLDocument *x = tree2xml(ir);
    x->SaveFile(file_irp.c_str());
    cout << "Canonicalizing IR" << endl;
    tree::Program *ir_canon = canon(ir);
    cout << "Saving Canonical IR (XML) to: " << file_irp_canon << endl;
    XMLDocument *x_canon = tree2xml(ir_canon);
    x_canon->SaveFile(file_irp_canon.c_str());
    cout << "-----Done---" << endl;
    return EXIT_SUCCESS;
}

/*
The following is for testing purposes only. It generates a simple IR program corresponding to the following fdmj program:

public int main() {
    putchar((x=1;} 19));
    return 100;
}
*/
tree::Program* generate_a_testIR() {
    Temp_map *tm = new Temp_map();
    tree::Label *entry_label = tm->newlabel();
    tree::LabelStm *label_stm = new tree::LabelStm(entry_label);
    tree::Move *move = new tree::Move(new tree::TempExp(tree::Type::INT, tm->newtemp()), new tree::Const(1));

    vector<tree::Stm*> *sl1 = new vector<tree::Stm*>(); sl1->push_back(move);

    vector<tree::Exp*> *args = new vector<tree::Exp*>();
    tree::Eseq *eseq = new tree::Eseq(tree::Type::INT, new tree::Seq(sl1), new tree::Const(19));
    args->push_back(eseq);

    tree::ExtCall *call = new tree::ExtCall(tree::Type::INT, new string("putchar"), args);
    tree::Return *ret = new tree::Return(new tree::Const(100));
    vector<tree::Stm*> *sl = new vector<tree::Stm*>();
    sl->push_back(label_stm);
    sl->push_back(new tree::ExpStm(call));
    sl->push_back(ret);
    tree::Block *block = new tree::Block(entry_label, nullptr, sl);
    vector<tree::Block*> *bl = new vector<tree::Block*>();
    bl->push_back(block);
    tree::FuncDecl *fd = new tree::FuncDecl("main", nullptr, bl, tree::Type::INT, 100, 100);
    vector<tree::FuncDecl*> *fdl = new vector<tree::FuncDecl*>();
    fdl->push_back(fd);
    return new tree::Program(fdl);
}