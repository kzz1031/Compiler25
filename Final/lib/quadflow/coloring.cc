#define DEBUG
#undef DEBUG

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "config.hh"
#include "quad.hh"
#include "temp.hh"
#include "ig.hh"
#include "coloring.hh"
#include "tinyxml2.hh"

using namespace std;
using namespace quad;
using namespace tinyxml2;

string Coloring::printColoring() {
    string result; result.reserve(50000);
    result = "Interference Graph: with #colors=" + to_string(k) + ":\n";
    for (const auto& pair : graph) {
        int node = pair.first;
        result += "Node " + to_string(node) + ": ";
        for (int neighbor : pair.second) {
            result += to_string(neighbor) + " ";
        }
        result += "\n";
    }
    //now print the simplified nodes
    result += "Simplified nodes (reversed order from the simplification process): ";
    stack<int> simplifiedNodesCopy = simplifiedNodes; // Copy the stack to avoid modifying the original
    while (!simplifiedNodesCopy.empty()) {
        int node = simplifiedNodesCopy.top();
        simplifiedNodesCopy.pop();
        result += to_string(node) + " ";
    }
    result += "\n";
    //now print the coalesced moves
    result += "Coalesced moves: \n";
    for (const auto& move : coalescedMoves) {
        result += "Nodes: {";
        bool first = true;
        for (const auto& coalescedNode : move.second) {
            if (!first) {
                result += ", ";
            }
            first = false;
            result += to_string(coalescedNode);
        }
        result += "} are coalesced to node " + to_string(move.first) + ".\n";
    }
    //noew print the remaining move pairs
    result += "Remaining move pairs: ";
    for (const auto& move : movePairs) {
        result += "(" + to_string(move.first) + ", " + to_string(move.second) + ") ";
    }
    result += "\n";
    if (spilled.size() > 0) {
        result += "Spilled temps: ";
        for (const auto& temp : spilled) {
            result += to_string(temp) + " ";
        }
        result += "\n";
    }
    if (colors.size() > 0) {
        result += "Colored nodes:\n";
        for (const auto& color : colors) {
            result += to_string(color.first) + " -> " + to_string(color.second) + "\n";
        }
    }
    return result;
}

void Coloring::printColoring(iostream &io) {
    io << printColoring(); 
    io << "\n";
}
        
// Get the neighbors of a node
set<int> Coloring::getNeighbors(int n) {
    if (graph.find(n) == graph.end()) {
        return set<int>(); // Node not found, return empty set
    }
    return graph[n];
}

// Check if a node is in a move
bool Coloring::isMove(int n) {
    for (auto& pair : movePairs) {
        if (pair.first == n || pair.second == n) {
            return true; // Node is in a move
        }
    }
    return false; // Node is not in a move
}

void Coloring::eraseNode(int node) {
    graph.erase(node);
    for (auto& pair : graph) {
        if (pair.second.find(node) != pair.second.end()) {
            pair.second.erase(node);
        }
    }
}

void Coloring::addEdge(int u, int v) {
    if (u == v) return; // Ignore self-loops
    if (graph.find(u) == graph.end()) {
        graph[u] = set<int>();
    }
    if (graph.find(v) == graph.end()) {
        graph[v] = set<int>();
    }
    graph[u].insert(v);
    graph[v].insert(u);
}

XMLElement* Coloring::coloring2xml(XMLDocument *doc, string funcname) {
    if (doc == nullptr) {
        cerr << "Error: XMLDocument is null." << endl;
        return nullptr; // No document to write to
    }
    XMLElement *root = doc->NewElement("Coloring");
    doc->InsertFirstChild(root);
    root->SetAttribute("func", funcname.c_str());
    root->SetAttribute("k", k);
    XMLElement *colors_e = doc->NewElement("Colors");
    root->InsertEndChild(colors_e);
    for (const auto& color : colors) {
        XMLElement *color_e = doc->NewElement("Color");
        color_e->SetAttribute("node", color.first);
        color_e->SetAttribute("color", color.second);
        colors_e->InsertEndChild(color_e);
    }
    XMLElement *spills_e = doc->NewElement("Spills"); 
    root->InsertEndChild(spills_e);
    for (const auto& spill : spilled) {
        XMLElement *spill_e = doc->NewElement("Spill");
        spill_e->SetAttribute("node", spill);
        spills_e->InsertEndChild(spill_e);
    }
    return root;
}

Coloring *coloring(InterferenceGraph *ig, int k) {
    Coloring *c = new Coloring(ig, k);
    while (c->simplify() || c->coalesce() || c->freeze() || c->spill()) {
        // Keep simplifying, coalescing, freezing, and spilling until no more changes can be made
#ifdef DEBUG
            cout << "After a loop: IG size: " << c->graph.size() << endl;
            cout << c->printColoring() << endl; //print the graph
#endif
    }
    c->select(); // Final selection of colors
    return c;
}

// Check if the coloring is valid
bool Coloring::checkColoring() {
#ifdef DEBUG
    cout << "Checking coloring... colors size = " << colors.size() << ", spills size=" << spilled.size() << " IG graph size=" << ig->graph.size() << endl;
#endif
    
    bool valid = true; // Assume valid coloring
    //gather all the nodes in the graph
    set<int> all_nodes = set<int>(); //set of all nodes in the original graph
    for (auto it: ig->graph) {
        all_nodes.insert(it.first); //add the node to the set
    }
#ifdef DEBUG
    cout << "All nodes in the graph: ";
    for (auto it : all_nodes) {
        cout << it << " ";
    }
    cout << endl;
#endif
    //check if all nodes are colored or spilled
    for (auto it : all_nodes) {
        if (colors.find(it) == colors.end() && spilled.find(it) == spilled.end()) {
            cout << "Error: Imcomplete Coloring: Node " << it << " not colored or spilled" << endl;
            valid = false; // Not valid coloring
        }
        //collect all it's neighbors
        set<int> neighbors = ig->graph[it];
#ifdef DEBUG
        cout << "Node " << it << " has color=" <<  colors[it] << " with neighbors: ";
        for (auto n : neighbors) {
            cout << n << " " ;
        }
        cout << endl;
#endif
        if (spilled.find(it) != spilled.end()) {
#ifdef DEBUG
            cout << "Node " << it << " is spilled, no need to check, skip..." << endl;
#endif
            continue; //if it's spilled, don't check the neighbors
        }
        set<pair<int, int>> neighbors_colors = set<pair<int, int>>(); //set of the colors of the neighbors
        for (auto n : neighbors) {
            if (colors.find(n) != colors.end()) {
                neighbors_colors.insert(pair(n, colors[n])); //add the color of the neighbor to the set
            }
        }
#ifdef DEBUG
        cout << "   It's neighbors colors: ";
        for (auto nc : neighbors_colors) {
            if (spilled.find(nc.first) != spilled.end()) {
                cout << nc.first << "@spilled ";
            } else 
                cout << nc.first<< "@" << nc.second<< " ";
        }
        cout << endl;
#endif        
        for (auto nc : neighbors_colors) {
            if (spilled.find(nc.first) != spilled.end()) continue; //if it's spilled, don't check the color 
            if (nc.second == colors[it])  {
                cout << "Error: Conflict! Node " << it << " has the same color (color=" 
                        << colors[it] << ") as its neighbor " << nc.first << " (color=" << nc.second << ")" << endl;
                valid = false; // Not valid coloring
            }
        }
    }
    return valid; // Valid coloring
}