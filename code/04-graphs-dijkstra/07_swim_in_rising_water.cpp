// LeetCode 778. Swim in Rising Water
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <tuple>
#include <vector>

// Same minimax idea as 1631, but the cost of a path is the max CELL value on it (not max edge):
// the water must have risen to at least grid[r][c] to stand on (r, c). Dijkstra with max().
int swimInWater(std::vector<std::vector<int>>& grid) {
    int n = static_cast<int>(grid.size());
    const int INF = std::numeric_limits<int>::max();
    std::vector<std::vector<int>> best(n, std::vector<int>(n, INF));
    using State = std::tuple<int, int, int>;                     // (time, r, c)
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    best[0][0] = grid[0][0];
    pq.push({grid[0][0], 0, 0});
    const int dr[4] = {1, -1, 0, 0}, dc[4] = {0, 0, 1, -1};
    while (!pq.empty()) {
        auto [t, r, c] = pq.top();
        pq.pop();
        if (r == n - 1 && c == n - 1) return t;
        if (t > best[r][c]) continue;
        for (int k = 0; k < 4; ++k) {
            int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nr >= n || nc < 0 || nc >= n) continue;
            int nt = std::max(t, grid[nr][nc]);
            if (nt < best[nr][nc]) {
                best[nr][nc] = nt;
                pq.push({nt, nr, nc});
            }
        }
    }
    return best[n - 1][n - 1];
}

// Alternative: binary search on t, DFS over cells with value <= t.
bool canReach(const std::vector<std::vector<int>>& g, int t) {
    int n = static_cast<int>(g.size());
    if (g[0][0] > t) return false;
    std::vector<std::vector<bool>> seen(n, std::vector<bool>(n, false));
    std::vector<std::pair<int, int>> st = {{0, 0}};
    seen[0][0] = true;
    const int dr[4] = {1, -1, 0, 0}, dc[4] = {0, 0, 1, -1};
    while (!st.empty()) {
        auto [r, c] = st.back();
        st.pop_back();
        if (r == n - 1 && c == n - 1) return true;
        for (int k = 0; k < 4; ++k) {
            int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nr >= n || nc < 0 || nc >= n || seen[nr][nc] || g[nr][nc] > t) continue;
            seen[nr][nc] = true;
            st.push_back({nr, nc});
        }
    }
    return false;
}

int swimInWaterBinarySearch(std::vector<std::vector<int>>& grid) {
    int n = static_cast<int>(grid.size());
    int lo = 0, hi = n * n - 1;                                  // grid is a permutation of 0..n*n-1
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (canReach(grid, mid)) hi = mid; else lo = mid + 1;
    }
    return lo;
}

int main() {
    std::vector<std::vector<int>> g1 = {{0, 2}, {1, 3}};
    assert(swimInWater(g1) == 3 && swimInWaterBinarySearch(g1) == 3);

    std::vector<std::vector<int>> g2 = {{0, 1, 2, 3, 4}, {24, 23, 22, 21, 5}, {12, 13, 14, 15, 16},
                                        {11, 17, 18, 19, 20}, {10, 9, 8, 7, 6}};
    assert(swimInWater(g2) == 16 && swimInWaterBinarySearch(g2) == 16);

    std::vector<std::vector<int>> g3 = {{0}};                    // single cell
    assert(swimInWater(g3) == 0 && swimInWaterBinarySearch(g3) == 0);

    std::vector<std::vector<int>> g4 = {{3, 2}, {0, 1}};         // start cell itself is the bottleneck
    assert(swimInWater(g4) == 3 && swimInWaterBinarySearch(g4) == 3);

    std::cout << "OK 07_swim_in_rising_water.cpp\n";
    return 0;
}
