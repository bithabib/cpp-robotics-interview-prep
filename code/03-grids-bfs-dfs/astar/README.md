# A* on a grid, from scratch, with tests

A small C++17 library, a demo, and a GoogleTest suite. Orthogonal moves cost 1,
diagonal moves cost sqrt(2), diagonals never cut obstacle corners, and Dijkstra is
A* with the zero heuristic.

```
include/astar/grid.hpp    Grid: width, height, flat vector<uint8_t> cells, inBounds, isFree, fromAscii
include/astar/astar.hpp   Cell, Connectivity, Heuristic, Result, aStar(), dijkstra()
src/astar.cpp             the planner (about 80 lines)
src/demo.cpp              astar_demo [map.txt] [4|8] — prints the map with the path as '*'
tests/astar_test.cpp      13 GoogleTest cases
maps/sample.txt           ASCII map: '.' free, '#' blocked, 'S' start, 'G' goal
```

## Build and test

```
cmake -S . -B build && cmake --build build && ctest --test-dir build
./build/astar_demo maps/sample.txt 4
./build/astar_demo maps/sample.txt 8
```

Needs `libgtest-dev` (Ubuntu 22.04 ships a `GTestConfig.cmake`, so `find_package(GTest)`
just works; the CMakeLists falls back to `add_subdirectory(/usr/src/googletest)` if it does not).

## Design notes

**Open set = `std::priority_queue` with lazy deletion.** The STL heap has no
decrease-key. Instead, every time a node's g improves we push a fresh
`{f, g, idx}` entry and leave the stale one in the heap. When an entry is popped
we check `closed[idx]`; if the node was already expanded the entry is stale and is
skipped. Cost: the heap can hold O(E) entries instead of O(V), which on a grid is
at most 8x. Benefit: no hand-written indexed heap, and the code fits in an
interview.

**Closed set = `std::vector<char>`.** A node is closed the first time it is popped.
With a *consistent* heuristic (h(a) <= cost(a,b) + h(b)) the first pop is
guaranteed optimal, so closed nodes are never reopened and neighbours that are
closed are skipped. Manhattan on 4-connected grids and octile / Euclidean on
8-connected grids are all consistent. If you hand in an inconsistent heuristic
through `HeuristicFn`, the planner still terminates but may return a suboptimal
path; that is the documented trade-off (weighted A* does exactly this on purpose).

**Heuristics.** `Manhattan = |dr| + |dc|` is exact on an open 4-connected grid and
overestimates on an 8-connected one (a diagonal costs sqrt2 < 2), so it is not
admissible there. `Octile = max(dr,dc) + (sqrt2 - 1) * min(dr,dc)` is exact on an open
8-connected grid. `Euclidean` is admissible for both but looser than octile, so it
expands more nodes. `Zero` turns A* into Dijkstra.

**Tie-breaking.** Among equal f, pop the larger g first. On an open map every cell
between start and goal has f == C*, and without this rule A* sweeps the whole
rectangle before reaching the goal. With it, the search runs straight along the
path. `HeuristicReducesExpansions` in the tests depends on this.

**No corner cutting.** A diagonal step from (r,c) to (r+dr,c+dc) is only allowed if
(r+dr,c) and (r,c+dc) are both free. A real robot with a footprint would collide
with the obstacle corner otherwise. On `maps/sample.txt` this makes the
8-connected answer identical to the 4-connected one.

**No path.** `Result{found=false, cost=+inf, path={}, expanded=n}`. Callers test
`found`, never the cost. A blocked start or goal returns immediately with
`expanded == 0`.

**Complexity.** O(E log V) time with V = cells, E <= 8V; O(V) memory for g, parent
and closed, plus O(E) worst case for the heap.

## What to say in the interview

1. "A grid is a graph: cells are nodes, the 4 or 8 neighbours are edges with cost 1
   or sqrt2. I'll index cells as `r*width+c` so all bookkeeping is flat vectors."
2. "A* expands nodes in order of f = g + h. g is the exact cost so far, h is a lower
   bound on the cost to go. With h = 0 this is Dijkstra."
3. "I'll use `std::priority_queue` with lazy deletion because the STL heap has no
   decrease-key: push duplicates, skip stale pops via the closed set."
4. "The heuristic must be admissible for optimality and consistent for the closed
   set to be safe. For 4-connected that's Manhattan, for 8-connected octile."
5. "I break ties towards larger g, otherwise open maps degrade to a full sweep."
6. "Diagonals only when both orthogonal neighbours are free, otherwise the robot
   clips the corner."
7. "Path comes back from a parent array walked from goal to start, then reversed."
8. "No path means the open set emptied; I return found=false and infinite cost, not
   an exception or a magic -1."
9. Complexity: O(E log V); on a 1000x1000 costmap that is a few million heap ops,
   fine for global planning at 1 Hz, which is why nav2's NavFn/Smac planners are
   grid A*/Dijkstra variants.
