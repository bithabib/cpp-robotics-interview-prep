// LeetCode 53. Maximum Subarray (Kadane)
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

// Kadane: best sum ending at i is either "extend the previous run" or "start fresh at i".
// Track the best of those over the whole array.
int maxSubArray(std::vector<int>& nums) {
    int best = nums[0];          // NOT 0 — an all-negative array must return its largest element
    int cur = nums[0];
    for (size_t i = 1; i < nums.size(); ++i) {
        cur = std::max(nums[i], cur + nums[i]);
        best = std::max(best, cur);
    }
    return best;
}

int main() {
    std::vector<int> a = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    assert(maxSubArray(a) == 6);                    // [4,-1,2,1]

    std::vector<int> b = {1};
    assert(maxSubArray(b) == 1);                    // single element

    std::vector<int> c = {5, 4, -1, 7, 8};
    assert(maxSubArray(c) == 23);                   // whole array

    std::vector<int> d = {-3, -1, -2};
    assert(maxSubArray(d) == -1);                   // all negative: initialising best=0 would give 0

    std::cout << "OK 06_maximum_subarray.cpp\n";
    return 0;
}
