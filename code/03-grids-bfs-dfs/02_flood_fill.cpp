// 02_flood_fill.cpp — LeetCode 733. Flood Fill (easy)
#include <cassert>
#include <iostream>
#include <vector>

// Recursive DFS. The recolouring itself is the visited mark: once a cell is
// newColor it no longer equals the original colour and is skipped.
void fill(std::vector<std::vector<int>>& img, int r, int c, int from, int to) {
    const int rows = static_cast<int>(img.size());
    const int cols = static_cast<int>(img[0].size());
    if (r < 0 || r >= rows || c < 0 || c >= cols || img[r][c] != from) return;
    img[r][c] = to;
    fill(img, r + 1, c, from, to);
    fill(img, r - 1, c, from, to);
    fill(img, r, c + 1, from, to);
    fill(img, r, c - 1, from, to);
}

std::vector<std::vector<int>> floodFill(std::vector<std::vector<int>>& image, int sr, int sc, int color) {
    // If the start already has the target colour, filling would never terminate
    // without this guard (every neighbour "still" matches the original colour).
    if (image[sr][sc] != color) fill(image, sr, sc, image[sr][sc], color);
    return image;
}

int main() {
    {
        std::vector<std::vector<int>> img = {{1, 1, 1}, {1, 1, 0}, {1, 0, 1}};
        const std::vector<std::vector<int>> want = {{2, 2, 2}, {2, 2, 0}, {2, 0, 1}};
        assert(floodFill(img, 1, 1, 2) == want);
    }
    {
        std::vector<std::vector<int>> img = {{0, 0, 0}, {0, 0, 0}};
        const std::vector<std::vector<int>> want = img;
        assert(floodFill(img, 0, 0, 0) == want);  // same colour: unchanged, no infinite loop
    }
    {
        std::vector<std::vector<int>> img = {{5}};
        assert(floodFill(img, 0, 0, 7) == std::vector<std::vector<int>>{{7}});
    }
    {
        // Diagonal cells are not 4-connected: the two 1s stay separate.
        std::vector<std::vector<int>> img = {{1, 0}, {0, 1}};
        const std::vector<std::vector<int>> want = {{9, 0}, {0, 1}};
        assert(floodFill(img, 0, 0, 9) == want);
    }
    std::cout << "OK 02_flood_fill.cpp\n";
    return 0;
}
