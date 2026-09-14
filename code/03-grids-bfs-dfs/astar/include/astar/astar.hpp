// astar.hpp — A* on a Grid. Dijkstra is A* with the zero heuristic.
#pragma once

#include <cstddef>
#include <functional>
#include <vector>

#include "astar/grid.hpp"

namespace astar {

struct Cell {
    int r = 0;
    int c = 0;
};
inline bool operator==(Cell a, Cell b) { return a.r == b.r && a.c == b.c; }
inline bool operator!=(Cell a, Cell b) { return !(a == b); }

enum class Connectivity { Four, Eight };

// Built-in heuristics. All are admissible and consistent for the connectivity
// they are meant for: Manhattan for Four, Octile or Euclidean for Eight.
// Manhattan is NOT admissible on an 8-connected grid (it overestimates diagonals).
enum class Heuristic { Zero, Manhattan, Octile, Euclidean };

using HeuristicFn = std::function<double(Cell, Cell)>;

double zeroHeuristic(Cell a, Cell b);
double manhattan(Cell a, Cell b);
double octile(Cell a, Cell b);
double euclidean(Cell a, Cell b);
HeuristicFn heuristicFor(Heuristic h);

struct Result {
    bool found = false;
    double cost = 0.0;        // +infinity when !found
    std::vector<Cell> path;   // start..goal inclusive; empty when !found
    std::size_t expanded = 0; // nodes popped from the open set and closed
};

// Orthogonal moves cost 1, diagonal moves cost sqrt(2). Diagonal moves are
// refused when either orthogonal neighbour is blocked (no corner cutting).
Result aStar(const Grid& grid, Cell start, Cell goal, Connectivity conn, const HeuristicFn& h);
Result aStar(const Grid& grid, Cell start, Cell goal, Connectivity conn, Heuristic h);
Result dijkstra(const Grid& grid, Cell start, Cell goal, Connectivity conn);

}  // namespace astar
