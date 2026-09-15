// LeetCode 973. K Closest Points to Origin
#include <algorithm>
#include <cassert>
#include <iostream>
#include <queue>
#include <vector>

// Max-heap of size k keyed on squared distance: the top is the FARTHEST of the
// k best, so a new point only enters if it beats that. sqrt is monotonic, so
// comparing squared distances gives the same order without the float call.
// O(n log k) time, O(k) space.
std::vector<std::vector<int>> kClosest(std::vector<std::vector<int>>& points, int k) {
    using Entry = std::pair<long long, int>;      // (squared distance, index into points)
    std::priority_queue<Entry> maxHeap;           // default std::less -> largest distance on top
    for (int i = 0; i < static_cast<int>(points.size()); ++i) {
        long long x = points[i][0], y = points[i][1];
        maxHeap.push({x * x + y * y, i});
        if (static_cast<int>(maxHeap.size()) > k) maxHeap.pop();
    }
    std::vector<std::vector<int>> out;
    out.reserve(k);
    while (!maxHeap.empty()) {
        out.push_back(points[maxHeap.top().second]);
        maxHeap.pop();
    }
    return out;
}

// Alternative: nth_element with a distance comparator. O(n) average, reorders points.
std::vector<std::vector<int>> kClosestSelect(std::vector<std::vector<int>>& points, int k) {
    auto dist2 = [](const std::vector<int>& p) {
        return static_cast<long long>(p[0]) * p[0] + static_cast<long long>(p[1]) * p[1];
    };
    std::nth_element(points.begin(), points.begin() + k, points.end(),
                     [&](const std::vector<int>& a, const std::vector<int>& b) { return dist2(a) < dist2(b); });
    return std::vector<std::vector<int>>(points.begin(), points.begin() + k);
}

// Order of the answer is unspecified, so compare as sorted sets.
static std::vector<std::vector<int>> sorted(std::vector<std::vector<int>> v) {
    std::sort(v.begin(), v.end());
    return v;
}

int main() {
    {
        std::vector<std::vector<int>> pts = {{1, 3}, {-2, 2}};
        auto copy = pts;
        assert(sorted(kClosest(pts, 1)) == sorted({{-2, 2}}));
        assert(sorted(kClosestSelect(copy, 1)) == sorted({{-2, 2}}));
    }
    {
        std::vector<std::vector<int>> pts = {{3, 3}, {5, -1}, {-2, 4}};
        auto copy = pts;
        assert(sorted(kClosest(pts, 2)) == sorted({{3, 3}, {-2, 4}}));
        assert(sorted(kClosestSelect(copy, 2)) == sorted({{3, 3}, {-2, 4}}));
    }
    {
        std::vector<std::vector<int>> pts = {{0, 1}, {1, 0}};      // k == n, returns everything
        assert(sorted(kClosest(pts, 2)) == sorted({{0, 1}, {1, 0}}));
    }
    {
        std::vector<std::vector<int>> pts = {{10000, 10000}};     // squares need 64-bit headroom
        assert(kClosest(pts, 1).size() == 1);
    }
    std::cout << "OK 02_k_closest_points.cpp\n";
    return 0;
}
