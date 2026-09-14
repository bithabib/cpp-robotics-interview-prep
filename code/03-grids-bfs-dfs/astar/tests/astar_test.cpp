// astar_test.cpp — GoogleTest suite for the grid A*.
#include <cmath>
#include <cstdlib>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "astar/astar.hpp"

using astar::Cell;
using astar::Connectivity;
using astar::Grid;
using astar::Heuristic;
using astar::Result;

namespace {
constexpr double kSqrt2 = 1.4142135623730951;

Grid openGrid(int w, int h) { return Grid(w, h); }

// Every consecutive pair of cells is one king move apart, every cell is free,
// and the summed step costs equal the reported cost.
void expectContiguous(const Grid& g, const Result& res, Cell start, Cell goal) {
    ASSERT_TRUE(res.found);
    ASSERT_FALSE(res.path.empty());
    EXPECT_EQ(res.path.front(), start);
    EXPECT_EQ(res.path.back(), goal);
    double sum = 0.0;
    for (std::size_t i = 0; i < res.path.size(); ++i) {
        EXPECT_TRUE(g.isFree(res.path[i].r, res.path[i].c));
        if (i == 0) continue;
        const int dr = std::abs(res.path[i].r - res.path[i - 1].r);
        const int dc = std::abs(res.path[i].c - res.path[i - 1].c);
        EXPECT_LE(dr, 1);
        EXPECT_LE(dc, 1);
        EXPECT_FALSE(dr == 0 && dc == 0);
        sum += (dr + dc == 2) ? kSqrt2 : 1.0;
    }
    EXPECT_NEAR(sum, res.cost, 1e-9);
}
}  // namespace

TEST(Grid, FromAsciiAndBounds) {
    const Grid g = Grid::fromAscii({"..#", "S.G"});
    EXPECT_EQ(g.width, 3);
    EXPECT_EQ(g.height, 2);
    EXPECT_TRUE(g.isFree(0, 0));
    EXPECT_FALSE(g.isFree(0, 2));
    EXPECT_TRUE(g.isFree(1, 0));  // 'S' and 'G' are free cells
    EXPECT_FALSE(g.inBounds(-1, 0));
    EXPECT_FALSE(g.inBounds(0, 3));
    EXPECT_FALSE(g.isFree(2, 0));  // out of bounds counts as not free
    EXPECT_THROW(Grid::fromAscii({"...", ".."}), std::invalid_argument);
}

TEST(AStar, StraightLineFourConnected) {
    const Grid g = openGrid(5, 1);
    const Result res = astar::aStar(g, {0, 0}, {0, 4}, Connectivity::Four, Heuristic::Manhattan);
    ASSERT_TRUE(res.found);
    EXPECT_DOUBLE_EQ(res.cost, 4.0);
    EXPECT_EQ(res.path.size(), 5u);
    expectContiguous(g, res, {0, 0}, {0, 4});
}

TEST(AStar, AroundObstacle) {
    // Wall down column 2 with a gap at the bottom row.
    const Grid g = Grid::fromAscii({
        "..#..",
        "..#..",
        "..#..",
        "....."});
    const Result res = astar::aStar(g, {0, 0}, {0, 4}, Connectivity::Four, Heuristic::Manhattan);
    ASSERT_TRUE(res.found);
    EXPECT_DOUBLE_EQ(res.cost, 10.0);  // down 3, right 4, up 3
    expectContiguous(g, res, {0, 0}, {0, 4});
    for (const auto& cell : res.path) EXPECT_NE(cell.c == 2 && cell.r < 3, true);
}

TEST(AStar, NoPath) {
    const Grid g = Grid::fromAscii({
        ".#.",
        ".#.",
        ".#."});
    const Result res = astar::aStar(g, {0, 0}, {0, 2}, Connectivity::Eight, Heuristic::Octile);
    EXPECT_FALSE(res.found);
    EXPECT_TRUE(res.path.empty());
    EXPECT_TRUE(std::isinf(res.cost));
    EXPECT_EQ(res.expanded, 3u);  // the whole left column was explored before giving up
}

TEST(AStar, BlockedStartOrGoal) {
    const Grid g = Grid::fromAscii({"#.", ".."});
    EXPECT_FALSE(astar::aStar(g, {0, 0}, {1, 1}, Connectivity::Four, Heuristic::Manhattan).found);
    EXPECT_FALSE(astar::aStar(g, {1, 1}, {0, 0}, Connectivity::Four, Heuristic::Manhattan).found);
    EXPECT_FALSE(astar::aStar(g, {1, 1}, {5, 5}, Connectivity::Four, Heuristic::Manhattan).found);
}

TEST(AStar, StartEqualsGoal) {
    const Grid g = openGrid(3, 3);
    const Result res = astar::aStar(g, {1, 1}, {1, 1}, Connectivity::Four, Heuristic::Manhattan);
    ASSERT_TRUE(res.found);
    EXPECT_DOUBLE_EQ(res.cost, 0.0);
    ASSERT_EQ(res.path.size(), 1u);
    EXPECT_EQ(res.path[0], (Cell{1, 1}));
    EXPECT_EQ(res.expanded, 1u);
}

