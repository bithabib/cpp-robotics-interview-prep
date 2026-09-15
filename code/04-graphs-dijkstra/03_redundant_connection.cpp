// LeetCode 684. Redundant Connection
#include <cassert>
#include <iostream>
#include <numeric>
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

// n nodes, n edges => exactly one cycle. Add edges in input order; the first edge whose
// endpoints are already connected is the one that closes that cycle, and it is by
// construction the last edge of the cycle in input order, which is what LeetCode asks for.
std::vector<int> findRedundantConnection(std::vector<std::vector<int>>& edges) {
    DSU dsu(static_cast<int>(edges.size()) + 1);    // nodes are 1..n, n == edges.size()
    for (const auto& e : edges)
        if (!dsu.unite(e[0], e[1])) return e;
    return {};
}

int main() {
    std::vector<std::vector<int>> e1 = {{1, 2}, {1, 3}, {2, 3}};
    assert(findRedundantConnection(e1) == std::vector<int>({2, 3}));

    std::vector<std::vector<int>> e2 = {{1, 2}, {2, 3}, {3, 4}, {1, 4}, {1, 5}};
    assert(findRedundantConnection(e2) == std::vector<int>({1, 4}));

    std::vector<std::vector<int>> e3 = {{1, 2}, {2, 3}, {1, 3}, {3, 4}, {4, 5}};   // cycle closed early
    assert(findRedundantConnection(e3) == std::vector<int>({1, 3}));

    std::vector<std::vector<int>> e4 = {{3, 4}, {1, 2}, {2, 4}, {3, 5}, {2, 5}};   // components merge before the cycle closes
    assert(findRedundantConnection(e4) == std::vector<int>({2, 5}));

    std::cout << "OK 03_redundant_connection.cpp\n";
    return 0;
}
