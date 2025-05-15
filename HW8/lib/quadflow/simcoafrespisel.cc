#define DEBUG
//#undef DEBUG

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "temp.hh"
#include "ig.hh"
#include "coloring.hh"

#ifdef DEBUG
#define DEBUG_OUTPUT(x) std::cout << x << std::endl;
#endif
bool isAnEdge(map<int, set<int>>& graph, int src, int dst) {
    if (graph.find(src) == graph.end()) return false; //src not in the graph
    if (graph[src].find(dst) == graph[src].end()) return false; //dst not in the graph
    return true; //edge exists
}

//return true if any node is removed
bool Coloring::simplify() {
    bool removed = false;

    for (auto& pair : graph) {
        int node = pair.first;

        if (isMachineReg(node) || isMove(node)) continue;

        if (pair.second.size() < k) {
            simplifiedNodes.push(node);
            DEBUG_OUTPUT("Simplified node " << node);
            eraseNode(node);
            removed = true;
            break; 
        }
    }
    
    return removed;
}

//return true if changed anything, false otherwise
bool Coloring::coalesce() {
    // Brigg's coalescing: merge u,v if the resulting node will have fewer than k neighbors with degree ≥ k
    for (auto it = movePairs.begin(); it != movePairs.end(); ) {
        int u = it->first;
        int v = it->second;
        
        if (isMachineReg(u) && isMachineReg(v)) {
            ++it;
            continue;
        }
        
        if (isMachineReg(u)) {
            std::swap(u, v);
        }
        
        // Check if u and v interfere
        if (isAnEdge(graph, u, v)) {
            DEBUG_OUTPUT("Skipping move " << u << " -> " << v << " because they interfere");
            ++it;
            continue;
        }
        
        set<int> uNeighbors = getNeighbors(u);
        set<int> vNeighbors = getNeighbors(v);
        
        // Combine neighbors (excluding u and v themselves)
        set<int> combinedNeighbors;
        for (int n : uNeighbors) {
            if (n != v) combinedNeighbors.insert(n);
        }
        for (int n : vNeighbors) {
            if (n != u) combinedNeighbors.insert(n);
        }
        
        // 使用Brigg's策略检查合并是否安全
        // 计算合并后的节点的邻居中有多少个度数 >= k 的节点
        int highDegreeNeighbors = 0;
        
        for (int n : combinedNeighbors) {
            if (getNeighbors(n).size() >= k) {
                highDegreeNeighbors++;
            }
        }
        
        // 如果高度数邻居少于k个，则合并是安全的
        bool safe = (highDegreeNeighbors < k);
        
        if (safe) {
            // Coalesce v into u
            // Add all of v's neighbors to u
            for (int n : vNeighbors) {
                if (n != u) {
                    addEdge(u, n);
                }
            }
            
            // Record the coalescing
            if (coalescedMoves.find(u) == coalescedMoves.end()) {
                coalescedMoves[u] = set<int>();
            }
            coalescedMoves[u].insert(v);
            
            // Remove v from the graph
            eraseNode(v);
            
            // Remove this move pair
            auto toRemove = it;
            ++it;
            movePairs.erase(toRemove);
            
            // Remove the inverse move pair if it exists
            movePairs.erase(pair<int, int>(v, u));
            
            return true;
        } else {
            ++it;
        }
    }
    
    return false;
}

//freeze the moves that are not coalesced
//return true if changed anything, false otherwise
bool Coloring::freeze() {
    // Find a move-related node with degree < k
    for (auto& pair : graph) {
        int node = pair.first;
        
        // Skip machine registers and non-move-related nodes
        if (isMachineReg(node) || !isMove(node)) continue;
        
        // If degree < k, freeze this node
        if (pair.second.size() < k) {
            // Remove all move pairs involving this node
            set<std::pair<int, int>> toRemove;
            for (auto& move : movePairs) {
                if (move.first == node || move.second == node) {
                    toRemove.insert(move);
                }
            }
            
            for (auto& move : toRemove) {
                movePairs.erase(move);
            }
            
            return true; // Successfully froze a node
        }
    }
    
    return false;
}

//This is a soft spill: we just remove the node from the graph and add it to the simplified nodes
//as if nothing happened. The actual spill happens when select&coloring
bool Coloring::spill() {
    // Find a node with highest degree to spill
    int maxDegree = -1;
    int spillNode = -1;
    
    for (auto& pair : graph) {
        int node = pair.first;
        
        // Skip machine registers
        if (isMachineReg(node)) continue;
        
        // Find node with highest degree
        if (pair.second.size() > maxDegree) {
            maxDegree = pair.second.size();
            spillNode = node;
        }
    }
    
    if (spillNode != -1) {
        // Push to simplified nodes stack
        simplifiedNodes.push(spillNode);
        
        // Remove the node from the graph
        eraseNode(spillNode);
        
        return true;
    }
    
    return false;
}

//now try to select the registers for the nodes
//finally check the validity of the coloring
bool Coloring::select() {
    // Initialize colors for machine registers
    for (int i = 0; i < k; i++) {
        colors[i] = i; // Machine registers are pre-colored
    }
    
    // Process nodes in reverse order of simplification
    while (!simplifiedNodes.empty()) {
        int node = simplifiedNodes.top();
        simplifiedNodes.pop();
        
        // Skip if node was coalesced
        bool wasCoalesced = false;
        for (auto& pair : coalescedMoves) {
            if (pair.second.find(node) != pair.second.end()) {
                wasCoalesced = true;
                break;
            }
        }
        if (wasCoalesced) continue;
        
        // Get original neighbors from the original graph
        set<int> origNeighbors = ig->graph[node];
        
        // Find available colors
        set<int> usedColors;
        for (int neighbor : origNeighbors) {
            // If neighbor has been colored, mark its color as used
            if (colors.find(neighbor) != colors.end()) {
                usedColors.insert(colors[neighbor]);
            }
            
            // Check if neighbor was coalesced into another node
            for (auto& pair : coalescedMoves) {
                if (pair.second.find(neighbor) != pair.second.end()) {
                    if (colors.find(pair.first) != colors.end()) {
                        usedColors.insert(colors[pair.first]);
                    }
                }
            }
        }
        
        // Find an available color
        bool colorFound = false;
        for (int c = 0; c < k; c++) {
            if (usedColors.find(c) == usedColors.end()) {
                colors[node] = c;
                colorFound = true;
                break;
            }
        }
        
        // If no color found, mark as spilled
        if (!colorFound) {
            spilled.insert(node);
        }
    }
    
    // Propagate colors to coalesced nodes
    for (auto& pair : coalescedMoves) {
        int mainNode = pair.first;
        if (colors.find(mainNode) != colors.end()) {
            for (int coalescedNode : pair.second) {
                colors[coalescedNode] = colors[mainNode];
            }
        } else if (spilled.find(mainNode) != spilled.end()) {
            for (int coalescedNode : pair.second) {
                spilled.insert(coalescedNode);
            }
        }
    }
    
    return checkColoring();
}
