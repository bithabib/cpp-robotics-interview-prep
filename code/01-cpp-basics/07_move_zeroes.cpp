// LeetCode 283. Move Zeroes
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

// Two pointers: `write` is the slot for the next non-zero. Swapping (instead of
// overwriting then zero-filling) keeps it a single pass and preserves order.
void moveZeroes(std::vector<int>& nums) {
    int write = 0;
    for (int read = 0; read < static_cast<int>(nums.size()); ++read) {
        if (nums[read] != 0) {
            std::swap(nums[write], nums[read]);   // no-op when write == read
            ++write;
        }
    }
}

int main() {
    std::vector<int> a = {0, 1, 0, 3, 12};
    moveZeroes(a);
    assert(a == std::vector<int>({1, 3, 12, 0, 0}));

    std::vector<int> b = {0};
    moveZeroes(b);
    assert(b == std::vector<int>({0}));

    std::vector<int> c = {};                         // empty
    moveZeroes(c);
    assert(c.empty());

    std::vector<int> d = {1, 2, 3};                  // no zeroes: order untouched
    moveZeroes(d);
    assert(d == std::vector<int>({1, 2, 3}));

    std::vector<int> e = {0, 0, 1};
    moveZeroes(e);
    assert(e == std::vector<int>({1, 0, 0}));

    std::cout << "OK 07_move_zeroes.cpp\n";
    return 0;
}
