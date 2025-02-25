#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "MinusIntConverter.hh"
#include "ast2xml.hh"
#include "xml2ast.hh"
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace fdmj;
using namespace tinyxml2;

#define with_location_info true 
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

  // boilerplate output filenames (used throutghout the compiler pipeline)
  string file_fmj = file + ".fmj"; // input source file
  string file_out = file + ".out";
  string file_src = file + ".src";
  string file_src2 = file + ".1.debug.src"; // debug source file
  string file_ast = file + ".2.ast";        // ast in xml
  string file_ast2 = file + ".2-debug.ast";
  string file_ast3 = file + ".2-debug3.ast";
  string file_ast4 = file + ".2-debug4.ast";
  string file_irp = file + ".3.irp";
  string file_stm = file + ".4.stm";
  string file_liv = file + ".5.liv";

  cout << "------Parsing fmj source file: " << file_fmj << "------------"
       << endl;
  std::ifstream fmjfile(file_fmj);
  Program *root =
      fdmjParser(fmjfile, true); // false means no debug info from parser
  if (root == nullptr) {
    std::cout << "AST is not valid!" << endl;
    return EXIT_FAILURE;
  }
  cout << "Convert AST  to XML..." << endl;
  XMLDocument *x = ast2xml(root, with_location_info);
  delete root;
  if (x->Error()) {
    std::cout << "AST is not valid!" << endl;
    return EXIT_FAILURE;
  }
  std::cout << "AST is valid!" << endl;
  return EXIT_SUCCESS;
}