// 06_01_matrix.cpp — LeetCode 542. 01 Matrix (medium) — distance transform via multi-source BFS
#include <cassert>
#include <iostream>
#include <queue>
#include <utility>
#include <vector>

// Seed the queue with every 0 at distance 0, then expand. The first time BFS
// reaches a cell is from its nearest zero, so the distance is final on push.
std::vector<std::vector<int>> updateMatrix(std::vector<std::vector<int>>& mat) {
    const int rows = static_cast<int>(mat.size());
    const int cols = rows ? static_cast<int>(mat[0].size()) : 0;
    std::vector<std::vector<int>> dist(rows, std::vector<int>(cols, -1));
    std::queue<std::pair<int, int>> q;
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            if (mat[r][c] == 0) {
                dist[r][c] = 0;
                q.push({r, c});
            }
    constexpr int dr[4] = {-1, 0, 1, 0};
    constexpr int dc[4] = {0, 1, 0, -1};
    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();
        for (int k = 0; k < 4; ++k) {
            const int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nr >= rows || nc < 0 || nc >= cols || dist[nr][nc] != -1) continue;
            dist[nr][nc] = dist[r][c] + 1;
            q.push({nr, nc});
        }
    }
    return dist;
}

int main() {
    {
        std::vector<std::vector<int>> m = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}};
        const std::vector<std::vector<int>> want = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}};
        assert(updateMatrix(m) == want);
    }
    {
        std::vector<std::vector<int>> m = {{0, 0, 0}, {0, 1, 0}, {1, 1, 1}};
        const std::vector<std::vector<int>> want = {{0, 0, 0}, {0, 1, 0}, {1, 2, 1}};
        assert(updateMatrix(m) == want);
    }
    {
        std::vector<std::vector<int>> m = {{0}};
        assert(updateMatrix(m) == std::vector<std::vector<int>>{{0}});
    }
    {
        // A single zero in the corner: distances are Manhattan distances to it.
        std::vector<std::vector<int>> m = {{0, 1, 1}, {1, 1, 1}};
        const std::vector<std::vector<int>> want = {{0, 1, 2}, {1, 2, 3}};
        assert(updateMatrix(m) == want);
    }
    std::cout << "OK 06_01_matrix.cpp\n";
    return 0;
}
