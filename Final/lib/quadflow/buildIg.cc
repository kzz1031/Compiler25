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
#include "flowinfo.hh"
#include "ig.hh"

static void addEdge(map<int, set<int>>& graph, int src, int dst) {
    if (graph.find(src) == graph.end()) {
        graph[src] = set<int>(); //add the src to the graph with no edges yet
    }
    if (graph.find(dst) == graph.end()) {
        graph[dst] = set<int>(); //add the dst to the graph with no edges yet
    }
    graph[src].insert(dst);
    graph[dst].insert(src);
}

static void addNode(map<int, set<int>>& graph, int n) {
    if (graph.find(n) == graph.end()) {
        graph[n] = set<int>(); //add the n to the graph with no edges yet
    }
}

static void addMovePair(set<pair<int, int>>& movePairs, int src, int dst) {
    if (src != dst && 
            ( ! InterferenceGraph::isMachineReg(src) || ! InterferenceGraph::isMachineReg(dst))) {
        movePairs.insert(make_pair(src, dst)); //add the move pair
        movePairs.insert(make_pair(dst, src)); //add the inverse move pair
    }
}

InterferenceGraph* buildIg(QuadFuncDecl* funcdecl) {
    map<int, set<int>> graph = map<int, set<int>>(); // Adjacency list representation of the interference graph
    set<pair<int, int>> movePairs = set<pair<int, int>>(); // Set of move pairs

    if (funcdecl == nullptr || funcdecl->quadblocklist == nullptr || funcdecl->quadblocklist->empty()) {
        return new InterferenceGraph(graph, movePairs); 
    }

    //get the data flow info
    DataFlowInfo* flowInfo = new DataFlowInfo(funcdecl);
    flowInfo->computeLiveness();

#ifdef DEBUG
    flowInfo->printLiveness();
    cout << "Building interference graph for function: " << funcdecl->funcname << endl;
#endif

    //scan the statements to build the interference graph
    for (QuadBlock* block : *funcdecl->quadblocklist) {
        if (block == nullptr || block->quadlist == nullptr || block->quadlist->empty()) continue; //skip empty blocks
        for (QuadStm* stmt : *block->quadlist) {
#ifdef DEBUG
            cout << "Processing statement: " << quadKindToString(stmt->kind) << endl;
#endif
            Temp *dst_temp = nullptr; Temp *src_temp = nullptr;
            if (stmt == nullptr) continue; //skip null statements
            if (stmt->kind == QuadKind::MOVE) { //if it's a move of two temps, add the move pair
                if (stmt->def != nullptr && stmt->def->size()>0 && stmt->use != nullptr && stmt->use->size()>0) {
                    for (Temp* def : *stmt->def) {dst_temp = def; break;} //pick the first one (move should have at most one)
                    for (Temp* use : *stmt->use) {src_temp = use; break;} //pick the first one (move should have at most one)
                    if (dst_temp != nullptr && src_temp != nullptr) {
#ifdef DEBUG
                        cout << "Adding move pair: " << dst_temp->num << " -> " << src_temp->num << endl;
#endif
                        if (dst_temp->num != src_temp->num && 
                                ( ! InterferenceGraph::isMachineReg(dst_temp->num) || ! InterferenceGraph::isMachineReg(src_temp->num))) {
                                    // Ignore self-moves, and those that are both machine registers
                            addMovePair(movePairs, dst_temp->num, src_temp->num); //add the move pair
                            //add them to the graph (if not already there)
                            addNode(graph, dst_temp->num);
                            addNode(graph, src_temp->num);
                        }
                    }
                }
            }
            //Now add the interference edges
            if (flowInfo->liveout->find(stmt) == flowInfo->liveout->end()) continue; //skip if no liveout info! should not happen
            // Add edges for def and liveout interference
            if (stmt->def != nullptr) {  //skip if no def info! should not happen
                for (Temp* def : *stmt->def) {
                    addNode(graph, def->num); //add the def to the graph with no edges yet
                    set<int> lo = flowInfo->liveout->at(stmt); 
                    for (int n : lo) {
                        if (n == def->num) continue; // Ignore self-interference
                        if (stmt->kind!=QuadKind::MOVE) { //if it's not a move, add interference
                            addEdge(graph, def->num, n); //add the interference edge
    #ifdef DEBUG
                            cout << "Adding interference edge: " << def->num << " -> " << n << endl;
    #endif
                        } else {if (src_temp == nullptr || src_temp->num != n) { //now a move, and: if the right of a move is NOT a temp
                                        //or if it's a temp but not the same as the lo, just add interference as usual (else don't)
                            addEdge(graph, def->num, n); //add the interference edge
    #ifdef DEBUG
                                    cout << "Adding interference edge: " << def->num << " -> " << n << endl;
    #endif
                            }
                        } 
                    }
                }
            }
            // Add a graph node for each use temp (if not already there), in case it's an isolated node (undefined and used once!)
            if (stmt->use != nullptr) {
                for (Temp* use : *stmt->use) 
                    addNode(graph, use->num); //add the use to the graph with no edges yet
            }
        }
    }

#ifdef DEBUG
    cout << "Delete moves that are invalid (i.e., an edge in graph) " << endl;
#endif
    //delete the move pairs that are in the graph
    set<pair<int, int>> toDelete=set<pair<int, int>>(); //pairs to delete
    for (auto it: movePairs) {
        int src = it.first; int dst = it.second;
        if (graph.find(src) != graph.end() && graph[src].find(dst) != graph[src].end()) {
            toDelete.insert(it);
        }
    }
    for (auto it: toDelete) {
        movePairs.erase(it); //remove the move pair
        movePairs.erase(make_pair(it.second, it.first)); //remove the inverse move pair
    }
#ifdef DEBUG
    cout << "Move pairs after deleting invalid ones: " << endl;
    for (auto it: movePairs) {
        cout << it.first << " -> " << it.second << endl;
    }
    cout << "Interference graph: " << endl;
    for (auto it: graph) {
        cout << it.first << ": ";
        for (auto n: it.second) {
            cout << n << " ";
        }
        cout << endl;
    }     
    cout << "Move pairs: " << endl;
    for (auto it: movePairs) {
        cout << it.first << " -> " << it.second << endl;
    }
#endif
    return new InterferenceGraph(graph, movePairs); //return the interference graph
}
