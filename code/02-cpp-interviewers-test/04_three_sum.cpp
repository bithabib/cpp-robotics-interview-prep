// 04_three_sum.cpp — LeetCode 15. Sort, fix one element, two pointers for the other two, skip duplicates.
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

std::vector<std::vector<int>> threeSum(std::vector<int>& nums) {
    std::vector<std::vector<int>> out;
    std::sort(nums.begin(), nums.end());
    const int n = static_cast<int>(nums.size());
    for (int i = 0; i < n - 2; ++i) {
        if (nums[i] > 0) break;                          // sorted: three positives cannot sum to 0
        if (i > 0 && nums[i] == nums[i - 1]) continue;   // same anchor value -> same triples, skip
        int lo = i + 1, hi = n - 1;
        while (lo < hi) {
            const int sum = nums[i] + nums[lo] + nums[hi];
            if (sum < 0) ++lo;
            else if (sum > 0) --hi;
            else {
                out.push_back({nums[i], nums[lo], nums[hi]});
                // Skip duplicates on both sides before moving on, otherwise the same triple repeats.
                while (lo < hi && nums[lo] == nums[lo + 1]) ++lo;
                while (lo < hi && nums[hi] == nums[hi - 1]) --hi;
                ++lo; --hi;
            }
        }
    }
    return out;
}

int main() {
    using VV = std::vector<std::vector<int>>;
    auto sorted = [](VV v) { std::sort(v.begin(), v.end()); return v; };   // answer order is unspecified

    std::vector<int> a = {-1, 0, 1, 2, -1, -4};
    assert(sorted(threeSum(a)) == sorted(VV{{-1, -1, 2}, {-1, 0, 1}}));

    std::vector<int> b = {0, 1, 1};
    assert(threeSum(b).empty());

    std::vector<int> c = {0, 0, 0, 0};                 // duplicates must yield exactly one triple
    assert((threeSum(c) == VV{{0, 0, 0}}));

    std::vector<int> d = {-2, 0, 0, 2, 2};             // duplicate skipping on lo and hi
    assert((threeSum(d) == VV{{-2, 0, 2}}));

    std::vector<int> e = {1, 2};                       // fewer than three elements
    assert(threeSum(e).empty());

    std::cout << "OK 04_three_sum.cpp\n";
    return 0;
}
