#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "ast2xml.hh"
#include "xml2ast.hh"
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace fdmj;
using namespace tinyxml2;

#define with_location_info false
// false means no location info in the AST XML files

Program *prog();

int main(int argc, const char *argv[]) {
    string file;

    const bool debug = argc > 1 && std::strcmp(argv[1], "--debug") == 0;

    if ((!debug && argc != 2) || (debug && argc != 3)) {
        cerr << "Usage: " << argv[0] << " [--debug] filename" << endl;
        return EXIT_FAILURE;
    }

    file = argv[argc - 1];

    // boilerplate output filenames (used throughout the compiler pipeline)
    string file_fmj = file + ".fmj"; // input source file
    string file_ast = file + ".2.ast";        // ast in xml
    string file_ast_semant = file + ".2-semant.ast";

    cout << "------Parsing fmj source file: " << file_fmj << "------------" << endl;
    std::ifstream fmjfile(file_fmj);

    Program *root = fdmjParser(fmjfile, false); // false means no debug info from parser

    if (root == nullptr) {
        std::cout << "AST is not valid!" << endl;
        return EXIT_FAILURE;
    }

    cout << "Convert AST  to XML..." << endl;
    XMLDocument *x = ast2xml(root, nullptr, with_location_info, false); // no semant info yet
    x->SaveFile(file_ast.c_str());
    std::cout << "Writing AST to file: " << file_ast << std::endl;

    if (x->Error()) {
        std::cout << "AST is not valid!" << endl;
        return EXIT_FAILURE;
    }
    
    //reloading from xml to AST
    delete root;
    std::cout << "Read AST from file: " << file_ast << std::endl;
    x->LoadFile(file_ast.c_str());
    std::cout << "Converting XML to AST..." << std::endl;
    root = xml2ast(x->FirstChildElement());

    if (root == nullptr) {
        std::cout << "AST from file is not valid!" << endl;
        return EXIT_FAILURE;
    }

    //Semantic analysis
    std::cout << "Semantic analysis..." << std::endl;
    std::cout << "--Making Name Maps..." << endl;
    Name_Maps *name_maps = makeNameMaps(root); 
    std::cout << "--Analyzing Semantics..." << endl;
    AST_Semant_Map *semant_map = semant_analyze(root); 

    cout << "Convert AST to XML with Semantic Info..." << endl;
    x = ast2xml(root, semant_map, with_location_info, true); // no semant info yet

    if (x->Error()) {
        std::cout << "AST is not valid when converting from AST with Semant Info!" << endl;
        return EXIT_FAILURE;  
    }

    x->SaveFile(file_ast_semant.c_str());
    return EXIT_SUCCESS;
}