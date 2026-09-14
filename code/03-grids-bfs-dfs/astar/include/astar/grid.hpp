// grid.hpp — a binary occupancy grid: 0 free, 1 blocked. Row-major, index r*width+c.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace astar {

struct Grid {
    int width = 0;
    int height = 0;
    std::vector<std::uint8_t> cells;  // flat, row-major; uint8_t not bool so it is a real byte array

    Grid() = default;
    Grid(int w, int h) : width(w), height(h), cells(static_cast<std::size_t>(w) * h, 0) {}

    int index(int r, int c) const { return r * width + c; }
    bool inBounds(int r, int c) const { return r >= 0 && r < height && c >= 0 && c < width; }
    bool isFree(int r, int c) const { return inBounds(r, c) && cells[index(r, c)] == 0; }
    void setBlocked(int r, int c, bool blocked = true) { cells[index(r, c)] = blocked ? 1 : 0; }

    // '#' is blocked, every other character ('.', 'S', 'G', ' ') is free.
    // Throws std::invalid_argument if rows have different lengths.
    static Grid fromAscii(const std::vector<std::string>& rows);
};

}  // namespace astar
