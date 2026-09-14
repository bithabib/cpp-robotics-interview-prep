// 00_stl_demo.cpp — sort with lambdas, priority_queue, deque/queue/stack, map vs unordered_map,
// std::array and std::string_view.
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

struct Detection {
    std::string label;
    double score;
    int frame;
};

// string_view: a (pointer, length) view of characters someone else owns. No copy, no allocation.
bool startsWith(std::string_view s, std::string_view prefix) {
    return s.substr(0, prefix.size()) == prefix;
}

int main() {
    // ---------- s1: std::sort with custom comparator / lambda ----------
    std::vector<int> v = {5, 2, 9, 1};
    std::sort(v.begin(), v.end());                                   // ascending, uses operator<
    assert((v == std::vector<int>{1, 2, 5, 9}));
    std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; }); // descending; comparator = "a goes before b?"
    assert((v == std::vector<int>{9, 5, 2, 1}));

    // pairs sort lexicographically out of the box: by first, then second
    std::vector<std::pair<int, int>> pts = {{2, 1}, {1, 5}, {1, 2}};
    std::sort(pts.begin(), pts.end());
    assert((pts == std::vector<std::pair<int, int>>{{1, 2}, {1, 5}, {2, 1}}));

    // structs: multiple keys — score descending, then frame ascending. std::tie makes this one line.
    std::vector<Detection> dets = {{"car", 0.9, 3}, {"person", 0.9, 1}, {"bike", 0.5, 2}};
    std::stable_sort(dets.begin(), dets.end(), [](const Detection& a, const Detection& b) {
        return std::tie(b.score, a.frame) < std::tie(a.score, b.frame);
    });
    assert(dets[0].label == "person" && dets[1].label == "car" && dets[2].label == "bike");

    // argsort: sort indices by the values they point at — the lambda captures `cost` by reference
    std::vector<double> cost = {3.5, 0.2, 1.7};
    std::vector<int> idx(cost.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(), [&cost](int a, int b) { return cost[a] < cost[b]; });
    assert((idx == std::vector<int>{1, 2, 0}));

    // captures: [=] copies, [&] references, [k] copies just k
    int k = 2;
    auto timesK = [k](int a) { return a * k; };
    auto bumpK = [&k]() { ++k; };
    bumpK();
    assert(timesK(3) == 6 && k == 3);      // timesK captured k when it was 2

    // ---------- s2: std::priority_queue ----------
    std::priority_queue<int> maxHeap;
    for (int e : {3, 1, 4}) maxHeap.push(e);
    assert(maxHeap.top() == 4);

    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;   // min-heap via greater<>
    for (int e : {3, 1, 4}) minHeap.push(e);
    assert(minHeap.top() == 1);
    minHeap.pop();                                                            // pop() returns void
    assert(minHeap.top() == 3);

    // min-heap of (distance, node) — Dijkstra's frontier. pair compares by first, so distance wins.
    using DistNode = std::pair<double, int>;
    std::priority_queue<DistNode, std::vector<DistNode>, std::greater<DistNode>> frontier;
    frontier.push({2.5, 7}); frontier.push({0.5, 3}); frontier.push({1.0, 9});
    assert(frontier.top().second == 3);

    // custom comparator: lambda + decltype, pass the lambda to the constructor
    auto byScoreAsc = [](const Detection& a, const Detection& b) { return a.score > b.score; }; // "a is lower priority than b"
    std::priority_queue<Detection, std::vector<Detection>, decltype(byScoreAsc)> lowScoreFirst(byScoreAsc);
    for (const auto& d : dets) lowScoreFirst.push(d);
    assert(lowScoreFirst.top().label == "bike");

    // ---------- s3: std::deque, std::queue, std::stack ----------
    std::queue<int> q;            // adapter over deque: push back, pop front (BFS)
    q.push(1); q.push(2);
    assert(q.front() == 1);
    q.pop();                      // void — read front() first if you need the value
    assert(q.front() == 2 && q.size() == 1);

    std::stack<int> st;           // adapter over deque: push/pop/top at the back (DFS, parentheses)
    st.push(1); st.push(2);
    assert(st.top() == 2);
    st.pop();
    assert(st.top() == 1);

    std::deque<int> window;       // both ends O(1), plus random access — sliding-window max
    window.push_back(1); window.push_back(2); window.push_front(0);
    assert(window.front() == 0 && window.back() == 2 && window[1] == 1);
    window.pop_front(); window.pop_back();
    assert(window.size() == 1 && window[0] == 1);

    // ---------- s4: std::map vs unordered_map, iterators ----------
    std::map<int, std::string> timeline = {{30, "c"}, {10, "a"}, {20, "b"}};   // red-black tree, key order
    std::string order;
    for (const auto& [t, name] : timeline) order += name;                      // iterates 10, 20, 30
    assert(order == "abc");
    auto it = timeline.lower_bound(15);            // first key >= 15 — O(log n), only on ordered containers
    assert(it != timeline.end() && it->first == 20);
    assert(timeline.lower_bound(99) == timeline.end());
    assert(timeline.rbegin()->first == 30);        // reverse iterator: largest key

    // erase-while-iterating: erase() returns the next valid iterator; ++it after erase is a bug
    for (auto e = timeline.begin(); e != timeline.end();) {
        if (e->first == 20) e = timeline.erase(e);
        else ++e;
    }
    assert(timeline.size() == 2 && timeline.count(20) == 0);

    std::unordered_map<std::string, int> counts;   // hash table, no order, O(1) average
    for (const char* w : {"a", "b", "a"}) ++counts[w];         // operator[] inserts 0 if missing
    assert(counts["a"] == 2 && counts.size() == 2);
    assert(counts.find("z") == counts.end());      // find() does not insert; operator[] would
    assert(counts.count("z") == 0 && counts.size() == 2);

    // iterator arithmetic (it + 2, it - begin) only on random-access containers (vector, deque, array)
    std::vector<int> w = {4, 5, 6};
    auto mid = w.begin() + 1;
    assert(*mid == 5 && mid - w.begin() == 1);
    assert(std::next(timeline.begin()) == std::prev(timeline.end()));   // for map, use std::next/std::prev

    // ---------- s5: std::array, std::string_view ----------
    std::array<double, 3> xyz = {1.0, 2.0, 3.0};   // size is part of the type, lives on the stack
    assert(xyz.size() == 3 && xyz[2] == 3.0);
    std::array<std::array<int, 3>, 3> grid{};      // fixed 3x3, zero-initialised: a tiny costmap
    grid[1][1] = 9;
    assert(grid[1][1] == 9 && grid[0][0] == 0);

    std::string topic = "/sensors/lidar/points";
    std::string_view sv = topic;                   // views topic's buffer; topic must outlive sv
    assert(startsWith(sv, "/sensors"));
    assert(sv.substr(9, 5) == "lidar");            // substr on a view is O(1): no allocation
    assert(startsWith("literal", "lit"));          // literals convert too

    std::cout << "OK 00_stl_demo.cpp\n";
    return 0;
}
