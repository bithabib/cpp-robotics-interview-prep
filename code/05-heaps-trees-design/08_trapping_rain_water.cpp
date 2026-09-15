// LeetCode 42. Trapping Rain Water
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

// Water above bar i = min(maxLeft(i), maxRight(i)) - height[i].
// Two pointers: whichever side has the lower running max is the side whose
// water level is already decided (the other side has something at least as
// tall to hold it), so we can settle that bar and move inwards.
// O(n) time, O(1) space.
int trap(std::vector<int>& height) {
    int l = 0, r = static_cast<int>(height.size()) - 1;
    int leftMax = 0, rightMax = 0, water = 0;
    while (l < r) {
        if (height[l] < height[r]) {
            leftMax = std::max(leftMax, height[l]);
            water += leftMax - height[l];        // never negative: leftMax >= height[l]
            ++l;
        } else {
            rightMax = std::max(rightMax, height[r]);
            water += rightMax - height[r];
            --r;
        }
    }
    return water;
}

// Alternative 1: prefix/suffix max arrays. Same formula, O(n) extra space,
// the most obvious version — say it first, then improve to two pointers.
int trapPrefixMax(std::vector<int>& height) {
    int n = static_cast<int>(height.size());
    if (n == 0) return 0;
    std::vector<int> leftMax(n), rightMax(n);
    leftMax[0] = height[0];
    for (int i = 1; i < n; ++i) leftMax[i] = std::max(leftMax[i - 1], height[i]);
    rightMax[n - 1] = height[n - 1];
    for (int i = n - 2; i >= 0; --i) rightMax[i] = std::max(rightMax[i + 1], height[i]);
    int water = 0;
    for (int i = 0; i < n; ++i) water += std::min(leftMax[i], rightMax[i]) - height[i];
    return water;
}

// Alternative 2: monotonic (decreasing) stack of indices. When a taller bar
// arrives, each popped bar is the floor of a horizontal strip bounded by the
// new bar and the next bar left on the stack. Fills water "layer by layer".
int trapMonotonicStack(std::vector<int>& height) {
    std::vector<int> stack;                      // indices, heights decreasing bottom->top
    int water = 0;
    for (int i = 0; i < static_cast<int>(height.size()); ++i) {
        while (!stack.empty() && height[stack.back()] < height[i]) {
            int floor = stack.back();
            stack.pop_back();
            if (stack.empty()) break;            // no left wall
            int left = stack.back();
            int width = i - left - 1;
            int depth = std::min(height[left], height[i]) - height[floor];
            water += width * depth;
        }
        stack.push_back(i);
    }
    return water;
}

int main() {
    auto check = [](std::vector<int> h, int expected) {
        assert(trap(h) == expected);
        assert(trapPrefixMax(h) == expected);
        assert(trapMonotonicStack(h) == expected);
    };
    check({0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1}, 6);
    check({4, 2, 0, 3, 2, 5}, 9);
    check({}, 0);                                // empty
    check({5}, 0);                               // single bar
    check({1, 2, 3, 4}, 0);                      // monotonic: nothing trapped
    check({4, 3, 2, 1}, 0);
    check({3, 0, 0, 3}, 6);                      // flat basin
    std::cout << "OK 08_trapping_rain_water.cpp\n";
    return 0;
}
