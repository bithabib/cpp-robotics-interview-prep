// LeetCode 239. Sliding Window Maximum
#include <cassert>
#include <deque>
#include <iostream>
#include <vector>

// Monotonic deque of INDICES, values decreasing from front to back.
// Front is always the max of the current window. Each index is pushed and
// popped at most once -> O(n) total, O(k) space.
std::vector<int> maxSlidingWindow(std::vector<int>& nums, int k) {
    std::deque<int> dq;
    std::vector<int> out;
    int n = static_cast<int>(nums.size());
    for (int i = 0; i < n; ++i) {
        // 1. drop indices that fell out of the window [i-k+1, i]
        if (!dq.empty() && dq.front() <= i - k) dq.pop_front();
        // 2. a new, larger value makes every smaller one behind it useless:
        //    they are older AND smaller, so they can never be a window max again
        while (!dq.empty() && nums[dq.back()] <= nums[i]) dq.pop_back();
        dq.push_back(i);
        // 3. once the first full window is in, record its max
        if (i >= k - 1) out.push_back(nums[dq.front()]);
    }
    return out;
}

int main() {
    {
        std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
        assert(maxSlidingWindow(nums, 3) == std::vector<int>({3, 3, 5, 5, 6, 7}));
    }
    {
        std::vector<int> nums = {1};
        assert(maxSlidingWindow(nums, 1) == std::vector<int>({1}));
    }
    {
        std::vector<int> nums = {9, 8, 7, 6};                    // strictly decreasing: front expires each step
        assert(maxSlidingWindow(nums, 2) == std::vector<int>({9, 8, 7}));
    }
    {
        std::vector<int> nums = {1, 2, 3, 4};                    // strictly increasing: deque holds one index
        assert(maxSlidingWindow(nums, 2) == std::vector<int>({2, 3, 4}));
    }
    {
        std::vector<int> nums = {4, 4, 4};                       // ties: <= pops the older duplicate, still correct
        assert(maxSlidingWindow(nums, 3) == std::vector<int>({4}));
    }
    std::cout << "OK 04_sliding_window_maximum.cpp\n";
    return 0;
}
