#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "treep.hh"
#include "canon.hh"
#include "quad.hh"
#include "xml2tree.hh"
#include "tree2xml.hh"
#include "tree2quad.hh"

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
    string file_irp = file + ".3.irp";
    string file_irp_canon = file + ".3-canon.irp";
    string file_quad = file + ".4.quad";

    cout << "Reading IR (XML) from: " << file_irp << endl;
    tree::Program *ir = xml2tree(file_irp);
    if (ir == NULL) {
        cerr << "Error: " << file_irp << " not found or IR ill-formed" << endl;
        return EXIT_FAILURE;
    }
    cout << "Canonicalization..." << endl;
    tree::Program *ir_canon = canon(ir);
    cout << "Writing Canonicalized IR to " << file_irp_canon << endl;
    XMLDocument *doc = tree2xml(ir_canon);
    cout << "Writing IR to: " << file_irp_canon << endl;
    doc->SaveFile(file_irp_canon.c_str()); 
    QuadProgram *qd = tree2quad(ir_canon);
    if (qd == nullptr) {
        cerr << "Error converting IR to Quad" << endl;
        return EXIT_FAILURE;
    }
    cout << "Done converting IR to Quad" << endl;
    string temp_str; temp_str.reserve(50000);
    qd->print(temp_str, 0, true);
    cout << "Writing Quad to: " << file_quad << endl;
    ofstream qo(file_quad);
    if (!qo) {
        cerr << "Error opening file: " << file_quad << endl;
        return EXIT_FAILURE;
    }
    qo << temp_str;
    qo.flush(); qo.close();

    ofstream out("/dev/tty");
    out << "-----Done---" << endl;
    return EXIT_SUCCESS;
}