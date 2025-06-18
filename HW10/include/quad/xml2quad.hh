#ifndef __QUAD_HH
#define __QUAD_HH

#include <string>
#include "quad.hh"

using namespace std;
using namespace quad;
using namespace tinyxml2;

// Convert XML to Quad program
QuadProgram* xml2quad(const char* filename);

#endif