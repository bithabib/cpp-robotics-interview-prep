// 07_top_k_frequent.cpp — LeetCode 347. Count with a hash map, keep the k largest counts in a min-heap.
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <utility>
#include <vector>

std::vector<int> topKFrequent(std::vector<int>& nums, int k) {
    std::unordered_map<int, int> freq;
    for (int x : nums) ++freq[x];

    // Min-heap on (count, value): the top is the weakest of the current k candidates.
    // When the heap exceeds k, pop the weakest. Heap size never exceeds k+1 -> O(n log k).
    using CountVal = std::pair<int, int>;
    std::priority_queue<CountVal, std::vector<CountVal>, std::greater<CountVal>> heap;
    for (const auto& [val, cnt] : freq) {
        heap.push({cnt, val});
        if (static_cast<int>(heap.size()) > k) heap.pop();
    }
    std::vector<int> out;
    out.reserve(static_cast<std::size_t>(k));
    while (!heap.empty()) {
        out.push_back(heap.top().second);
        heap.pop();
    }
    return out;   // any order is accepted by the problem
}

int main() {
    auto sorted = [](std::vector<int> v) { std::sort(v.begin(), v.end()); return v; };

    std::vector<int> a = {1, 1, 1, 2, 2, 3};
    assert(sorted(topKFrequent(a, 2)) == (std::vector<int>{1, 2}));

    std::vector<int> b = {1};
    assert(sorted(topKFrequent(b, 1)) == (std::vector<int>{1}));

    std::vector<int> c = {4, 4, 4, 5, 5, 6, 6, 7};        // k equal to the number of distinct values
    assert(sorted(topKFrequent(c, 4)) == (std::vector<int>{4, 5, 6, 7}));

    std::vector<int> d = {-1, -1, 2, 2, 2, 3};            // negatives are fine as hash keys
    assert(sorted(topKFrequent(d, 1)) == (std::vector<int>{2}));

    std::cout << "OK 07_top_k_frequent.cpp\n";
    return 0;
}
