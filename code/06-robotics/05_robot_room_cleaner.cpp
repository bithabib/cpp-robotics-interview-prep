// LeetCode 489. Robot Room Cleaner (hard, premium).
// The robot only knows: move() (true if it moved), turnLeft(), turnRight(), clean().
// It does not know the grid or its own position. Clean every reachable cell.
#include <cassert>
#include <cstdint>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <utility>
#include <vector>

// The interface exactly as LeetCode declares it.
class Robot {
public:
    virtual ~Robot() = default;
    virtual bool move() = 0;
    virtual void turnLeft() = 0;
    virtual void turnRight() = 0;
    virtual void clean() = 0;
};

class Solution {
public:
    void cleanRoom(Robot& robot) {
        visited_.clear();
        dfs(robot, 0, 0, 0);   // our own frame: start at (0,0) facing "up" (direction 0)
    }

private:
    // Clockwise order matters: turnRight() advances the direction index by one.
    static constexpr int dr[4] = {-1, 0, 1, 0};
    static constexpr int dc[4] = {0, 1, 0, -1};

    static std::int64_t key(int r, int c) {
        return (static_cast<std::int64_t>(r) << 32) ^ static_cast<std::uint32_t>(c);
    }

    void dfs(Robot& robot, int r, int c, int dir) {
        visited_.insert(key(r, c));
        robot.clean();
        for (int i = 0; i < 4; ++i) {
            const int nd = (dir + i) % 4;            // the direction the robot is facing right now
            const int nr = r + dr[nd], nc = c + dc[nd];
            if (!visited_.count(key(nr, nc)) && robot.move()) {
                dfs(robot, nr, nc, nd);
                goBack(robot);                       // physical backtrack: the robot must return
            }
            robot.turnRight();                       // after 4 turns we face `dir` again
        }
    }

    // Turn around, step back, turn around again: heading is preserved.
    static void goBack(Robot& robot) {
        robot.turnRight();
        robot.turnRight();
        robot.move();
        robot.turnRight();
        robot.turnRight();
    }

    std::unordered_set<std::int64_t> visited_;
};

// ---------- test-side simulator ----------
class GridRobot : public Robot {
public:
    GridRobot(std::vector<std::vector<int>> grid, int row, int col)
        : grid_(std::move(grid)), cleaned_(grid_.size(), std::vector<bool>(grid_[0].size(), false)),
          r_(row), c_(col) {}

    bool move() override {
        const int nr = r_ + dr[dir_], nc = c_ + dc[dir_];
        if (nr < 0 || nc < 0 || nr >= static_cast<int>(grid_.size()) ||
            nc >= static_cast<int>(grid_[0].size()) || grid_[nr][nc] == 0)
            return false;
        r_ = nr; c_ = nc;
        return true;
    }
    void turnLeft() override { dir_ = (dir_ + 3) % 4; }
    void turnRight() override { dir_ = (dir_ + 1) % 4; }
    void clean() override { cleaned_[r_][c_] = true; }

    bool cleaned(int r, int c) const { return cleaned_[r][c]; }

private:
    static constexpr int dr[4] = {-1, 0, 1, 0};
    static constexpr int dc[4] = {0, 1, 0, -1};
    std::vector<std::vector<int>> grid_;
    std::vector<std::vector<bool>> cleaned_;
    int r_, c_, dir_ = 0;   // 0 = up, matching the problem statement
};

// Ground truth: which cells are reachable from the start (plain BFS in the test).
std::vector<std::vector<bool>> reachable(const std::vector<std::vector<int>>& grid, int row, int col) {
    const int R = static_cast<int>(grid.size()), C = static_cast<int>(grid[0].size());
    std::vector<std::vector<bool>> seen(R, std::vector<bool>(C, false));
    std::queue<std::pair<int, int>> q;
    seen[row][col] = true;
    q.push({row, col});
    const int dr[4] = {-1, 0, 1, 0}, dc[4] = {0, 1, 0, -1};
    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();
        for (int k = 0; k < 4; ++k) {
            const int nr = r + dr[k], nc = c + dc[k];
            if (nr < 0 || nc < 0 || nr >= R || nc >= C || grid[nr][nc] == 0 || seen[nr][nc]) continue;
            seen[nr][nc] = true;
            q.push({nr, nc});
        }
    }
    return seen;
}

void check(const std::vector<std::vector<int>>& grid, int row, int col) {
    GridRobot robot(grid, row, col);
    Solution().cleanRoom(robot);
    const auto truth = reachable(grid, row, col);
    for (int r = 0; r < static_cast<int>(grid.size()); ++r)
        for (int c = 0; c < static_cast<int>(grid[0].size()); ++c)
            assert(robot.cleaned(r, c) == truth[r][c]);   // every reachable cell, nothing else
}

int main() {
    // LeetCode example 1
    check({{1, 1, 1, 1, 1, 0, 1, 1},
           {1, 1, 1, 1, 1, 0, 1, 1},
           {1, 0, 1, 1, 1, 1, 1, 1},
           {0, 0, 0, 1, 0, 0, 0, 0},
           {1, 1, 1, 1, 1, 1, 1, 1}}, 1, 3);
    // LeetCode example 2: single open cell
    check({{1}}, 0, 0);
    // Corridor with dead ends: forces deep backtracking
    check({{1, 1, 1, 1, 1},
           {1, 0, 0, 0, 1},
           {1, 0, 1, 0, 1},
           {1, 0, 1, 0, 1},
           {1, 1, 1, 1, 1}}, 2, 2);   // start on the inner pillar; it joins the ring via row 4, so everything is cleaned
    check({{1, 1, 1, 1, 1},
           {1, 0, 0, 0, 1},
           {1, 0, 1, 0, 1},
           {1, 0, 1, 0, 1},
           {1, 1, 1, 1, 1}}, 0, 0);   // start on the ring: ring cleaned, island not

    std::cout << "OK 05_robot_room_cleaner.cpp\n";
    return 0;
}
