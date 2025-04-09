#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "config.hh"
#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "xml2ast.hh"
#include "temp.hh"
#include "treep.hh"
#include "namemaps.hh"
#include "semant.hh"
#include "ast2tree.hh"
#include "tree2xml.hh"

using namespace std;
using namespace fdmj;
using namespace tree;
using namespace tinyxml2;

int main(int argc, const char *argv[]) {
    string file;

    const bool debug = argc > 1 && std::strcmp(argv[1], "--debug") == 0;

    if ((!debug && argc != 2) || (debug && argc != 3)) {
        cerr << "Usage: " << argv[0] << " [--debug] filename" << endl;
        return EXIT_FAILURE;
    }
    file = argv[argc - 1];

    // boilerplate output filenames (used throughout the compiler pipeline)
    string file_ast = file + ".2-semant.ast"; // ast in xml
    string file_irp = file + ".3.irp";

    cout << "------Reading AST from : " << file_ast << "------------" << endl;
    AST_Semant_Map *semant_map = new AST_Semant_Map();
    fdmj::Program *root = xml2ast(file_ast, &semant_map);
    if (root == nullptr) {
        cerr << "Error reading AST from: " << file_ast << endl;
        return EXIT_FAILURE;
    }
    semant_map->getNameMaps()->print();
    cout << "Converting AST to IR" << endl;
    //Compiler_Config::print_config();
    tree::Program *ir = ast2tree(root, semant_map);
    cout << "Saving IR (XML) to: " << file_irp << endl;
    XMLDocument *x = tree2xml(ir);
    x->SaveFile(file_irp.c_str());
    cout << "-----Done---" << endl;
    return EXIT_SUCCESS;
}
