// 10_pacific_atlantic.cpp — LeetCode 417. Pacific Atlantic Water Flow (medium)
// Reverse BFS from each ocean: instead of asking "can this cell reach the
// ocean?" (n*m searches), ask "which cells can the ocean climb up to?" (2 searches).
#include <cassert>
#include <iostream>
#include <queue>
#include <utility>
#include <vector>

// Multi-source BFS that moves only to neighbours with height >= current.
void climb(const std::vector<std::vector<int>>& h, std::vector<std::vector<char>>& reach,
           std::queue<std::pair<int, int>>& q) {
    const int rows = static_cast<int>(h.size());
    const int cols = static_cast<int>(h[0].size());
    constexpr int dr[4] = {-1, 0, 1, 0};
    constexpr int dc[4] = {0, 1, 0, -1};
    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();
        for (int k = 0; k < 4; ++k) {
            const int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) continue;
            if (reach[nr][nc] || h[nr][nc] < h[r][c]) continue;  // water flows downhill, so we climb uphill
            reach[nr][nc] = 1;
            q.push({nr, nc});
        }
    }
}

std::vector<std::vector<int>> pacificAtlantic(std::vector<std::vector<int>>& heights) {
    std::vector<std::vector<int>> out;
    const int rows = static_cast<int>(heights.size());
    const int cols = rows ? static_cast<int>(heights[0].size()) : 0;
    if (rows == 0 || cols == 0) return out;
    // vector<char> instead of vector<bool>: bool is a bit-packed proxy and you cannot take references into it.
    std::vector<std::vector<char>> pac(rows, std::vector<char>(cols, 0));
    std::vector<std::vector<char>> atl(rows, std::vector<char>(cols, 0));
    std::queue<std::pair<int, int>> qp, qa;
    for (int r = 0; r < rows; ++r) {
        pac[r][0] = 1; qp.push({r, 0});
        atl[r][cols - 1] = 1; qa.push({r, cols - 1});
    }
    for (int c = 0; c < cols; ++c) {
        if (!pac[0][c]) { pac[0][c] = 1; qp.push({0, c}); }
        if (!atl[rows - 1][c]) { atl[rows - 1][c] = 1; qa.push({rows - 1, c}); }
    }
    climb(heights, pac, qp);
    climb(heights, atl, qa);
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            if (pac[r][c] && atl[r][c]) out.push_back({r, c});
    return out;  // row-major order, which is what the tests below expect
}

int main() {
    {
        std::vector<std::vector<int>> h = {
            {1, 2, 2, 3, 5},
            {3, 2, 3, 4, 4},
            {2, 4, 5, 3, 1},
            {6, 7, 1, 4, 5},
            {5, 1, 1, 2, 4}};
        const std::vector<std::vector<int>> want = {{0, 4}, {1, 3}, {1, 4}, {2, 2}, {3, 0}, {3, 1}, {4, 0}};
        assert(pacificAtlantic(h) == want);
    }
    {
        std::vector<std::vector<int>> h = {{1}};
        const std::vector<std::vector<int>> want = {{0, 0}};  // touches both oceans
        assert(pacificAtlantic(h) == want);
    }
    {
        std::vector<std::vector<int>> h;
        assert(pacificAtlantic(h).empty());
    }
    {
        // Flat grid: every cell reaches both oceans.
        std::vector<std::vector<int>> h = {{1, 1}, {1, 1}};
        assert(pacificAtlantic(h).size() == 4);
    }
    {
        // A pit: the centre 0 cannot flow uphill to any neighbour, the 8 border cells can.
        std::vector<std::vector<int>> h = {{1, 1, 1}, {1, 0, 1}, {1, 1, 1}};
        const auto out = pacificAtlantic(h);
        assert(out.size() == 8);
        for (const auto& rc : out) assert(!(rc[0] == 1 && rc[1] == 1));
    }
    {
        // (0,0) is the lowest cell: it touches the Pacific but nothing lets it climb to the Atlantic side.
        std::vector<std::vector<int>> h = {{1, 2}, {3, 4}};
        const std::vector<std::vector<int>> want = {{0, 1}, {1, 0}, {1, 1}};
        assert(pacificAtlantic(h) == want);
    }
    std::cout << "OK 10_pacific_atlantic.cpp\n";
    return 0;
}
