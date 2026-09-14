// 05_container_water.cpp — LeetCode 11. Two pointers from the outside in; always move the shorter wall.
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

int maxArea(std::vector<int>& height) {
    int lo = 0, hi = static_cast<int>(height.size()) - 1;
    int best = 0;
    while (lo < hi) {
        best = std::max(best, std::min(height[lo], height[hi]) * (hi - lo));
        // The shorter wall bounds the area. Moving the taller one can only shrink width without
        // raising the limit, so the shorter side is the only move that might improve.
        if (height[lo] < height[hi]) ++lo;
        else --hi;
    }
    return best;
}

int main() {
    std::vector<int> a = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    assert(maxArea(a) == 49);

    std::vector<int> b = {1, 1};
    assert(maxArea(b) == 1);

    std::vector<int> c = {4, 3, 2, 1, 4};   // outer walls are the answer
    assert(maxArea(c) == 16);

    std::vector<int> d = {1, 2, 1};
    assert(maxArea(d) == 2);

    std::vector<int> e = {7};                // single wall: no container
    assert(maxArea(e) == 0);

    std::cout << "OK 05_container_water.cpp\n";
    return 0;
}
