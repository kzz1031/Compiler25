#define DEBUG
#undef DEBUG

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include "namemaps.hh"
#include "semant.hh"

using namespace std;
using namespace fdmj;

// TODO

AST_Semant_Map* semant_analyze(Program* node) {
    std::cout << "TODO" << std::endl;
    return nullptr;
    // std::cerr << "Start Semantic Analysis" << std::endl;
    // if (node == nullptr) {
    //     return nullptr;
    // }
    // Name_Maps* name_maps = makeNameMaps(node);
    // AST_Semant_Visitor semant_visitor(name_maps);
    // semant_visitor.visit(node);
    // std::cerr << "Semantic Analysis Done" << std::endl;
    // return semant_visitor.getSemantMap();
}

// void AST_Semant_Visitor::visit(Program* node) {
// #ifdef DEBUG
//     std::cout << "Visiting Program" << std::endl;
// #endif
//     if (node == nullptr) {
//         return;
//     }
//     if (node->main != nullptr) {
//         node->main->accept(*this);
//     }
//     if (node->cdl != nullptr) {
//         for (auto cl : *(node->cdl)) {
//             cl->accept(*this);
//         }
//     }
// }

// rest of the code