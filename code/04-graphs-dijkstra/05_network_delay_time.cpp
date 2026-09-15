// LeetCode 743. Network Delay Time
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

// Plain Dijkstra from k; answer is the largest finite distance (the last node to hear the signal).
int networkDelayTime(std::vector<std::vector<int>>& times, int n, int k) {
    std::vector<std::vector<std::pair<int, int>>> adj(n + 1);   // 1-indexed nodes
    for (const auto& t : times) adj[t[0]].push_back({t[1], t[2]});

    const int INF = std::numeric_limits<int>::max();
    std::vector<int> dist(n + 1, INF);
    using State = std::pair<int, int>;                           // (dist, node)
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    dist[k] = 0;
    pq.push({0, k});
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;                               // stale heap entry
        for (const auto& [v, w] : adj[u]) {
            if (d + w < dist[v]) {                               // d is finite here, so no overflow
                dist[v] = d + w;
                pq.push({dist[v], v});
            }
        }
    }
    int ans = 0;
    for (int v = 1; v <= n; ++v) {
        if (dist[v] == INF) return -1;                           // some node never reached
        ans = std::max(ans, dist[v]);
    }
    return ans;
}

int main() {
    std::vector<std::vector<int>> t1 = {{2, 1, 1}, {2, 3, 1}, {3, 4, 1}};
    assert(networkDelayTime(t1, 4, 2) == 2);

    std::vector<std::vector<int>> t2 = {{1, 2, 1}};
    assert(networkDelayTime(t2, 2, 1) == 1);
    assert(networkDelayTime(t2, 2, 2) == -1);                    // edge is directed: 2 cannot reach 1

    std::vector<std::vector<int>> t3 = {{1, 2, 1}, {2, 3, 2}, {1, 3, 4}};   // direct edge loses to the 2-hop path
    assert(networkDelayTime(t3, 3, 1) == 3);

    std::vector<std::vector<int>> t4;                            // single node, no edges
    assert(networkDelayTime(t4, 1, 1) == 0);

    std::cout << "OK 05_network_delay_time.cpp\n";
    return 0;
}
