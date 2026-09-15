// LeetCode 133. Clone Graph
#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// LeetCode's definition
class Node {
public:
    int val;
    std::vector<Node*> neighbors;
    Node() : val(0), neighbors() {}
    explicit Node(int _val) : val(_val), neighbors() {}
    Node(int _val, std::vector<Node*> _neighbors) : val(_val), neighbors(std::move(_neighbors)) {}
};

// DFS with a map original -> copy. The map is both the "visited" set and the way
// to wire copied edges to already-created copies (which is what handles cycles).
Node* cloneGraph(Node* node) {
    if (!node) return nullptr;
    std::unordered_map<Node*, Node*> copies;
    std::vector<Node*> st = {node};
    copies[node] = new Node(node->val);
    while (!st.empty()) {
        Node* u = st.back();
        st.pop_back();
        for (Node* v : u->neighbors) {
            if (!copies.count(v)) {                 // first time we see v: create its copy, explore later
                copies[v] = new Node(v->val);
                st.push_back(v);
            }
            copies[u]->neighbors.push_back(copies[v]);
        }
    }
    return copies[node];
}

// ---------- test helpers ----------
// Build from LeetCode's 1-indexed adjacency list; returns all nodes (node 1 first).
std::vector<Node*> buildGraph(const std::vector<std::vector<int>>& adj) {
    std::vector<Node*> nodes;
    for (int i = 0; i < static_cast<int>(adj.size()); ++i) nodes.push_back(new Node(i + 1));
    for (int i = 0; i < static_cast<int>(adj.size()); ++i)
        for (int j : adj[i]) nodes[i]->neighbors.push_back(nodes[j - 1]);
    return nodes;
}

// Collect every node reachable from start (so we can free a clone we only hold a pointer into).
std::vector<Node*> collect(Node* start) {
    std::vector<Node*> out;
    if (!start) return out;
    std::unordered_set<Node*> seen = {start};
    std::vector<Node*> st = {start};
    while (!st.empty()) {
        Node* u = st.back();
        st.pop_back();
        out.push_back(u);
        for (Node* v : u->neighbors)
            if (seen.insert(v).second) st.push_back(v);
    }
    return out;
}

void freeAll(const std::vector<Node*>& nodes) { for (Node* n : nodes) delete n; }

// Structural equality: same vals, same neighbor vals in the same order, and NO shared pointers.
bool sameStructure(Node* a, Node* b) {
    if (!a || !b) return a == b;
    std::unordered_map<Node*, Node*> pairing;
    std::vector<std::pair<Node*, Node*>> st = {{a, b}};
    pairing[a] = b;
    while (!st.empty()) {
        auto [x, y] = st.back();
        st.pop_back();
        if (x == y) return false;                   // clone must not alias the original
        if (x->val != y->val || x->neighbors.size() != y->neighbors.size()) return false;
        for (int i = 0; i < static_cast<int>(x->neighbors.size()); ++i) {
            Node* nx = x->neighbors[i];
            Node* ny = y->neighbors[i];
            auto it = pairing.find(nx);
            if (it == pairing.end()) { pairing[nx] = ny; st.push_back({nx, ny}); }
            else if (it->second != ny) return false;
        }
    }
    return true;
}

int main() {
    // Example 1: 4-cycle with chords, adjList = [[2,4],[1,3],[2,4],[1,3]]
    std::vector<Node*> g1 = buildGraph({{2, 4}, {1, 3}, {2, 4}, {1, 3}});
    Node* c1 = cloneGraph(g1[0]);
    assert(sameStructure(g1[0], c1));
    assert(collect(c1).size() == 4);
    freeAll(collect(c1));
    freeAll(g1);

    // Example 2: single node, no neighbors
    std::vector<Node*> g2 = buildGraph({{}});
    Node* c2 = cloneGraph(g2[0]);
    assert(c2 != g2[0] && c2->val == 1 && c2->neighbors.empty());
    delete c2;
    freeAll(g2);

    // Example 3: empty graph
    assert(cloneGraph(nullptr) == nullptr);

    // Self-loop plus a chain: 1-1, 1-2, 2-3
    std::vector<Node*> g3 = buildGraph({{1, 2}, {1, 3}, {2}});
    Node* c3 = cloneGraph(g3[0]);
    assert(sameStructure(g3[0], c3));
    assert(c3->neighbors[0] == c3);                 // self-loop preserved and points at the copy
    freeAll(collect(c3));
    freeAll(g3);

    std::cout << "OK 01_clone_graph.cpp\n";
    return 0;
}
