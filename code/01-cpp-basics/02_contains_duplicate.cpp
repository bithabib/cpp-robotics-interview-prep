// LeetCode 217. Contains Duplicate
#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_set>
#include <vector>

// Hash set: O(n) time, O(n) space. insert() returns {iterator, inserted?}.
bool containsDuplicate(std::vector<int>& nums) {
    std::unordered_set<int> seen;
    seen.reserve(nums.size() * 2);       // avoid rehashing; optional but interviewers like it
    for (int x : nums) {
        if (!seen.insert(x).second) return true;
    }
    return false;
}

// Alternative: sort and compare neighbours. O(n log n) time, O(1) extra space.
bool containsDuplicateSort(std::vector<int> nums) {   // by value: we may reorder it
    std::sort(nums.begin(), nums.end());
    return std::adjacent_find(nums.begin(), nums.end()) != nums.end();
}

int main() {
    std::vector<int> a = {1, 2, 3, 1};
    assert(containsDuplicate(a) == true);
    assert(containsDuplicateSort(a) == true);

    std::vector<int> b = {1, 2, 3, 4};
    assert(containsDuplicate(b) == false);
    assert(containsDuplicateSort(b) == false);

    std::vector<int> c = {1, 1, 1, 3, 3, 4, 3, 2, 4, 2};
    assert(containsDuplicate(c) == true);

    std::vector<int> d = {};                    // empty
    assert(containsDuplicate(d) == false);
    std::vector<int> e = {42};                  // single element
    assert(containsDuplicate(e) == false);

    std::cout << "OK 02_contains_duplicate.cpp\n";
    return 0;
}
