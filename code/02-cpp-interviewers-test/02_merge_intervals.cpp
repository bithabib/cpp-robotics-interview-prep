// 02_merge_intervals.cpp — LeetCode 56. Sort by start, then sweep and extend the last merged interval.
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

std::vector<std::vector<int>> merge(std::vector<std::vector<int>>& intervals) {
    if (intervals.empty()) return {};
    // vector<int> already compares lexicographically, so this sorts by start (then end).
    // Being explicit with a lambda is what interviewers expect to see.
    std::sort(intervals.begin(), intervals.end(),
              [](const std::vector<int>& a, const std::vector<int>& b) { return a[0] < b[0]; });
    std::vector<std::vector<int>> out;
    out.push_back(intervals[0]);
    for (const auto& iv : intervals) {
        std::vector<int>& last = out.back();      // reference: we mutate in place
        if (iv[0] <= last[1]) last[1] = std::max(last[1], iv[1]);   // overlap (touching counts): extend
        else out.push_back(iv);                                       // gap: start a new interval
    }
    return out;
}

int main() {
    using VV = std::vector<std::vector<int>>;
    VV a = {{1, 3}, {2, 6}, {8, 10}, {15, 18}};
    assert((merge(a) == VV{{1, 6}, {8, 10}, {15, 18}}));

    VV b = {{1, 4}, {4, 5}};                 // touching endpoints merge
    assert((merge(b) == VV{{1, 5}}));

    VV c = {{1, 4}, {2, 3}};                 // fully contained: end must be max, not the later interval's end
    assert((merge(c) == VV{{1, 4}}));

    VV d = {};
    assert(merge(d).empty());

    VV e = {{5, 5}};                         // single degenerate interval
    assert((merge(e) == VV{{5, 5}}));

    std::cout << "OK 02_merge_intervals.cpp\n";
    return 0;
}
