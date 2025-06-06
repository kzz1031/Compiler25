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

using namespace std;
using namespace tree;
using namespace quad;
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
    string file_quad_prepared_xml = file + ".4-prepared-xml.quad";
    string file_quad_color_xml = file + ".4-xml.clr";
    string file_rpi = file + ".s";

    cout << "Reading prepared quad from xml: " << file_quad_prepared_xml << endl;
    quad::QuadProgram *x3 = xml2quad(file_quad_prepared_xml.c_str());
    if (x3 == nullptr) {
        cerr << "Error reading Quad from xml: " << file_quad_prepared_xml << endl;
        return EXIT_FAILURE;
    }
    cout << "Reading prepared colors for the QuadProgram: " << file_quad_color_xml << endl;
    ColorMap *colormap = xml2colormap(file_quad_color_xml);
    //colormap->print(); //check the color map
    cout << "Writing rpi code to: " << file_rpi << endl;
    quad2rpi(x3, colormap, file_rpi);
    cout << "-----Done---" << endl;
    return EXIT_SUCCESS;
}