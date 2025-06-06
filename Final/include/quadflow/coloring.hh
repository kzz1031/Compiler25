#ifndef __COLORING_HH
#define __COLORING_HH

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include "ig.hh"
#include "tinyxml2.hh"

using namespace std;
using namespace quad;
using namespace tinyxml2;

class Coloring {
public:
    InterferenceGraph *ig; // Pointer to the interference graph
    int k; // number of colors (machine registers) available
    map<int, set<int>> graph; // Adjacency list representation of the ig in various stages of coloring 
    set<pair<int, int>> movePairs; // set of moves that are still active (i.e., not coalesced nor freezed)
    stack<int> simplifiedNodes; // stack of nodes that have been simplified out (in the order they were simplified)
    map<int, set<int>> coalescedMoves; // Set of nodes (involved in moves) that have been coalesced into a node (int), which is still in the graph
    set<int> spilled; // Set of spilled temps (result of coloring)
    map<int, int> colors; // the colors resulted from the coloring

    //constructing the graph from the original graph and the move pairs
    Coloring(InterferenceGraph *ig, int k) : ig(ig), k(k) {
        graph = ig->graph;
        movePairs = ig->movePairs;
        simplifiedNodes = stack<int>();
        coalescedMoves = map<int, set<int>>();
        spilled = set<int>();
    }; // Constructor

    //printing functions
    string printColoring(); // Print the current graph with other info (move pairs, etc.)
    void printColoring(iostream &io); // Print the current graph with other info (move pairs, etc.)
    tinyxml2::XMLElement* coloring2xml(XMLDocument *doc, string funcname); // Getting into XML form

    //helper functions
    void addEdge(int u, int v);// Add an edge to the graph
    void eraseNode(int node); // Erase a node from the graph (and the edges)
    set<int> getNeighbors(int node); // Get the neighbors of a node
    bool isMove(int n); // Check if a node is still in a move
    static bool isMachineReg(int n) {return n < 100;}; // Check if a node is a machine register 

    // Coloring functions
    bool simplify(); // Simplify the graph by removing nodes with degree < k, return true if any nodes were removed
    bool coalesce(); // Coalesce the graph by merging nodes with the same color, return true if any nodes were merged
    bool freeze(); // Freeze the graph by removing nodes with degree < k, return true if any nodes were frozen
    bool spill(); // Select nodes with degree >= k, return true if any nodes were spilled (this is potential spilling process)
    bool select(); // Select nodes to color, return true if any nodes were selected. The actual spilling occurs here.

    bool checkColoring(); // Check if the coloring is valid 
};

Coloring *coloring(InterferenceGraph *ig, int k); // Color the graph with k colors (machine registers)

#endif // __COLORING_HH