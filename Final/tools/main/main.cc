#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "treep.hh"
#include "quad.hh"
#include "xml2quad.hh"
#include "flowinfo.hh"
#include "color.hh"
#include "quad2rpi.hh"
#include "prepareregalloc.hh"
#include "regalloc.hh"

#include "quad2xml.hh"
#include "blocking.hh"
#include "quadssa.hh"

#include "tree2xml.hh"
#include "tree2quad.hh"
#include "canon.hh"
#include "xml2tree.hh"

#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "xml2ast.hh"
#include "ast2xml.hh"
#include "temp.hh"
#include "namemaps.hh"
#include "semant.hh"
#include "ast2tree.hh"

using namespace std;
using namespace tree;
using namespace quad;
using namespace tinyxml2;

# define with_location_info false
// false means no location info in the AST XML files

int main(int argc, const char *argv[]) {
    string file;

    int number_of_colors = 9; //default 9: r0-r8
    const bool debug = argc > 1 && std::strcmp(argv[1], "--debug") == 0;

    if ((!debug && argc != 2) || (debug && argc != 3)) {
        cerr << "Usage: " << argv[0] << " [--debug] filename" << endl;
        return EXIT_FAILURE;
    }
    file = argv[argc - 1];
    //from HW3 onwards, we use the following naming convention for input files:
    string file_fmj = file + ".fmj"; // input source file
    //from HW5&HW4 onwards, we use the following naming convention for input files:
    string file_ast = file + ".2.ast"; // ast in xml
    string file_ast_semant = file + ".2-semant.ast"; // ast with semantic info in xml
    //frome HW6 onwards, we use the following naming convention for input files:
    string file_irp = file + ".3.irp";
    //from HW7 onwards, we use the following naming convention for output files:
    string file_quad_xml = file + ".4-xml.quad";
    string file_quad_ssa = file + ".4-ssa.quad";

    // boilerplate output filenames (used throughout the compiler pipeline)
    string file_quad_prepared = file + ".4-prepared.quad";
    string file_quad_color_xml = file + ".4-xml.clr";
    string file_rpi = file + ".s";

    cout << "------Parsing fmj source file: " << file_fmj << "------------" << endl;
    std::ifstream fmjfile(file_fmj);

    fdmj::Program *x_ast = fdmjParser(fmjfile, false); // false means no debug info from parser
    cout << "Convert AST to XML..." << endl;
    XMLDocument *x = ast2xml(x_ast, nullptr, with_location_info, false); // no semant info yet
    x->SaveFile(file_ast.c_str());
    std::cout << "Writing AST to file: " << file_ast << std::endl;
    delete x_ast; // free the original AST

    std::cout << "Read AST from file: " << file_ast << std::endl;
    x->LoadFile(file_ast.c_str());
    std::cout << "Converting XML to AST..." << std::endl;
    x_ast = xml2ast(x->FirstChildElement());

    if (x_ast == nullptr) {
        std::cout << "AST from file is not valid!" << endl;
        return EXIT_FAILURE;
    }
    std::cout << "Semantic analyzing AST..." << std::endl;
    std::cout << "--Making Name Maps..." << endl;
    Name_Maps *name_maps = makeNameMaps(x_ast); 
    std::cout << "--Analyzing Semantics..." << endl;
    AST_Semant_Map *semant_map = semant_analyze(x_ast); 
    semant_map->setNameMaps(name_maps);
    if(semant_map->getNameMaps() == nullptr)
        printf("semant_map->getNameMaps() == nullptr\n");
    semant_map->getNameMaps()->print();
    cout << "Convert AST to XML with Semantic Info..." << endl;
    x = ast2xml(x_ast, semant_map, with_location_info, true); 

    if (x->Error()) {
        std::cout << "AST is not valid when converting from AST with Semant Info!" << endl;
        return EXIT_FAILURE;  
    }

    x->SaveFile(file_ast_semant.c_str());
    tree::Program *ir = ast2tree(x_ast, semant_map);

    cout << "Saving IR (XML) to: " << file_irp << endl;
    x = tree2xml(ir);
    x->SaveFile(file_irp.c_str());

    tree::Program *ir_canon = canon(ir);
    quad::QuadProgram *x_quad = tree2quad(ir_canon);
    cout << "Done converting IR to Quad" << endl;
    QuadProgram *x_quad_blocked = blocking(x_quad);
    QuadProgram *x_ssa = quad2ssa(x_quad_blocked);
    cout << "Done converting Quad to Quad-SSA" << endl;
    cout << "Writing Quad-SSA to: " << file_quad_ssa<< endl;
    ofstream out_ssa(file_quad_ssa);
    if (!out_ssa) {
        cerr << "Error opening file: " << file_quad_ssa << endl;
        return EXIT_FAILURE;
    }
    string temp_str;
    temp_str.clear(); temp_str.reserve(10000);
    x_ssa->print(temp_str, 0, true);
    out_ssa << temp_str;
    out_ssa.flush(); out_ssa.close();
    cout << "Done writing Quad-SSA (text) to: " << file_quad_ssa << endl;

    QuadProgram *x_reg_alloc = prepareRegAlloc(x_ssa);
    quad2file(x_reg_alloc, file_quad_prepared.c_str(), true);
    cout << "Coloring: " << file_quad_prepared << endl;

    XMLDocument *x_color = coloring(x_reg_alloc, number_of_colors, true);
    cout << "Writing regAlloc'ed (colors XML) to " << file_quad_color_xml << endl;
    x_color->SaveFile(file_quad_color_xml.c_str());
    ColorMap *colormap = xml2colormap(file_quad_color_xml);
    //colormap->print(); //check the color map
    cout << "Writing rpi code to: " << file_rpi << endl;
    
    quad2rpi(x_reg_alloc, colormap, file_rpi);

    cout << "-----Done---" << endl;
    return EXIT_SUCCESS;
}