// Graph toolkit demo: adjacency lists, BFS, DFS (iterative), Kahn's topological sort,
// union-find, and the Dijkstra template with path reconstruction.
#include <algorithm>
#include <cassert>
#include <deque>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <queue>
#include <utility>
#include <vector>

using Graph = std::vector<std::vector<int>>;                      // unweighted adjacency list
using WGraph = std::vector<std::vector<std::pair<int, int>>>;     // weighted: (neighbor, weight)

// ---------- building from edge lists ----------
Graph buildUndirected(int n, const std::vector<std::pair<int, int>>& edges) {
    Graph adj(n);
    for (const auto& [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);          // the only difference from a directed graph
    }
    return adj;
}

Graph buildDirected(int n, const std::vector<std::pair<int, int>>& edges) {
    Graph adj(n);
    for (const auto& [u, v] : edges) adj[u].push_back(v);
    return adj;
}

// ---------- BFS: shortest hop count from src on an unweighted graph ----------
std::vector<int> bfsDistances(const Graph& adj, int src) {
    std::vector<int> dist(adj.size(), -1);        // -1 == unreached, doubles as visited
    std::queue<int> q;
    dist[src] = 0;
    q.push(src);
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : adj[u]) {
            if (dist[v] != -1) continue;
            dist[v] = dist[u] + 1;                // mark when pushing, not when popping
            q.push(v);
        }
    }
    return dist;
}

// ---------- iterative DFS: preorder visit list ----------
std::vector<int> dfsOrder(const Graph& adj, int src) {
    std::vector<bool> visited(adj.size(), false);
    std::vector<int> order;
    std::vector<int> st = {src};
    while (!st.empty()) {
        int u = st.back();
        st.pop_back();
        if (visited[u]) continue;                  // a node can be pushed several times
        visited[u] = true;
        order.push_back(u);
        for (auto it = adj[u].rbegin(); it != adj[u].rend(); ++it)   // reverse so smallest is popped first
            if (!visited[*it]) st.push_back(*it);
    }
    return order;
}

// ---------- connected components on an undirected graph ----------
int countComponents(const Graph& adj) {
    int n = static_cast<int>(adj.size());
    std::vector<bool> visited(n, false);
    int count = 0;
    for (int s = 0; s < n; ++s) {
        if (visited[s]) continue;
        ++count;
        std::vector<int> st = {s};
        visited[s] = true;
        while (!st.empty()) {
            int u = st.back();
            st.pop_back();
            for (int v : adj[u])
                if (!visited[v]) { visited[v] = true; st.push_back(v); }
        }
    }
    return count;
}

// ---------- directed cycle detection with white/gray/black colouring ----------
bool hasDirectedCycle(const Graph& adj) {
    int n = static_cast<int>(adj.size());
    std::vector<int> color(n, 0);                  // 0 white, 1 gray (on stack), 2 black (done)
    std::function<bool(int)> dfs = [&](int u) {
        color[u] = 1;
        for (int v : adj[u]) {
            if (color[v] == 1) return true;        // back edge to an ancestor: cycle
            if (color[v] == 0 && dfs(v)) return true;
        }
        color[u] = 2;
        return false;
    };
    for (int s = 0; s < n; ++s)
        if (color[s] == 0 && dfs(s)) return true;
    return false;
}

// ---------- Kahn's topological sort; result shorter than n means a cycle ----------
std::vector<int> kahnTopo(const Graph& adj) {
    int n = static_cast<int>(adj.size());
    std::vector<int> indeg(n, 0);
    for (int u = 0; u < n; ++u)
        for (int v : adj[u]) ++indeg[v];
    std::queue<int> q;
    for (int u = 0; u < n; ++u)
        if (indeg[u] == 0) q.push(u);
    std::vector<int> order;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        order.push_back(u);
        for (int v : adj[u])
            if (--indeg[v] == 0) q.push(v);        // "removing" u frees v
    }
    return order;
}

// ---------- union-find with path compression and union by size ----------
struct DSU {
    std::vector<int> parent, sz;
    explicit DSU(int n) : parent(n), sz(n, 1) { std::iota(parent.begin(), parent.end(), 0); }
    int find(int x) {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];         // path halving: every step skips a level
            x = parent[x];
        }
        return x;
    }
    bool unite(int a, int b) {                     // false if a and b were already connected
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (sz[a] < sz[b]) std::swap(a, b);        // attach the smaller tree under the larger
        parent[b] = a;
        sz[a] += sz[b];
        return true;
    }
};

// ---------- the Dijkstra template (memorise this one) ----------
constexpr long long INF = std::numeric_limits<long long>::max() / 4;   // /4 so INF + w never overflows

