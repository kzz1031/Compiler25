#ifndef __HH_PREPAREREGALLOC
#define __HH_PREPAREREGALLOC
#include <iostream>
#include <string>
#include <vector>
#include "quad.hh"

using namespace std;
using namespace quad;

//this is to add necessary info to the quadFuncDecl to prepare it for regalloc
QuadFuncDecl* prepareRegAlloc(QuadFuncDecl* quadFuncDecl); 
QuadProgram* prepareRegAlloc(QuadProgram* quadProgram); 

#endif