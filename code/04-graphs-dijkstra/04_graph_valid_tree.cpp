// LeetCode 261. Graph Valid Tree (premium)
#include <cassert>
#include <iostream>
#include <numeric>
#include <queue>
#include <utility>
#include <vector>

struct DSU {
    std::vector<int> parent, sz;
    explicit DSU(int n) : parent(n), sz(n, 1) { std::iota(parent.begin(), parent.end(), 0); }
    int find(int x) {
        while (parent[x] != x) { parent[x] = parent[parent[x]]; x = parent[x]; }
        return x;
    }
    bool unite(int a, int b) {
        a = find(a); b = find(b);
        if (a == b) return false;
        if (sz[a] < sz[b]) std::swap(a, b);
        parent[b] = a;
        sz[a] += sz[b];
        return true;
    }
};

// A tree on n nodes has exactly n-1 edges and no cycle (which then implies connected).
bool validTree(int n, std::vector<std::vector<int>>& edges) {
    if (static_cast<int>(edges.size()) != n - 1) return false;
    DSU dsu(n);
    for (const auto& e : edges)
        if (!dsu.unite(e[0], e[1])) return false;    // edge inside an existing component = cycle
    return true;                                     // n-1 successful unions => one component
}

// BFS alternative: connected AND n-1 edges. Same answer, shows the other way of thinking.
bool validTreeBfs(int n, std::vector<std::vector<int>>& edges) {
    if (static_cast<int>(edges.size()) != n - 1) return false;
    std::vector<std::vector<int>> adj(n);
    for (const auto& e : edges) { adj[e[0]].push_back(e[1]); adj[e[1]].push_back(e[0]); }
    std::vector<bool> visited(n, false);
    std::queue<int> q;
    q.push(0);
    visited[0] = true;
    int seen = 1;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : adj[u])
            if (!visited[v]) { visited[v] = true; ++seen; q.push(v); }
    }
    return seen == n;
}

int main() {
    std::vector<std::vector<int>> e1 = {{0, 1}, {0, 2}, {0, 3}, {1, 4}};
    assert(validTree(5, e1) && validTreeBfs(5, e1));

    std::vector<std::vector<int>> e2 = {{0, 1}, {1, 2}, {2, 3}, {1, 3}, {1, 4}};   // cycle 1-2-3
    assert(!validTree(5, e2) && !validTreeBfs(5, e2));

    std::vector<std::vector<int>> e3 = {{0, 1}, {2, 3}};      // too few edges: cannot be connected
    assert(!validTree(4, e3) && !validTreeBfs(4, e3));

    std::vector<std::vector<int>> e4 = {{0, 1}, {2, 3}, {1, 2}, {0, 3}};   // edge count is n-1 but there is a cycle (and node 4 is isolated)
    assert(!validTree(5, e4) && !validTreeBfs(5, e4));

    std::vector<std::vector<int>> e5;                          // single node, no edges: a tree
    assert(validTree(1, e5) && validTreeBfs(1, e5));

    std::cout << "OK 04_graph_valid_tree.cpp\n";
    return 0;
}
