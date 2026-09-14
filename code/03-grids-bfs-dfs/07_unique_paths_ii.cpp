// 07_unique_paths_ii.cpp — LeetCode 63. Unique Paths II (medium) — DP with obstacles
#include <cassert>
#include <iostream>
#include <vector>

// dp[c] = number of ways to reach (current row, c) moving only right/down.
// Rolling a single row: dp[c] already holds "from above", add dp[c-1] "from left".
// An obstacle zeroes its cell so nothing flows through it.
int uniquePathsWithObstacles(std::vector<std::vector<int>>& obstacleGrid) {
    const int rows = static_cast<int>(obstacleGrid.size());
    const int cols = rows ? static_cast<int>(obstacleGrid[0].size()) : 0;
    if (rows == 0 || cols == 0 || obstacleGrid[0][0] == 1) return 0;
    std::vector<long long> dp(cols, 0);  // long long: counts can exceed int mid-grid even if the answer fits
    dp[0] = 1;
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c) {
            if (obstacleGrid[r][c] == 1) dp[c] = 0;
            else if (c > 0) dp[c] += dp[c - 1];
        }
    return static_cast<int>(dp[cols - 1]);
}

int main() {
    {
        std::vector<std::vector<int>> g = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}};
        assert(uniquePathsWithObstacles(g) == 2);
    }
    {
        std::vector<std::vector<int>> g = {{0, 1}, {0, 0}};
        assert(uniquePathsWithObstacles(g) == 1);
    }
    {
        std::vector<std::vector<int>> g = {{1}};
        assert(uniquePathsWithObstacles(g) == 0);  // start blocked
    }
    {
        std::vector<std::vector<int>> g = {{0}};
        assert(uniquePathsWithObstacles(g) == 1);  // 1x1 free: one empty path
    }
    {
        std::vector<std::vector<int>> g = {{0, 0}, {1, 1}, {0, 0}};
        assert(uniquePathsWithObstacles(g) == 0);  // a full wall row blocks everything
    }
    {
        // 3x7 without obstacles is the classic Unique Paths answer, C(8,2) = 28.
        std::vector<std::vector<int>> g(3, std::vector<int>(7, 0));
        assert(uniquePathsWithObstacles(g) == 28);
    }
    std::cout << "OK 07_unique_paths_ii.cpp\n";
    return 0;
}
