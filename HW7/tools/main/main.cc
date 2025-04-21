#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "treep.hh"
#include "quad.hh"
#include "xml2quad.hh"
#include "quadssa.hh"
#include "blocking.hh"

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
    string file_quad_xml = file + ".4-xml.quad";
    string file_quad = file + ".4.quad";
    string file_quad_block = file + ".4-block.quad";
    string file_quad_ssa = file + ".4-ssa.quad";

    cout << "Reading Quad from xml: " << file_quad_xml << endl;
    quad::QuadProgram *x3 = xml2quad(file_quad_xml.c_str());
    cout << "Writing is back (text) to: " << file_quad << endl;
    ofstream out2(file_quad);
    if (!out2) {
        cerr << "Error opening file: " << file_quad << endl;
        return EXIT_FAILURE;
    }
    string temp_str;
    temp_str.clear(); temp_str.reserve(100000);
    x3->print(temp_str, 0, true);
    out2 << temp_str;
    out2.flush(); out2.close();
    cout << "Done writing Quad (text) to: " << file_quad << endl;
    cout << "Blocking the quad" << endl;
    QuadProgram *x4 = blocking(x3);
    cout << "Writing blocked Quad to: " << file_quad_block << endl;
    temp_str.clear(); temp_str.reserve(100000);
    ofstream out_block(file_quad_block);
    if (!out_block) {
        cerr << "Error opening file: " << file_quad_block << endl;
        return EXIT_FAILURE;
    }
    x4->print(temp_str, 0, true);
    out_block << temp_str;
    out_block.flush(); out_block.close();
    cout << "Done writing blocked Quad (text) to: " << file_quad_block << endl;
    cout << "Converting Quad to Quad-SSA" << endl;
    //quad2ssa assumes blocked quad!!!
    QuadProgram *x5 = quad2ssa(x4);
    if (x5 == nullptr) {
        cerr << "Error converting Quad to Quad-SSA" << endl;
        return EXIT_FAILURE;
    }
    cout << "Done converting Quad to Quad-SSA" << endl;
    cout << "Writing Quad-SSA to: " << file_quad_ssa<< endl;
    ofstream out_ssa(file_quad_ssa);
    if (!out_ssa) {
        cerr << "Error opening file: " << file_quad_ssa << endl;
        return EXIT_FAILURE;
    }
    temp_str.clear(); temp_str.reserve(10000);
    x5->print(temp_str, 0, true);
    out_ssa << temp_str;
    out_ssa.flush(); out_ssa.close();
    cout << "Done writing Quad-SSA (text) to: " << file_quad_ssa << endl;
    cout << "-----Done---" << endl;
    return EXIT_SUCCESS;
}