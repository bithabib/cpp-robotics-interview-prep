// LeetCode 704. Binary Search
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

// Classic closed interval [lo, hi]. Loop while lo <= hi.
int search(std::vector<int>& nums, int target) {
    int lo = 0;
    int hi = static_cast<int>(nums.size()) - 1;   // cast first: size() - 1 on empty wraps
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;             // (lo + hi) / 2 can overflow int
        if (nums[mid] == target) return mid;
        if (nums[mid] < target) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

// The lower_bound idiom: first index whose value is >= target, in [0, n].
// Half-open [lo, hi); loop while lo < hi. This is what you actually reuse in
// "first true" / "insert position" / "count of elements < x" questions.
int lowerBound(const std::vector<int>& nums, int target) {
    int lo = 0;
    int hi = static_cast<int>(nums.size());
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (nums[mid] < target) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

// upper_bound: first index whose value is > target. Only the comparison changes.
int upperBound(const std::vector<int>& nums, int target) {
    int lo = 0;
    int hi = static_cast<int>(nums.size());
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (nums[mid] <= target) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

int main() {
    std::vector<int> a = {-1, 0, 3, 5, 9, 12};
    assert(search(a, 9) == 4);
    assert(search(a, 2) == -1);
    assert(search(a, -1) == 0);            // first element
    assert(search(a, 12) == 5);            // last element

    std::vector<int> empty;
    assert(search(empty, 1) == -1);        // empty: hi = -1, loop never runs
    std::vector<int> one = {5};
    assert(search(one, 5) == 0);
    assert(search(one, 4) == -1);

    // lower/upper bound vs the STL versions
    std::vector<int> b = {1, 2, 2, 2, 5, 7};
    assert(lowerBound(b, 2) == 1);
    assert(upperBound(b, 2) == 4);
    assert(upperBound(b, 2) - lowerBound(b, 2) == 3);   // count of 2s
    assert(lowerBound(b, 3) == 4);                       // insert position for 3
    assert(lowerBound(b, 100) == 6);                     // == size when nothing is >= target
    assert(lowerBound(b, 0) == 0);
    assert(std::lower_bound(b.begin(), b.end(), 2) - b.begin() == 1);
    assert(std::upper_bound(b.begin(), b.end(), 2) - b.begin() == 4);
    assert(std::binary_search(b.begin(), b.end(), 5) == true);

    std::cout << "OK 10_binary_search.cpp\n";
    return 0;
}
