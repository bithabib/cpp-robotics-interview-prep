// 06_rotate_image.cpp — LeetCode 48. Rotate an n x n matrix 90 degrees clockwise in place:
// transpose, then reverse each row.
#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

void rotate(std::vector<std::vector<int>>& matrix) {
    const int n = static_cast<int>(matrix.size());
    // Transpose: swap across the main diagonal. j starts at i+1 so each pair is swapped once.
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            std::swap(matrix[i][j], matrix[j][i]);
    // Reverse each row: transpose + horizontal flip == 90 degrees clockwise.
    for (auto& row : matrix) std::reverse(row.begin(), row.end());
}

int main() {
    using VV = std::vector<std::vector<int>>;
    VV a = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    rotate(a);
    assert((a == VV{{7, 4, 1}, {8, 5, 2}, {9, 6, 3}}));

    VV b = {{5, 1, 9, 11}, {2, 4, 8, 10}, {13, 3, 6, 7}, {15, 14, 12, 16}};
    rotate(b);
    assert((b == VV{{15, 13, 2, 5}, {14, 3, 4, 1}, {12, 6, 8, 9}, {16, 7, 10, 11}}));

    VV c = {{1}};                            // 1x1 is a fixed point
    rotate(c);
    assert((c == VV{{1}}));

    VV d = {{1, 2}, {3, 4}};
    rotate(d);
    assert((d == VV{{3, 1}, {4, 2}}));
    rotate(d); rotate(d); rotate(d);         // four rotations = identity
    assert((d == VV{{1, 2}, {3, 4}}));

    std::cout << "OK 06_rotate_image.cpp\n";
    return 0;
}
