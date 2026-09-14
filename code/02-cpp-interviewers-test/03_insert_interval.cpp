// 03_insert_interval.cpp — LeetCode 57. Input is sorted and non-overlapping; one linear pass in three phases.
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

std::vector<std::vector<int>> insert(std::vector<std::vector<int>>& intervals, std::vector<int>& newInterval) {
    std::vector<std::vector<int>> out;
    out.reserve(intervals.size() + 1);
    const int n = static_cast<int>(intervals.size());
    int i = 0;
    // Phase 1: everything that ends before the new interval starts is untouched.
    while (i < n && intervals[i][1] < newInterval[0]) out.push_back(intervals[i++]);
    // Phase 2: everything that overlaps gets absorbed into newInterval.
    while (i < n && intervals[i][0] <= newInterval[1]) {
        newInterval[0] = std::min(newInterval[0], intervals[i][0]);
        newInterval[1] = std::max(newInterval[1], intervals[i][1]);
        ++i;
    }
    out.push_back(newInterval);
    // Phase 3: the rest starts after the merged interval ends.
    while (i < n) out.push_back(intervals[i++]);
    return out;
}

int main() {
    using VV = std::vector<std::vector<int>>;
    VV a = {{1, 3}, {6, 9}};
    std::vector<int> na = {2, 5};
    assert((insert(a, na) == VV{{1, 5}, {6, 9}}));

    VV b = {{1, 2}, {3, 5}, {6, 7}, {8, 10}, {12, 16}};
    std::vector<int> nb = {4, 8};
    assert((insert(b, nb) == VV{{1, 2}, {3, 10}, {12, 16}}));

    VV c = {};                                // empty list: result is just the new interval
    std::vector<int> nc = {5, 7};
    assert((insert(c, nc) == VV{{5, 7}}));

    VV d = {{1, 5}};                          // insert before everything
    std::vector<int> nd = {-3, 0};
    assert((insert(d, nd) == VV{{-3, 0}, {1, 5}}));

    VV e = {{1, 5}};                          // insert after everything
    std::vector<int> ne = {6, 8};
    assert((insert(e, ne) == VV{{1, 5}, {6, 8}}));

    std::cout << "OK 03_insert_interval.cpp\n";
    return 0;
}