std::vector<long long> dijkstra(const WGraph& adj, int src, std::vector<int>* parent = nullptr) {
    int n = static_cast<int>(adj.size());
    std::vector<long long> dist(n, INF);
    if (parent) parent->assign(n, -1);
    using State = std::pair<long long, int>;                         // (dist, node)
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;   // min-heap!
    dist[src] = 0;
    pq.push({0, src});
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;                 // stale entry: we already found a better path to u
        for (const auto& [v, w] : adj[u]) {
            long long nd = d + w;
            if (nd < dist[v]) {                    // relax
                dist[v] = nd;
                if (parent) (*parent)[v] = u;
                pq.push({nd, v});
            }
        }
    }
    return dist;
}

std::vector<int> reconstructPath(const std::vector<int>& parent, int target) {
    std::vector<int> path;
    for (int v = target; v != -1; v = parent[v]) path.push_back(v);
    std::reverse(path.begin(), path.end());
    return path;
}

// ---------- 0-1 BFS: weights are only 0 or 1, deque instead of heap ----------
std::vector<int> zeroOneBfs(const WGraph& adj, int src) {
    std::vector<int> dist(adj.size(), std::numeric_limits<int>::max());
    std::deque<int> dq;
    dist[src] = 0;
    dq.push_back(src);
    while (!dq.empty()) {
        int u = dq.front();
        dq.pop_front();
        for (const auto& [v, w] : adj[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                if (w == 0) dq.push_front(v); else dq.push_back(v);   // 0-edges keep the front
            }
        }
    }
    return dist;
}

// ---------- Bellman-Ford: V-1 rounds of relaxing every edge; handles negative weights ----------
struct Edge { int u, v, w; };
std::vector<long long> bellmanFord(int n, const std::vector<Edge>& edges, int src) {
    std::vector<long long> dist(n, INF);
    dist[src] = 0;
    for (int round = 0; round < n - 1; ++round)
        for (const auto& e : edges)
            if (dist[e.u] != INF && dist[e.u] + e.w < dist[e.v]) dist[e.v] = dist[e.u] + e.w;
    return dist;
}

int main() {
    // Undirected graph: 0-1, 0-2, 1-3, 2-3, 4-5 (two components)
    Graph g = buildUndirected(6, {{0, 1}, {0, 2}, {1, 3}, {2, 3}, {4, 5}});
    assert(bfsDistances(g, 0) == std::vector<int>({0, 1, 1, 2, -1, -1}));
    assert(dfsOrder(g, 0) == std::vector<int>({0, 1, 3, 2}));
    assert(countComponents(g) == 2);

    // Directed graphs: a DAG and one with a cycle
    Graph dag = buildDirected(4, {{0, 1}, {0, 2}, {1, 3}, {2, 3}});
    assert(!hasDirectedCycle(dag));
    std::vector<int> topo = kahnTopo(dag);
    assert(topo.size() == 4 && topo[0] == 0 && topo[3] == 3);
    Graph cyc = buildDirected(3, {{0, 1}, {1, 2}, {2, 0}});
    assert(hasDirectedCycle(cyc));
    assert(kahnTopo(cyc).empty());                 // nothing has in-degree 0 -> nothing emitted

    // Union-find: adding edges, the third closes a cycle
    DSU dsu(4);
    assert(dsu.unite(0, 1));
    assert(dsu.unite(1, 2));
    assert(!dsu.unite(0, 2));                      // already connected
    assert(dsu.find(0) == dsu.find(2) && dsu.find(3) != dsu.find(0));

    // Dijkstra with path reconstruction. Directed weighted graph:
    // 0->1 (4), 0->2 (1), 2->1 (2), 1->3 (1), 2->3 (5)
    WGraph wg(5);
    auto addEdge = [&](int u, int v, int w) { wg[u].push_back({v, w}); };
    addEdge(0, 1, 4); addEdge(0, 2, 1); addEdge(2, 1, 2); addEdge(1, 3, 1); addEdge(2, 3, 5);
    std::vector<int> parent;
    std::vector<long long> dist = dijkstra(wg, 0, &parent);
    assert((dist == std::vector<long long>({0, 3, 1, 4, INF})));   // node 4 unreachable
    assert(reconstructPath(parent, 3) == std::vector<int>({0, 2, 1, 3}));

    // Bellman-Ford agrees on the same graph
    std::vector<Edge> edges = {{0, 1, 4}, {0, 2, 1}, {2, 1, 2}, {1, 3, 1}, {2, 3, 5}};
    assert(bellmanFord(5, edges, 0) == dist);

    // 0-1 BFS: 0->1 (1), 0->2 (0), 2->1 (0)
    WGraph zg(3);
    zg[0].push_back({1, 1}); zg[0].push_back({2, 0}); zg[2].push_back({1, 0});
    assert(zeroOneBfs(zg, 0) == std::vector<int>({0, 0, 0}));

    // Edge case: single node graph
    WGraph one(1);
    assert(dijkstra(one, 0) == std::vector<long long>({0}));

    std::cout << "OK 00_graph_toolkit.cpp\n";
    return 0;
}
