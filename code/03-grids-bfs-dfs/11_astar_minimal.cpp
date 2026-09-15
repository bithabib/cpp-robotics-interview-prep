#include <algorithm>   // std::reverse
#include <cassert>
#include <cstdlib>     // std::abs
#include <functional>  // std::greater
#include <iostream>
#include <queue>       // std::priority_queue
#include <string>
#include <tuple>
#include <utility>     // std::pair
#include <vector>

// A* on a 4-connected grid ('#' = wall). Every step costs 1.
// Returns the cost of the cheapest path from (sr,sc) to (gr,gc), or -1 if there
// is none, and fills `path` with the cells from start to goal inclusive.
int aStar4(const std::vector<std::string>& grid, int sr, int sc, int gr, int gc,
           std::vector<std::pair<int, int>>& path) {
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());
    const int dr[4] = {-1, 0, 1, 0};
    const int dc[4] = {0, 1, 0, -1};
    auto h = [&](int r, int c) { return std::abs(r - gr) + std::abs(c - gc); };  // rows+cols to goal

    std::vector<int> g(rows * cols, -1);        // best cost so far per cell; -1 = never reached
    std::vector<int> parent(rows * cols, -1);   // which cell we came from
    std::vector<char> closed(rows * cols, 0);   // 1 once a cell has been taken off the list for good
    using Entry = std::tuple<int, int, int, int>;   // (f, -g, r, c): smallest f first, then LARGEST g
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> open;

    g[sr * cols + sc] = 0;
    open.push({h(sr, sc), 0, sr, sc});
    while (!open.empty()) {
        const auto [f, negG, r, c] = open.top();   // the entry with the smallest f
        open.pop();
        const int idx = r * cols + c;
        if (closed[idx]) continue;                 // stale duplicate of a cell already done: skip it
        closed[idx] = 1;
        if (r == gr && c == gc) {                  // goal taken off the list: its g is the answer
            for (int i = idx; i != -1; i = parent[i]) path.push_back({i / cols, i % cols});
            std::reverse(path.begin(), path.end());  // we walked goal -> start, flip it
            return -negG;
        }
        for (int k = 0; k < 4; ++k) {
            const int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;   // outside
            if (grid[nr][nc] == '#') continue;                             // wall
            const int ni = nr * cols + nc;
            const int ng = -negG + 1;                                      // one more step
            if (g[ni] == -1 || ng < g[ni]) {       // first time here, or a cheaper way than before
                g[ni] = ng;
                parent[ni] = idx;
                open.push({ng + h(nr, nc), -ng, nr, nc});   // push even if ni is already in the list
            }
        }
    }
    return -1;   // list ran empty: the goal is unreachable
}

int main() {
    const std::vector<std::string> tiny = {
        "S...",
        ".##.",
        "...G"};
    std::vector<std::pair<int, int>> path;
    assert(aStar4(tiny, 0, 0, 2, 3, path) == 5);
    assert(path.size() == 6);
    assert(path.front() == std::make_pair(0, 0));
    assert(path.back() == std::make_pair(2, 3));
    for (const auto& [r, c] : path) std::cout << "(" << r << "," << c << ") ";
    std::cout << "\n";

    const std::vector<std::string> walled = {
        "S#.",
        ".#.",
        ".#G"};
    path.clear();
    assert(aStar4(walled, 0, 0, 2, 2, path) == -1);
    assert(path.empty());

    path.clear();
    assert(aStar4(tiny, 0, 0, 0, 0, path) == 0 && path.size() == 1);   // start == goal
    std::cout << "OK 11_astar_minimal.cpp\n";
    return 0;
}
