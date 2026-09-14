// demo.cpp — astar_demo [map.txt] [4|8]. Reads an ASCII map with 'S' and 'G',
// runs A*, prints the map with the path drawn as '*'.
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "astar/astar.hpp"

int main(int argc, char** argv) {
    const std::string file = argc > 1 ? argv[1] : "maps/sample.txt";
    const bool eight = argc > 2 && std::string(argv[2]) == "8";

    std::ifstream in(file);
    if (!in) {
        std::cerr << "cannot open " << file << "\n";
        return 1;
    }
    std::vector<std::string> rows;
    for (std::string line; std::getline(in, line);)
        if (!line.empty()) rows.push_back(line);

    astar::Cell start{-1, -1}, goal{-1, -1};
    for (int r = 0; r < static_cast<int>(rows.size()); ++r)
        for (int c = 0; c < static_cast<int>(rows[r].size()); ++c) {
            if (rows[r][c] == 'S') start = {r, c};
            if (rows[r][c] == 'G') goal = {r, c};
        }
    if (start.r < 0 || goal.r < 0) {
        std::cerr << "map needs an S and a G\n";
        return 1;
    }

    const astar::Grid grid = astar::Grid::fromAscii(rows);
    const auto conn = eight ? astar::Connectivity::Eight : astar::Connectivity::Four;
    const auto h = eight ? astar::Heuristic::Octile : astar::Heuristic::Manhattan;
    const astar::Result res = astar::aStar(grid, start, goal, conn, h);
    const astar::Result dij = astar::dijkstra(grid, start, goal, conn);

    for (const auto& cell : res.path)
        if (cell != start && cell != goal) rows[cell.r][cell.c] = '*';
    for (const auto& row : rows) std::cout << row << "\n";
    if (!res.found) {
        std::cout << "no path\n";
        return 0;
    }
    std::cout << "cost " << res.cost << ", steps " << res.path.size() - 1
              << ", A* expanded " << res.expanded << ", Dijkstra expanded " << dij.expanded << "\n";
    return 0;
}
