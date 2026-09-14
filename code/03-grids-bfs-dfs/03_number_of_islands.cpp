// 03_number_of_islands.cpp — LeetCode 200. Number of Islands (medium)
#include <cassert>
#include <iostream>
#include <stack>
#include <utility>
#include <vector>

// Iterative DFS that sinks each island by overwriting '1' with '0' in place.
// Mutating the input is the cheapest visited set. Copy the grid first if the
// caller still needs the original.
void sink(std::vector<std::vector<char>>& g, int sr, int sc) {
    const int rows = static_cast<int>(g.size());
    const int cols = static_cast<int>(g[0].size());
    constexpr int dr[4] = {-1, 0, 1, 0};
    constexpr int dc[4] = {0, 1, 0, -1};
    std::stack<std::pair<int, int>> st;
    st.push({sr, sc});
    g[sr][sc] = '0';
    while (!st.empty()) {
        auto [r, c] = st.top();
        st.pop();
        for (int k = 0; k < 4; ++k) {
            const int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols || g[nr][nc] != '1') continue;
            g[nr][nc] = '0';  // mark when pushing so a cell is never pushed twice
            st.push({nr, nc});
        }
    }
}

int numIslands(std::vector<std::vector<char>>& grid) {
    if (grid.empty() || grid[0].empty()) return 0;
    const int rows = static_cast<int>(grid.size());
    const int cols = static_cast<int>(grid[0].size());
    int islands = 0;
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            if (grid[r][c] == '1') {
                ++islands;
                sink(grid, r, c);
            }
    return islands;
}

int main() {
    {
        std::vector<std::vector<char>> g = {
            {'1', '1', '1', '1', '0'},
            {'1', '1', '0', '1', '0'},
            {'1', '1', '0', '0', '0'},
            {'0', '0', '0', '0', '0'}};
        assert(numIslands(g) == 1);
    }
    {
        std::vector<std::vector<char>> g = {
            {'1', '1', '0', '0', '0'},
            {'1', '1', '0', '0', '0'},
            {'0', '0', '1', '0', '0'},
            {'0', '0', '0', '1', '1'}};
        assert(numIslands(g) == 3);  // diagonals do not connect
    }
    {
        std::vector<std::vector<char>> g;
        assert(numIslands(g) == 0);
    }
    {
        std::vector<std::vector<char>> g = {{'0'}};
        assert(numIslands(g) == 0);
    }
    {
        // Large all-land grid: recursion would be 250k deep; the stack version is fine.
        std::vector<std::vector<char>> g(500, std::vector<char>(500, '1'));
        assert(numIslands(g) == 1);
    }
    std::cout << "OK 03_number_of_islands.cpp\n";
    return 0;
}
