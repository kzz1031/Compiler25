#ifndef _XML2TREE_HH
#define _XML2TREE_HH

#include <iostream>
#include "treep.hh"
#include "tinyxml2.hh"

using namespace tree;
using namespace tinyxml2;

tree::Program* xml2tree(tinyxml2::XMLDocument* doc);
tree::Program* xml2tree(string xmlfilename);

#endif