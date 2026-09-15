// LeetCode 1631. Path With Minimum Effort
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <tuple>
#include <vector>

// Minimax Dijkstra on the grid: the "distance" of a path is the largest step along it,
// so relaxing uses max(d, step) instead of d + step. Everything else is the template.
int minimumEffortPath(std::vector<std::vector<int>>& heights) {
    int rows = static_cast<int>(heights.size());
    int cols = static_cast<int>(heights[0].size());
    const int INF = std::numeric_limits<int>::max();
    std::vector<std::vector<int>> effort(rows, std::vector<int>(cols, INF));
    using State = std::tuple<int, int, int>;                     // (effort, r, c)
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    effort[0][0] = 0;
    pq.push({0, 0, 0});
    const int dr[4] = {1, -1, 0, 0};
    const int dc[4] = {0, 0, 1, -1};
    while (!pq.empty()) {
        auto [e, r, c] = pq.top();
        pq.pop();
        if (r == rows - 1 && c == cols - 1) return e;            // early exit: first pop of target is optimal
        if (e > effort[r][c]) continue;
        for (int k = 0; k < 4; ++k) {
            int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
            int ne = std::max(e, std::abs(heights[nr][nc] - heights[r][c]));
            if (ne < effort[nr][nc]) {
                effort[nr][nc] = ne;
                pq.push({ne, nr, nc});
            }
        }
    }
    return effort[rows - 1][cols - 1];                           // unreachable for a grid, keeps -Wall quiet
}

// Alternative: binary search the answer, BFS to check "can I cross using only steps <= limit?".
bool reachable(const std::vector<std::vector<int>>& h, int limit) {
    int rows = static_cast<int>(h.size()), cols = static_cast<int>(h[0].size());
    std::vector<std::vector<bool>> seen(rows, std::vector<bool>(cols, false));
    std::queue<std::pair<int, int>> q;
    q.push({0, 0});
    seen[0][0] = true;
    const int dr[4] = {1, -1, 0, 0}, dc[4] = {0, 0, 1, -1};
    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();
        if (r == rows - 1 && c == cols - 1) return true;
        for (int k = 0; k < 4; ++k) {
            int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols || seen[nr][nc]) continue;
            if (std::abs(h[nr][nc] - h[r][c]) > limit) continue;
            seen[nr][nc] = true;
            q.push({nr, nc});
        }
    }
    return false;
}

int minimumEffortPathBinarySearch(std::vector<std::vector<int>>& heights) {
    int lo = 0, hi = 1000000;                                    // heights <= 1e6 per the constraints
    while (lo < hi) {                                            // smallest limit that is reachable
        int mid = lo + (hi - lo) / 2;
        if (reachable(heights, mid)) hi = mid; else lo = mid + 1;
    }
    return lo;
}

int main() {
    std::vector<std::vector<int>> h1 = {{1, 2, 2}, {3, 8, 2}, {5, 3, 5}};
    assert(minimumEffortPath(h1) == 2 && minimumEffortPathBinarySearch(h1) == 2);

    std::vector<std::vector<int>> h2 = {{1, 2, 3}, {3, 8, 4}, {5, 3, 5}};
    assert(minimumEffortPath(h2) == 1 && minimumEffortPathBinarySearch(h2) == 1);

    std::vector<std::vector<int>> h3 = {{1, 2, 1, 1, 1}, {1, 2, 1, 2, 1}, {1, 2, 1, 2, 1},
                                        {1, 2, 1, 2, 1}, {1, 1, 1, 2, 1}};
    assert(minimumEffortPath(h3) == 0 && minimumEffortPathBinarySearch(h3) == 0);

    std::vector<std::vector<int>> h4 = {{7}};                    // single cell: no steps at all
    assert(minimumEffortPath(h4) == 0 && minimumEffortPathBinarySearch(h4) == 0);

    std::vector<std::vector<int>> h5 = {{1, 10, 6, 7, 9, 10, 4, 9}};   // one row: forced path, answer is max step
    assert(minimumEffortPath(h5) == 9 && minimumEffortPathBinarySearch(h5) == 9);

    std::cout << "OK 06_path_with_minimum_effort.cpp\n";
    return 0;
}
