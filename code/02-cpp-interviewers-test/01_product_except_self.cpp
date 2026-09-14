// 01_product_except_self.cpp — LeetCode 238. Prefix/suffix products, no division, O(1) extra space.
#include <cassert>
#include <iostream>
#include <vector>

std::vector<int> productExceptSelf(std::vector<int>& nums) {
    const int n = static_cast<int>(nums.size());
    std::vector<int> out(n, 1);
    // Pass 1: out[i] = product of everything to the left of i.
    for (int i = 1; i < n; ++i) out[i] = out[i - 1] * nums[i - 1];
    // Pass 2: walk right-to-left carrying the running product of everything to the right.
    // The output array is reused as the prefix array, so the only extra space is `suffix`.
    int suffix = 1;
    for (int i = n - 1; i >= 0; --i) {
        out[i] *= suffix;
        suffix *= nums[i];
    }
    return out;
}

int main() {
    std::vector<int> a = {1, 2, 3, 4};
    assert((productExceptSelf(a) == std::vector<int>{24, 12, 8, 6}));

    std::vector<int> b = {-1, 1, 0, -3, 3};   // a zero: every other slot becomes 0, the zero's slot gets the rest
    assert((productExceptSelf(b) == std::vector<int>{0, 0, 9, 0, 0}));

    std::vector<int> c = {0, 0};               // two zeros: everything is 0
    assert((productExceptSelf(c) == std::vector<int>{0, 0}));

    std::vector<int> d = {5, 7};               // minimum size per the problem (n >= 2)
    assert((productExceptSelf(d) == std::vector<int>{7, 5}));

    std::cout << "OK 01_product_except_self.cpp\n";
    return 0;
}
