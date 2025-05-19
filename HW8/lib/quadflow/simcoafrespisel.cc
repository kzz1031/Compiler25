#define DEBUG
#undef DEBUG

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
#else
#define DEBUG_OUTPUT(x)
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
    //DEBUG_OUTPUT("Coalescing...");
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
        
        bool safe = (highDegreeNeighbors < k);  
        if (safe) {
            DEBUG_OUTPUT("Coalescing " << u << " and " << v);
            for (int n : vNeighbors) {
                if (n != u) {
                    addEdge(u, n);
                }
            }
            
            if (coalescedMoves.find(u) == coalescedMoves.end()) {
                coalescedMoves[u] = set<int>();
            }
            coalescedMoves[u].insert(v);
            eraseNode(v);    
            auto toRemove = it;
            ++it;
            movePairs.erase(toRemove);
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
            DEBUG_OUTPUT("Frozen move " << node);
            return true; // Successfully froze a node
        }
    }
    
    return false;
}

//This is a soft spill: we just remove the node from the graph and add it to the simplified nodes
//as if nothing happened. The actual spill happens when select&coloring
bool Coloring::spill() {
    // Find a node with highest degree to spill
    int maxDegree = 0;
    int spillNode = -1;
    DEBUG_OUTPUT("Spilling...");
    for (auto& pair : graph) {
        int node = pair.first;

        if (isMachineReg(node)){
            DEBUG_OUTPUT("Skipping machine register " << node);
            continue; 
        } 
        // Find node with highest degree
        if (pair.second.size() > maxDegree) {
            maxDegree = pair.second.size();
            spillNode = node;
        }
    }
    
    if (spillNode != -1) {
        simplifiedNodes.push(spillNode);
        eraseNode(spillNode);
        DEBUG_OUTPUT("Spilled node " << spillNode);
        return true;
    }
    return false;
}

//now try to select the registers for the nodes
//finally check the validity of the coloring
bool Coloring::select() {
    // Initialize colors for machine registers
    for (int i = 0; i < 4; i++) {
        colors[i] = i; // Machine registers are pre-colored
    }
    
    // Process nodes in reverse order of simplification
    while (!simplifiedNodes.empty()) {
        int node = simplifiedNodes.top();
        simplifiedNodes.pop();
        
        // Skip if node is a machine register (already colored)
        if (isMachineReg(node)) continue;
        
        // Skip if node was coalesced
        bool wasCoalesced = false;
        for (auto& pair : coalescedMoves) {
            if (pair.second.find(node) != pair.second.end()) {
                wasCoalesced = true;
                break;
            }
        }
        if (wasCoalesced) continue;
        
        // 获取原始邻居
        set<int> allNeighbors;
        if (ig->graph.find(node) != ig->graph.end()) {
            allNeighbors = ig->graph[node];
        }
        else {
            DEBUG_OUTPUT("Node " << node << " not found in graph");
        }
        
        for (auto& pair : coalescedMoves) {
            int mainNode = pair.first;
            // 如果当前节点是合并的主节点，需要考虑所有被合并节点的邻居
            if (mainNode == node) {
                for (int coalescedNode : pair.second) {
                    if (ig->graph.find(coalescedNode) != ig->graph.end()) {
                        for (int neighbor : ig->graph[coalescedNode]) {
                            if (neighbor != node) {
                                allNeighbors.insert(neighbor);
                            }
                        }
                    }
                }
            }
            for (int coalescedNode : pair.second) {
                if (ig->graph.find(coalescedNode) != ig->graph.end() && 
                    ig->graph[coalescedNode].find(node) != ig->graph[coalescedNode].end()) {
                    allNeighbors.insert(mainNode);
                }
            }
        }
        
        // 找出已使用的颜色
        set<int> usedColors;
        for (int neighbor : allNeighbors) {
            // 如果邻居已着色，标记其颜色为已使用
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
                DEBUG_OUTPUT("Colored node " << node << " with color " << c);
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
                if (!isMachineReg(coalescedNode)) {
                    DEBUG_OUTPUT("Propagating color " << colors[mainNode] << " to coalesced node " << coalescedNode);
                    colors[coalescedNode] = colors[mainNode];
                }
            }
        } else if (spilled.find(mainNode) != spilled.end()) {
            for (int coalescedNode : pair.second) {
                if (!isMachineReg(coalescedNode)) {
                    spilled.insert(coalescedNode);
                }
            }
        }
    }
    
    return checkColoring();
}
