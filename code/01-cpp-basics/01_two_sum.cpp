// LeetCode 1. Two Sum
#include <cassert>
#include <iostream>
#include <unordered_map>
#include <vector>

// One pass: for each x, ask "have I already seen target - x?". If not, remember x.
std::vector<int> twoSum(std::vector<int>& nums, int target) {
    std::unordered_map<int, int> seen;   // value -> index
    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        int need = target - nums[i];
        auto it = seen.find(need);       // find, not [] — [] would insert need with index 0
        if (it != seen.end()) return {it->second, i};
        seen[nums[i]] = i;               // insert AFTER checking so we never pair i with itself
    }
    return {};                           // LeetCode guarantees a solution; keep the compiler happy
}

int main() {
    std::vector<int> a = {2, 7, 11, 15};
    assert(twoSum(a, 9) == std::vector<int>({0, 1}));

    std::vector<int> b = {3, 2, 4};
    assert(twoSum(b, 6) == std::vector<int>({1, 2}));

    std::vector<int> c = {3, 3};                 // duplicate values: must not reuse index
    assert(twoSum(c, 6) == std::vector<int>({0, 1}));

    std::vector<int> d = {-1, -2, -3, -4, -5};   // negatives
    assert(twoSum(d, -8) == std::vector<int>({2, 4}));

    std::vector<int> e = {1};                    // no answer -> empty
    assert(twoSum(e, 2).empty());

    std::cout << "OK 01_two_sum.cpp\n";
    return 0;
}
