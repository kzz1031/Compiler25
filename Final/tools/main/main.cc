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


using namespace std;
using namespace tree;
using namespace quad;
using namespace tinyxml2;

int main(int argc, const char *argv[]) {
    string file;

    int number_of_colors = 9; //default 9: r0-r8
    const bool debug = argc > 1 && std::strcmp(argv[1], "--debug") == 0;

    if ((!debug && argc != 2) || (debug && argc != 3)) {
        cerr << "Usage: " << argv[0] << " [--debug] filename" << endl;
        return EXIT_FAILURE;
    }
    file = argv[argc - 1];
    //from HW7 onwards, we use the following naming convention for output files:
    string file_quad_xml = file + ".4-xml.quad";
    string file_quad_ssa = file + ".4-ssa.quad";

    // boilerplate output filenames (used throughout the compiler pipeline)
    string file_quad_prepared = file + ".4-prepared.quad";
    string file_quad_color_xml = file + ".4-xml.clr";
    string file_rpi = file + ".s";

    cout << "Reading Quad from xml: " << file_quad_xml << endl;
    quad::QuadProgram *x_quad = xml2quad(file_quad_xml.c_str());
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