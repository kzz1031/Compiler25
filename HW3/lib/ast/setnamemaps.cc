#define DEBUG
#undef DEBUG

#include <iostream>
#include <variant>
#include <map>
#include <vector>
#include <algorithm>
#include "ASTheader.hh"
#include "FDMJAST.hh"
#include "namemaps.hh"

using namespace std;
using namespace fdmj;

// TODO

// void AST_Name_Map_Visitor::visit(Program* node) {
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

//rest of the visit functions here
