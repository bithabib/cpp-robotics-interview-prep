// LeetCode 505. The Maze II (medium, premium). Ball rolls until it hits a wall;
// return the minimum number of empty cells travelled to stop at the destination, or -1.
// Also LeetCode 490. The Maze: same rolling rule, just "can it stop there?" -> BFS.
#include <cassert>
#include <iostream>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>

constexpr int kDr[4] = {-1, 1, 0, 0};
constexpr int kDc[4] = {0, 0, -1, 1};

// Roll from (r,c) in direction k until the next cell is a wall or outside.
// Returns the stopping cell and how many cells were traversed.
std::tuple<int, int, int> roll(const std::vector<std::vector<int>>& maze, int r, int c, int k) {
    const int R = static_cast<int>(maze.size()), C = static_cast<int>(maze[0].size());
    int steps = 0;
    while (true) {
        const int nr = r + kDr[k], nc = c + kDc[k];
        if (nr < 0 || nc < 0 || nr >= R || nc >= C || maze[nr][nc] == 1) break;
        r = nr; c = nc; ++steps;
    }
    return {r, c, steps};
}

// 505: Dijkstra. Nodes are cells the ball can stop at; an edge is one roll, weight = its length.
int shortestDistance(std::vector<std::vector<int>>& maze, std::vector<int>& start, std::vector<int>& destination) {
    const int R = static_cast<int>(maze.size()), C = static_cast<int>(maze[0].size());
    const int INF = 1 << 30;
    std::vector<std::vector<int>> dist(R, std::vector<int>(C, INF));
    using Item = std::tuple<int, int, int>;                          // (dist, r, c)
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;
    dist[start[0]][start[1]] = 0;
    pq.push({0, start[0], start[1]});
    while (!pq.empty()) {
        auto [d, r, c] = pq.top();
        pq.pop();
        if (d > dist[r][c]) continue;                                // stale entry (lazy deletion)
        if (r == destination[0] && c == destination[1]) return d;    // first pop of the target is optimal
        for (int k = 0; k < 4; ++k) {
            auto [nr, nc, len] = roll(maze, r, c, k);
            if (len == 0) continue;                                  // wall right next to us
            if (d + len < dist[nr][nc]) {
                dist[nr][nc] = d + len;
                pq.push({d + len, nr, nc});
            }
        }
    }
    return -1;
}

// 490: plain BFS over stopping cells (edge weights irrelevant, only reachability).
bool hasPath(std::vector<std::vector<int>>& maze, std::vector<int>& start, std::vector<int>& destination) {
    const int R = static_cast<int>(maze.size()), C = static_cast<int>(maze[0].size());
    std::vector<std::vector<bool>> seen(R, std::vector<bool>(C, false));
    std::queue<std::pair<int, int>> q;
    seen[start[0]][start[1]] = true;
    q.push({start[0], start[1]});
    while (!q.empty()) {
        auto [r, c] = q.front();
        q.pop();
        if (r == destination[0] && c == destination[1]) return true;
        for (int k = 0; k < 4; ++k) {
            auto [nr, nc, len] = roll(maze, r, c, k);
            if (seen[nr][nc]) continue;
            seen[nr][nc] = true;
            q.push({nr, nc});
        }
    }
    return false;
}

int main() {
    std::vector<std::vector<int>> maze1 = {{0, 0, 1, 0, 0},
                                           {0, 0, 0, 0, 0},
                                           {0, 0, 0, 1, 0},
                                           {1, 1, 0, 1, 1},
                                           {0, 0, 0, 0, 0}};
    std::vector<int> s1 = {0, 4}, d1 = {4, 4};
    assert(shortestDistance(maze1, s1, d1) == 12);       // LeetCode example 1
    assert(hasPath(maze1, s1, d1));
    std::vector<int> d2 = {3, 2};
    assert(shortestDistance(maze1, s1, d2) == -1);       // example 2: cannot stop there
    assert(!hasPath(maze1, s1, d2));

    std::vector<std::vector<int>> maze3 = {{0, 0, 0, 0, 0},
                                           {1, 1, 0, 0, 1},
                                           {0, 0, 0, 0, 0},
                                           {0, 1, 0, 0, 1},
                                           {0, 1, 0, 0, 0}};
    std::vector<int> s3 = {4, 3}, d3 = {0, 1};
    assert(shortestDistance(maze3, s3, d3) == -1);       // example 3

    std::vector<std::vector<int>> one = {{0}};
    std::vector<int> z = {0, 0};
    assert(shortestDistance(one, z, z) == 0);            // start == destination
    assert(hasPath(one, z, z));

    std::vector<std::vector<int>> corridor = {{0, 0, 0, 0}};
    std::vector<int> cs = {0, 0}, cd = {0, 3};
    assert(shortestDistance(corridor, cs, cd) == 3);     // one roll to the end wall

    std::cout << "OK 06_the_maze_ii.cpp\n";
    return 0;
}
