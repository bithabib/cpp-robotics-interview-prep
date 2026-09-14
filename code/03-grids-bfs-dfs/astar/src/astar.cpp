// astar.cpp — A* with a lazy-deletion binary heap and a closed set.
#include "astar/astar.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <queue>
#include <stdexcept>

namespace astar {

Grid Grid::fromAscii(const std::vector<std::string>& rows) {
    Grid g(rows.empty() ? 0 : static_cast<int>(rows[0].size()), static_cast<int>(rows.size()));
    for (int r = 0; r < g.height; ++r) {
        if (static_cast<int>(rows[r].size()) != g.width) throw std::invalid_argument("ragged ascii map");
        for (int c = 0; c < g.width; ++c) g.cells[g.index(r, c)] = rows[r][c] == '#' ? 1 : 0;
    }
    return g;
}

namespace {
constexpr double kSqrt2 = 1.4142135623730951;
// First four entries are orthogonal (cost 1), last four diagonal (cost sqrt2).
constexpr int kDr[8] = {-1, 0, 1, 0, -1, -1, 1, 1};
constexpr int kDc[8] = {0, 1, 0, -1, -1, 1, -1, 1};

struct Node {
    double f;
    double g;
    int idx;
};
// priority_queue is a max-heap, so "greater" puts the smallest f on top.
// Tie-break on larger g: among equal f, prefer the node closer to the goal,
// which avoids sweeping a whole plateau of equal-f cells on open maps.
struct NodeGreater {
    bool operator()(const Node& a, const Node& b) const {
        if (a.f != b.f) return a.f > b.f;
        return a.g < b.g;
    }
};
}  // namespace

double zeroHeuristic(Cell, Cell) { return 0.0; }
double manhattan(Cell a, Cell b) { return std::abs(a.r - b.r) + std::abs(a.c - b.c); }
double octile(Cell a, Cell b) {
    const int dr = std::abs(a.r - b.r), dc = std::abs(a.c - b.c);
    return (dr + dc) + (kSqrt2 - 2.0) * std::min(dr, dc);  // = max + (sqrt2-1)*min
}
double euclidean(Cell a, Cell b) { return std::hypot(a.r - b.r, a.c - b.c); }

HeuristicFn heuristicFor(Heuristic h) {
    switch (h) {
        case Heuristic::Manhattan: return manhattan;
        case Heuristic::Octile: return octile;
        case Heuristic::Euclidean: return euclidean;
        case Heuristic::Zero: break;
    }
    return zeroHeuristic;
}

Result aStar(const Grid& grid, Cell start, Cell goal, Connectivity conn, const HeuristicFn& h) {
    Result res;
    res.cost = std::numeric_limits<double>::infinity();
    if (!grid.isFree(start.r, start.c) || !grid.isFree(goal.r, goal.c)) return res;

    const int n = grid.width * grid.height;
    const double inf = std::numeric_limits<double>::infinity();
    std::vector<double> gScore(n, inf);
    std::vector<int> parent(n, -1);
    std::vector<char> closed(n, 0);
    std::priority_queue<Node, std::vector<Node>, NodeGreater> open;

    const int s = grid.index(start.r, start.c);
    const int t = grid.index(goal.r, goal.c);
    gScore[s] = 0.0;
    open.push({h(start, goal), 0.0, s});
    const int nDirs = conn == Connectivity::Four ? 4 : 8;

    while (!open.empty()) {
        const Node cur = open.top();
        open.pop();
        // Lazy deletion: a node can sit in the heap several times with different
        // g. Only the first (best) pop counts; later copies are stale and skipped.
        if (closed[cur.idx]) continue;
        closed[cur.idx] = 1;
        ++res.expanded;

        if (cur.idx == t) {
            res.found = true;
            res.cost = cur.g;
            for (int i = t; i != -1; i = parent[i]) res.path.push_back({i / grid.width, i % grid.width});
            std::reverse(res.path.begin(), res.path.end());
            return res;
        }

        const int r = cur.idx / grid.width, c = cur.idx % grid.width;
        for (int k = 0; k < nDirs; ++k) {
            const int nr = r + kDr[k], nc = c + kDc[k];
            if (!grid.isFree(nr, nc)) continue;
            // No corner cutting: a diagonal step needs both orthogonal cells free,
            // otherwise a real robot would clip the obstacle corner.
            if (k >= 4 && (!grid.isFree(r + kDr[k], c) || !grid.isFree(r, c + kDc[k]))) continue;
            const int ni = grid.index(nr, nc);
            // With a consistent heuristic a closed node already has its optimal g.
            if (closed[ni]) continue;
            const double ng = cur.g + (k < 4 ? 1.0 : kSqrt2);
            if (ng < gScore[ni]) {
                gScore[ni] = ng;
                parent[ni] = cur.idx;
                open.push({ng + h({nr, nc}, goal), ng, ni});
            }
        }
    }
    return res;  // open set exhausted: no path, cost stays +inf, path stays empty
}

Result aStar(const Grid& grid, Cell start, Cell goal, Connectivity conn, Heuristic h) {
    return aStar(grid, start, goal, conn, heuristicFor(h));
}

Result dijkstra(const Grid& grid, Cell start, Cell goal, Connectivity conn) {
    return aStar(grid, start, goal, conn, zeroHeuristic);
}

}  // namespace astar
