#ifndef __QUAD_REGALLOC_HH
#define __QUAD_REGALLOC_HH
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include "quad.hh"
#include "temp.hh"
#include "ig.hh"
#include "coloring.hh"

using namespace std;
using namespace quad;

//These are function that are used to perform register allocation 
//Source: Quad program, Target: RPi assembly
//Dependencies: ControlFlowInfo, DataFlowInfo, Interference Graph and Coloring
//The resulting coloring object is to be used in quad2rpi 

//this is to add necessary info to the quadFuncDecl to prepare it for regalloc
QuadFuncDecl* prepareRegAlloc(QuadFuncDecl* quadFuncDecl); 

//get the colloring for a function
Coloring *coloring(QuadFuncDecl *func, int k, bool output_ig); // Color the func with k colors (machine registers)

XMLDocument *coloring(QuadProgram *prog, int k, bool output_ig); // Get the coloring in XML form

#endif // __QUAD_REGALLOC_HH