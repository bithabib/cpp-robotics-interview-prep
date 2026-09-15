// LeetCode 215. Kth Largest Element in an Array
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <queue>
#include <vector>

// Approach 1: min-heap of size k. The heap holds the k largest seen so far;
// its top is the smallest of those, i.e. the answer. O(n log k) time, O(k) space.
int findKthLargest(std::vector<int>& nums, int k) {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;
    for (int x : nums) {
        minHeap.push(x);
        if (static_cast<int>(minHeap.size()) > k) minHeap.pop();   // evict the smallest
    }
    return minHeap.top();
}

// Approach 2: quickselect via std::nth_element. O(n) average, O(n^2) worst
// (libstdc++ uses introselect so in practice it never degrades). Reorders nums.
int findKthLargestSelect(std::vector<int>& nums, int k) {
    // kth largest == element at index n-k once sorted ascending
    auto target = nums.begin() + (static_cast<int>(nums.size()) - k);
    std::nth_element(nums.begin(), target, nums.end());
    return *target;
}

int main() {
    auto check = [](std::vector<int> nums, int k, int expected) {
        std::vector<int> copy = nums;              // heap version does not modify, select does
        assert(findKthLargest(nums, k) == expected);
        assert(findKthLargestSelect(copy, k) == expected);
    };
    check({3, 2, 1, 5, 6, 4}, 2, 5);
    check({3, 2, 3, 1, 2, 4, 5, 5, 6}, 4, 4);
    check({1}, 1, 1);                              // single element
    check({2, 1}, 2, 1);                           // k == n -> the minimum
    check({-1, -1, -1}, 2, -1);                    // duplicates and negatives
    std::cout << "OK 01_kth_largest.cpp\n";
    return 0;
}
