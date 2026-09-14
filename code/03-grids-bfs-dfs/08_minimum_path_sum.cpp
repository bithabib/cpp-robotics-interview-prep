// 08_minimum_path_sum.cpp — LeetCode 64. Minimum Path Sum (medium) — DP
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

// dp[c] = cheapest cost to reach (current row, c). Rolling row again:
// before the update dp[c] is the cost from above, dp[c-1] the cost from the left.
int minPathSum(std::vector<std::vector<int>>& grid) {
    const int rows = static_cast<int>(grid.size());
    const int cols = rows ? static_cast<int>(grid[0].size()) : 0;
    if (rows == 0 || cols == 0) return 0;
    std::vector<int> dp(cols, 0);
    dp[0] = grid[0][0];
    for (int c = 1; c < cols; ++c) dp[c] = dp[c - 1] + grid[0][c];  // first row: only from the left
    for (int r = 1; r < rows; ++r) {
        dp[0] += grid[r][0];  // first column: only from above
        for (int c = 1; c < cols; ++c) dp[c] = std::min(dp[c], dp[c - 1]) + grid[r][c];
    }
    return dp[cols - 1];
}

int main() {
    {
        std::vector<std::vector<int>> g = {{1, 3, 1}, {1, 5, 1}, {4, 2, 1}};
        assert(minPathSum(g) == 7);  // 1->3->1->1->1
    }
    {
        std::vector<std::vector<int>> g = {{1, 2, 3}, {4, 5, 6}};
        assert(minPathSum(g) == 12);
    }
    {
        std::vector<std::vector<int>> g = {{5}};
        assert(minPathSum(g) == 5);
    }
    {
        std::vector<std::vector<int>> g = {{1}, {2}, {3}};
        assert(minPathSum(g) == 6);  // single column
    }
    {
        std::vector<std::vector<int>> g = {{9, 1, 1}, {9, 9, 1}, {9, 9, 1}};
        assert(minPathSum(g) == 13);  // hug the top and right edges
    }
    std::cout << "OK 08_minimum_path_sum.cpp\n";
    return 0;
}
