#define DEBUG
#undef DEBUG

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
using namespace tinyxml2;

//get the colloring for a function
Coloring *coloring(QuadFuncDecl *func, int k, bool output_ig) { // Color the func with k colors (machine registers)
    InterferenceGraph *ig = buildIg(func); // Build the interference graph
    if (output_ig) {
        cout << "Interference graph for function: " << func->funcname << endl;
        cout << ig->printGraph() ; // Print the graph
    }
    return coloring(ig, k); //color the graph
}

XMLDocument *coloring(QuadProgram *prog, int k, bool output_ig) { // Get the coloring in XML form
    XMLDocument *doc = new XMLDocument();
    XMLDeclaration *decl = doc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    doc->InsertFirstChild(decl);
    XMLElement* xml_root = doc->NewElement("COLORING");
    doc->InsertEndChild(xml_root);
    if (prog == nullptr || prog->quadFuncDeclList == nullptr || prog->quadFuncDeclList->empty()) {
        return doc; // No functions to color
    }
    for (QuadFuncDecl *func : *prog->quadFuncDeclList) {
        Coloring *color = coloring(func, k, output_ig); // Color the function
        if (color != nullptr) {
            XMLElement *coloring_e = color->coloring2xml(doc, func->funcname); // Get the coloring in XML form
            xml_root->InsertEndChild(coloring_e);
        }
    }
    return  doc;
}