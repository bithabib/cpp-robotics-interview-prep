// 00_grid_toolkit.cpp — the grid-as-graph toolkit: direction arrays, bounds
// check, BFS distances (single- and multi-source), recursive + iterative DFS,
// and a flat visited array. Everything else in this chapter is a variation.
#include <cassert>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <utility>
#include <vector>

using Grid = std::vector<std::string>;  // '.' free, '#' blocked

// 4-connectivity: up, right, down, left. Index i pairs dr4[i] with dc4[i].
constexpr int dr4[4] = {-1, 0, 1, 0};
constexpr int dc4[4] = {0, 1, 0, -1};
// 8-connectivity adds the diagonals.
constexpr int dr8[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
constexpr int dc8[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

bool inBounds(int r, int c, int rows, int cols) {
    return r >= 0 && r < rows && c >= 0 && c < cols;
}

// Multi-source BFS: every start cell is pushed at distance 0 before the loop.
// Returns a distance grid; -1 means unreachable. Single-source is the same
// function with one source.
std::vector<std::vector<int>> bfsDistances(const Grid& g,
                                           const std::vector<std::pair<int, int>>& sources) {
    const int rows = static_cast<int>(g.size());
    const int cols = rows ? static_cast<int>(g[0].size()) : 0;
    std::vector<std::vector<int>> dist(rows, std::vector<int>(cols, -1));
    std::queue<std::pair<int, int>> q;
    for (const auto& [r, c] : sources) {
        if (inBounds(r, c, rows, cols) && g[r][c] != '#' && dist[r][c] == -1) {
            dist[r][c] = 0;
            q.push({r, c});
        }
    }
    while (!q.empty()) {
        auto [r, c] = q.front();  // structured binding copies the pair — fine, it is two ints
        q.pop();
        for (int k = 0; k < 4; ++k) {
            const int nr = r + dr4[k], nc = c + dc4[k];
            if (!inBounds(nr, nc, rows, cols) || g[nr][nc] == '#' || dist[nr][nc] != -1) continue;
            dist[nr][nc] = dist[r][c] + 1;  // mark on push, not on pop: no duplicates in the queue
            q.push({nr, nc});
        }
    }
    return dist;
}

// Recursive DFS / flood fill. Mutates a visited grid passed by reference.
// Depth is bounded by the number of free cells, so a 1000x1000 open grid can
// overflow an 8 MB stack — use the iterative version for big maps.
void dfsRecursive(const Grid& g, int r, int c, std::vector<char>& visited, int& count) {
    const int rows = static_cast<int>(g.size());
    const int cols = static_cast<int>(g[0].size());
    if (!inBounds(r, c, rows, cols) || g[r][c] == '#' || visited[r * cols + c]) return;
    visited[r * cols + c] = 1;
    ++count;
    for (int k = 0; k < 4; ++k) dfsRecursive(g, r + dr4[k], c + dc4[k], visited, count);
}

// Iterative DFS with an explicit stack: same reachability, no recursion limit.
int dfsIterative(const Grid& g, int sr, int sc, std::vector<char>& visited) {
    const int rows = static_cast<int>(g.size());
    const int cols = static_cast<int>(g[0].size());
    if (!inBounds(sr, sc, rows, cols) || g[sr][sc] == '#' || visited[sr * cols + sc]) return 0;
    int count = 0;
    std::stack<std::pair<int, int>> st;
    st.push({sr, sc});
    visited[sr * cols + sc] = 1;
    while (!st.empty()) {
        auto [r, c] = st.top();
        st.pop();
        ++count;
        for (int k = 0; k < 4; ++k) {
            const int nr = r + dr4[k], nc = c + dc4[k];
            if (!inBounds(nr, nc, rows, cols) || g[nr][nc] == '#' || visited[nr * cols + nc]) continue;
            visited[nr * cols + nc] = 1;
            st.push({nr, nc});
        }
    }
    return count;
}

int countNeighbours8(const Grid& g, int r, int c) {
    const int rows = static_cast<int>(g.size());
    const int cols = static_cast<int>(g[0].size());
    int n = 0;
    for (int k = 0; k < 8; ++k) {
        const int nr = r + dr8[k], nc = c + dc8[k];
        if (inBounds(nr, nc, rows, cols) && g[nr][nc] != '#') ++n;
    }
    return n;
}

int main() {
    const Grid g = {
        "....#",
        ".##.#",
        "....#",
        "#####",
        "....."};  // bottom row is walled off from the top block

    // Single-source BFS from (0,0): Manhattan-ish distances around the wall.
    auto d = bfsDistances(g, {{0, 0}});
    assert(d[0][0] == 0);
    assert(d[0][3] == 3);
    assert(d[2][3] == 5);   // must go around the ## block: (0,3)->(1,3)->(2,3)
    assert(d[2][0] == 2);
    assert(d[1][1] == -1);  // blocked
    assert(d[4][0] == -1);  // unreachable: row 3 is a full wall
    assert(d[0][4] == -1);

    // Multi-source BFS: two sources, distance is to the nearest one.
    auto m = bfsDistances(g, {{0, 0}, {2, 3}});
    assert(m[2][3] == 0);
    assert(m[1][3] == 1);
    assert(m[0][3] == 2);  // nearer to (2,3) than to (0,0)

    // Recursive and iterative DFS agree on component size.
    const int cols = static_cast<int>(g[0].size());
    std::vector<char> v1(g.size() * cols, 0), v2(g.size() * cols, 0);
    int rec = 0;
    dfsRecursive(g, 0, 0, v1, rec);
    const int it = dfsIterative(g, 0, 0, v2);
    assert(rec == 10 && it == 10);  // 4 + 2 + 4 free cells in the top block
    assert(v1 == v2);
    assert(dfsIterative(g, 1, 1, v2) == 0);  // starting on a wall

    // 8-connectivity neighbour count and bounds check.
    assert(countNeighbours8(g, 0, 0) == 2);  // (0,1) and (1,0); (1,1) is a wall
    assert(countNeighbours8(g, 2, 3) == 2);  // (1,3) and (2,2); the other six are walls
    assert(!inBounds(-1, 0, 5, 5) && !inBounds(0, 5, 5, 5) && inBounds(4, 4, 5, 5));

    // Empty grid: BFS on nothing returns nothing.
    assert(bfsDistances(Grid{}, {{0, 0}}).empty());

    std::cout << "OK 00_grid_toolkit.cpp\n";
    return 0;
}