TEST(AStar, EightConnectedDiagonalCostsSqrt2) {
    const Grid g = openGrid(3, 3);
    const Result res = astar::aStar(g, {0, 0}, {2, 2}, Connectivity::Eight, Heuristic::Octile);
    ASSERT_TRUE(res.found);
    EXPECT_NEAR(res.cost, 2.0 * kSqrt2, 1e-12);
    EXPECT_EQ(res.path.size(), 3u);
    // Four-connected on the same grid must walk the long way.
    const Result four = astar::aStar(g, {0, 0}, {2, 2}, Connectivity::Four, Heuristic::Manhattan);
    EXPECT_DOUBLE_EQ(four.cost, 4.0);
}

TEST(AStar, NoCornerCutting) {
    // One orthogonal neighbour blocked: the diagonal is refused, go around (cost 2, not sqrt2).
    const Grid oneBlocked = Grid::fromAscii({".#", ".."});
    const Result r1 = astar::aStar(oneBlocked, {0, 0}, {1, 1}, Connectivity::Eight, Heuristic::Octile);
    ASSERT_TRUE(r1.found);
    EXPECT_DOUBLE_EQ(r1.cost, 2.0);
    EXPECT_EQ(r1.path.size(), 3u);
    // Both orthogonal neighbours blocked: squeezing between them is impossible.
    const Grid pinched = Grid::fromAscii({".#", "#."});
    const Result r2 = astar::aStar(pinched, {0, 0}, {1, 1}, Connectivity::Eight, Heuristic::Octile);
    EXPECT_FALSE(r2.found);
}

TEST(AStar, PathIsContiguousOnMaze) {
    const Grid g = Grid::fromAscii({
        "S..#......",
        ".#.#.####.",
        ".#...#....",
        ".####.#.#.",
        "......#..G"});
    for (Connectivity conn : {Connectivity::Four, Connectivity::Eight}) {
        const Heuristic h = conn == Connectivity::Four ? Heuristic::Manhattan : Heuristic::Octile;
        const Result res = astar::aStar(g, {0, 0}, {4, 9}, conn, h);
        expectContiguous(g, res, {0, 0}, {4, 9});
    }
}

TEST(AStar, MatchesDijkstraOnRandomMaps) {
    std::mt19937 rng(42);  // fixed seed: the test is deterministic
    std::bernoulli_distribution blocked(0.3);
    std::uniform_int_distribution<int> coord(0, 19);
    int comparedFound = 0;
    for (int trial = 0; trial < 60; ++trial) {
        Grid g(20, 20);
        for (auto& cell : g.cells) cell = blocked(rng) ? 1 : 0;
        const Cell s{coord(rng), coord(rng)}, t{coord(rng), coord(rng)};
        const Connectivity conn = trial % 2 ? Connectivity::Eight : Connectivity::Four;
        for (Heuristic h : {conn == Connectivity::Four ? Heuristic::Manhattan : Heuristic::Octile,
                            Heuristic::Euclidean}) {
            const Result a = astar::aStar(g, s, t, conn, h);
            const Result d = astar::dijkstra(g, s, t, conn);
            EXPECT_EQ(a.found, d.found);
            if (!a.found) continue;
            ++comparedFound;
            EXPECT_NEAR(a.cost, d.cost, 1e-9);
            expectContiguous(g, a, s, t);
            expectContiguous(g, d, s, t);
        }
    }
    EXPECT_GT(comparedFound, 20);  // sanity: the random maps were not all disconnected
}

TEST(AStar, DijkstraIsAStarWithZeroHeuristic) {
    const Grid g = openGrid(15, 15);
    const Result d = astar::dijkstra(g, {7, 7}, {14, 0}, Connectivity::Four);
    const Result z = astar::aStar(g, {7, 7}, {14, 0}, Connectivity::Four, Heuristic::Zero);
    EXPECT_DOUBLE_EQ(d.cost, z.cost);
    EXPECT_EQ(d.expanded, z.expanded);
}

TEST(AStar, HeuristicReducesExpansions) {
    // Start in the middle so Dijkstra has to grow a full diamond before it
    // touches the corner goal; A* with a good heuristic heads straight there.
    const Grid g = openGrid(41, 41);
    const Cell s{20, 20}, t{40, 40};
    const Result d4 = astar::dijkstra(g, s, t, Connectivity::Four);
    const Result a4 = astar::aStar(g, s, t, Connectivity::Four, Heuristic::Manhattan);
    EXPECT_DOUBLE_EQ(a4.cost, d4.cost);
    EXPECT_LT(a4.expanded * 4, d4.expanded);

    const Result d8 = astar::dijkstra(g, s, t, Connectivity::Eight);
    const Result a8 = astar::aStar(g, s, t, Connectivity::Eight, Heuristic::Octile);
    const Result e8 = astar::aStar(g, s, t, Connectivity::Eight, Heuristic::Euclidean);
    EXPECT_NEAR(a8.cost, d8.cost, 1e-9);
    EXPECT_NEAR(e8.cost, d8.cost, 1e-9);
    EXPECT_LT(a8.expanded * 4, d8.expanded);
    EXPECT_LE(a8.expanded, e8.expanded);  // octile is the tighter bound, so it expands no more
}

TEST(AStar, CustomHeuristicFunction) {
    const Grid g = openGrid(6, 6);
    // Weighted A* (w = 2): still finds a path, cost may be suboptimal in general,
    // but on an open grid with Manhattan it stays optimal.
    const astar::HeuristicFn weighted = [](Cell a, Cell b) { return 2.0 * astar::manhattan(a, b); };
    const Result w = astar::aStar(g, {0, 0}, {5, 5}, Connectivity::Four, weighted);
    ASSERT_TRUE(w.found);
    EXPECT_DOUBLE_EQ(w.cost, 10.0);
}
