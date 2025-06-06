#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "ig.hh"

using namespace std;

void InterferenceGraph::printGraph(string filename) {
    ofstream io(filename);
    if (!io.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }
    printGraph(io);
    io.close();
}

void InterferenceGraph::printGraph(ofstream &io) {
    io << printGraph();
}

string InterferenceGraph::printGraph() {
    string result; result.reserve(50000);
    result = "Interference Graph: size=" + to_string(graph.size()) + "\n";
    for (const auto& pair : graph) {
        int node = pair.first;
        result += "Node " + to_string(node) +  ": ";
        for (int neighbor : pair.second) {
            result  += to_string(neighbor) +  " ";
        }
        result += "\n";
    }
    //noew print the move pairs
    result += "Move pairs: \n";
    for (const auto& move : movePairs) {
        result += "(" + to_string(move.first) + ", " + to_string(move.second) +  ") ";
    }
    result += "\n";
    return result;
} 