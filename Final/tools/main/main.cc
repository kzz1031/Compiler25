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

    string file_quad_ssa_xml = file + ".4-ssa-xml.quad";
    // boilerplate output filenames (used throughout the compiler pipeline)
    string file_quad_prepared = file + ".4-prepared.quad";
    string file_quad_prepared_xml = file + ".4-prepared-xml.quad";
    string file_quad_color_xml = file + ".4-xml.clr";
    string file_rpi = file + ".s";

    cout << "Reading Quad from xml: " << file_quad_ssa_xml << endl;
    quad::QuadProgram *x3 = xml2quad(file_quad_ssa_xml.c_str());
    if (x3 == nullptr) {
        cerr << "Error reading Quad from xml: " << file_quad_ssa_xml << endl;
        return EXIT_FAILURE;
    }

    QuadProgram *x4 = prepareRegAlloc(x3);
    quad2file(x4, file_quad_prepared.c_str(), true);
    cout << "Coloring: " << file_quad_prepared << endl;

    XMLDocument *x5 = coloring(x4, number_of_colors, true);
    cout << "Writing regAlloc'ed (colors XML) to " << file_quad_color_xml << endl;
    x5->SaveFile(file_quad_color_xml.c_str());
    ColorMap *colormap = xml2colormap(file_quad_color_xml);
    //colormap->print(); //check the color map
    cout << "Writing rpi code to: " << file_rpi << endl;
    
    quad2rpi(x4, colormap, file_rpi);

    cout << "-----Done---" << endl;
    return EXIT_SUCCESS;
}