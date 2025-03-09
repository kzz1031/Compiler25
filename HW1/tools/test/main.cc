#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "MinusIntConverter.hh"
#include "constantPropagation.hh"
#include "executor.hh"
#include "ast2xml.hh"
#include "xml2ast.hh"
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <cassert>

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

  // boilerplate output filenames (used throutghout the compiler pipeline)
  string file_fmj = file + ".fmj"; // input source file
  string file_ast_1 = file + ".1.out"; // raw
  string file_ast_2 = file + ".2.out"; // constant propagation
  string file_return = file + ".3.out"; // return

  ifstream fmjfile(file_fmj);
  Program *root = fdmjParser(fmjfile, false); // false means no debug info from parser
  if (root == nullptr) {
    cerr << "AST is not valid!" << endl;
    return EXIT_FAILURE;
  }

  XMLDocument *x = nullptr;

  // raw
  x = ast2xml(root, with_location_info);
  assert(!x->Error() && "AST is not valid!");
  x->SaveFile(file_ast_1.c_str());

  // constantPropagation
  Program *cp = constantPropagate(root);
  x = ast2xml(cp, with_location_info);
  assert(!x->Error() && "AST is not valid!");
  x->SaveFile(file_ast_2.c_str());

  // execution
  ofstream outFile(file_return);
  outFile << execute(root) << endl << execute(cp) << endl;
  outFile.close();

  return EXIT_SUCCESS;
}