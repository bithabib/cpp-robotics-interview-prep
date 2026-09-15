// LeetCode 378. Kth Smallest Element in a Sorted Matrix
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <queue>
#include <tuple>
#include <vector>

// Approach 1: n-way merge with a min-heap. Seed the heap with the head of each
// row (rows are sorted), pop k-1 times, each pop pushes that row's next element.
// O(k log n) time, O(n) space where n = number of rows (capped at k).
int kthSmallest(std::vector<std::vector<int>>& matrix, int k) {
    int n = static_cast<int>(matrix.size());
    using Cell = std::tuple<int, int, int>;                 // (value, row, col) — tuple compares lexicographically
    std::priority_queue<Cell, std::vector<Cell>, std::greater<Cell>> minHeap;
    for (int r = 0; r < std::min(n, k); ++r) minHeap.push({matrix[r][0], r, 0});
    while (true) {
        auto [val, r, c] = minHeap.top();
        minHeap.pop();
        if (--k == 0) return val;
        if (c + 1 < n) minHeap.push({matrix[r][c + 1], r, c + 1});
    }
}

// Approach 2: binary search on the VALUE range, not on indices. For a candidate
// mid, count how many cells are <= mid in O(n) by walking from the bottom-left
// corner (a "staircase" walk). Smallest mid with count >= k is the answer.
// O(n log(max-min)) time, O(1) space. No heap at all.
int kthSmallestBinarySearch(std::vector<std::vector<int>>& matrix, int k) {
    int n = static_cast<int>(matrix.size());
    auto countLessEqual = [&](int target) {
        int count = 0;
        int r = n - 1, c = 0;
        while (r >= 0 && c < n) {
            if (matrix[r][c] <= target) { count += r + 1; ++c; }  // whole column above r is <= target
            else --r;
        }
        return count;
    };
    int lo = matrix[0][0], hi = matrix[n - 1][n - 1];
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (countLessEqual(mid) >= k) hi = mid;               // answer is <= mid
        else lo = mid + 1;
    }
    return lo;   // lo is always an actual matrix value: it's the smallest value with count >= k
}

int main() {
    {
        std::vector<std::vector<int>> m = {{1, 5, 9}, {10, 11, 13}, {12, 13, 15}};
        assert(kthSmallest(m, 8) == 13);
        assert(kthSmallestBinarySearch(m, 8) == 13);
        assert(kthSmallest(m, 1) == 1);
        assert(kthSmallestBinarySearch(m, 1) == 1);
        assert(kthSmallest(m, 9) == 15);
        assert(kthSmallestBinarySearch(m, 9) == 15);
    }
    {
        std::vector<std::vector<int>> m = {{-5}};                // 1x1
        assert(kthSmallest(m, 1) == -5);
        assert(kthSmallestBinarySearch(m, 1) == -5);
    }
    {
        std::vector<std::vector<int>> m = {{1, 2}, {1, 3}};      // duplicates across rows
        assert(kthSmallest(m, 2) == 1);
        assert(kthSmallestBinarySearch(m, 2) == 1);
        assert(kthSmallest(m, 3) == 2);
        assert(kthSmallestBinarySearch(m, 3) == 2);
    }
    std::cout << "OK 03_kth_smallest_sorted_matrix.cpp\n";
    return 0;
}
