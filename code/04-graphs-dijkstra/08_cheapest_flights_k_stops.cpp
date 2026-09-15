// LeetCode 787. Cheapest Flights Within K Stops
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

// Bellman-Ford limited to k+1 rounds. Round i computes the cheapest price using at most i edges.
// Relaxing from a COPY of the previous round's dist is what stops a single round from
// chaining several edges together (which would break the stop limit).
int findCheapestPrice(int n, std::vector<std::vector<int>>& flights, int src, int dst, int k) {
    const int INF = std::numeric_limits<int>::max();
    std::vector<int> dist(n, INF);
    dist[src] = 0;
    for (int round = 0; round <= k; ++round) {                   // k stops == k+1 flights
        std::vector<int> next = dist;
        for (const auto& f : flights) {
            int u = f[0], v = f[1], w = f[2];
            if (dist[u] != INF && dist[u] + w < next[v]) next[v] = dist[u] + w;   // INF check avoids overflow
        }
        dist = std::move(next);
    }
    return dist[dst] == INF ? -1 : dist[dst];
}

int main() {
    std::vector<std::vector<int>> f1 = {{0, 1, 100}, {1, 2, 100}, {2, 0, 100}, {1, 3, 600}, {2, 3, 200}};
    assert(findCheapestPrice(4, f1, 0, 3, 1) == 700);            // 0->1->3; 0->1->2->3 needs 2 stops

    std::vector<std::vector<int>> f2 = {{0, 1, 100}, {1, 2, 100}, {0, 2, 500}};
    assert(findCheapestPrice(3, f2, 0, 2, 1) == 200);
    assert(findCheapestPrice(3, f2, 0, 2, 0) == 500);            // no stops allowed: direct flight only

    std::vector<std::vector<int>> f3 = {{0, 1, 1}, {1, 2, 1}};   // dst not reachable within limit
    assert(findCheapestPrice(3, f3, 0, 2, 0) == -1);
    assert(findCheapestPrice(3, f3, 2, 0, 5) == -1);             // not reachable at all (directed)

    // Where plain Dijkstra fails: cheapest path to 1 uses 2 stops, but the only
    // k-feasible route to 3 goes through 1 via the pricier direct edge.
    std::vector<std::vector<int>> f4 = {{0, 1, 5}, {0, 2, 1}, {2, 4, 1}, {4, 1, 1}, {1, 3, 1}};
    assert(findCheapestPrice(5, f4, 0, 3, 1) == 6);              // 0->1->3 (Dijkstra would settle 1 at cost 3 and find nothing)
    assert(findCheapestPrice(5, f4, 0, 3, 3) == 4);              // with enough stops the cheap route wins

    std::vector<std::vector<int>> none;                          // src == dst, no flights needed
    assert(findCheapestPrice(1, none, 0, 0, 0) == 0);

    std::cout << "OK 08_cheapest_flights_k_stops.cpp\n";
    return 0;
}
