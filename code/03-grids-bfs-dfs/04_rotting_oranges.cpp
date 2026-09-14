// 04_rotting_oranges.cpp — LeetCode 994. Rotting Oranges (medium) — multi-source BFS
#include <cassert>
#include <iostream>
#include <queue>
#include <utility>
#include <vector>

// Every rotten orange is a BFS source at time 0. Process the queue level by
// level; each level is one minute. The answer is the number of levels that
// actually rotted something, and -1 if any fresh orange survives.
int orangesRotting(std::vector<std::vector<int>>& grid) {
    const int rows = static_cast<int>(grid.size());
    const int cols = rows ? static_cast<int>(grid[0].size()) : 0;
    std::queue<std::pair<int, int>> q;
    int fresh = 0;
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c] == 2) q.push({r, c});
            else if (grid[r][c] == 1) ++fresh;
        }
    constexpr int dr[4] = {-1, 0, 1, 0};
    constexpr int dc[4] = {0, 1, 0, -1};
    int minutes = 0;
    while (!q.empty() && fresh > 0) {
        const int levelSize = static_cast<int>(q.size());  // snapshot: the queue grows inside
        for (int i = 0; i < levelSize; ++i) {
            auto [r, c] = q.front();
            q.pop();
            for (int k = 0; k < 4; ++k) {
                const int nr = r + dr[k], nc = c + dc[k];
                if (nr < 0 || nr >= rows || nc < 0 || nc >= cols || grid[nr][nc] != 1) continue;
                grid[nr][nc] = 2;  // the grid itself is the visited set
                --fresh;
                q.push({nr, nc});
            }
        }
        ++minutes;
    }
    return fresh == 0 ? minutes : -1;
}

int main() {
    {
        std::vector<std::vector<int>> g = {{2, 1, 1}, {1, 1, 0}, {0, 1, 1}};
        assert(orangesRotting(g) == 4);
    }
    {
        std::vector<std::vector<int>> g = {{2, 1, 1}, {0, 1, 1}, {1, 0, 1}};
        assert(orangesRotting(g) == -1);  // bottom-left orange is isolated
    }
    {
        std::vector<std::vector<int>> g = {{0, 2}};
        assert(orangesRotting(g) == 0);  // nothing fresh: zero minutes
    }
    {
        std::vector<std::vector<int>> g = {{1}};
        assert(orangesRotting(g) == -1);  // fresh but no source
    }
    {
        std::vector<std::vector<int>> g = {{2, 1, 1, 1, 2}};
        assert(orangesRotting(g) == 2);  // two sources meet in the middle
    }
    std::cout << "OK 04_rotting_oranges.cpp\n";
    return 0;
}
