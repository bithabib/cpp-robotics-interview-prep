// Chapter 05 toolkit demo: hand-rolled binary heap, std heap algorithms,
// nth_element, TreeNode build-from-vector + traversals, std::list::splice.
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

// ---- 1. A max-heap by hand on a std::vector --------------------------------
// Layout: node i has children 2i+1 and 2i+2, parent (i-1)/2. No pointers, no
// allocation per node, cache-friendly — that is why heaps live in arrays.
struct MaxHeap {
    std::vector<int> a;

    void push(int x) {
        a.push_back(x);
        siftUp(static_cast<int>(a.size()) - 1);
    }
    int top() const { return a.front(); }
    void pop() {
        std::swap(a.front(), a.back());   // move last leaf to the root ...
        a.pop_back();
        if (!a.empty()) siftDown(0);      // ... and let it sink to its place
    }
    bool empty() const { return a.empty(); }

    void siftUp(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (a[p] >= a[i]) break;      // heap property restored
            std::swap(a[p], a[i]);
            i = p;
        }
    }
    void siftDown(int i) {
        int n = static_cast<int>(a.size());
        while (true) {
            int l = 2 * i + 1, r = 2 * i + 2, best = i;
            if (l < n && a[l] > a[best]) best = l;
            if (r < n && a[r] > a[best]) best = r;
            if (best == i) break;
            std::swap(a[i], a[best]);
            i = best;
        }
    }
};

// ---- 2. TreeNode as LeetCode defines it + level-order builder ---------------
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* left, TreeNode* right) : val(x), left(left), right(right) {}
};

// Builds from LeetCode's level-order encoding; NONE marks a missing child.
// Children are only queued for non-null nodes, exactly like LeetCode's format.
const int NONE = -1;
TreeNode* buildTree(const std::vector<int>& vals) {
    if (vals.empty() || vals[0] == NONE) return nullptr;
    TreeNode* root = new TreeNode(vals[0]);
    std::queue<TreeNode*> q;
    q.push(root);
    int i = 1;
    int n = static_cast<int>(vals.size());
    while (!q.empty() && i < n) {
        TreeNode* cur = q.front();
        q.pop();
        if (i < n && vals[i] != NONE) { cur->left = new TreeNode(vals[i]); q.push(cur->left); }
        ++i;
        if (i < n && vals[i] != NONE) { cur->right = new TreeNode(vals[i]); q.push(cur->right); }
        ++i;
    }
    return root;
}
void deleteTree(TreeNode* root) {          // post-order: children first, then self
    if (root == nullptr) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}
void inorder(TreeNode* root, std::vector<int>& out) {
    if (root == nullptr) return;
    inorder(root->left, out);
    out.push_back(root->val);
    inorder(root->right, out);
}
void preorder(TreeNode* root, std::vector<int>& out) {
    if (root == nullptr) return;
    out.push_back(root->val);
    preorder(root->left, out);
    preorder(root->right, out);
}
// Inorder without recursion: the explicit stack replaces the call stack.
std::vector<int> inorderIterative(TreeNode* root) {
    std::vector<int> out;
    std::vector<TreeNode*> stack;
    TreeNode* cur = root;
    while (cur != nullptr || !stack.empty()) {
        while (cur != nullptr) { stack.push_back(cur); cur = cur->left; }
        cur = stack.back();
        stack.pop_back();
        out.push_back(cur->val);
        cur = cur->right;
    }
    return out;
}

int main() {
    // ---- hand-rolled heap vs std::sort ----
    {
        std::vector<int> data = {5, 1, 9, 3, 7, 3, 8, 0, -2, 6};
        MaxHeap h;
        for (int x : data) h.push(x);
        std::vector<int> popped;
        while (!h.empty()) { popped.push_back(h.top()); h.pop(); }
        std::vector<int> sorted = data;
        std::sort(sorted.begin(), sorted.end(), std::greater<int>());
        assert(popped == sorted);
    }

    // ---- std::make_heap / push_heap / pop_heap on a vector ----
    {
        std::vector<int> v = {4, 10, 3, 5, 1};
        std::make_heap(v.begin(), v.end());        // O(n) build, max-heap by default
        assert(v.front() == 10);
        v.push_back(42);
        std::push_heap(v.begin(), v.end());        // sift the new back() up
        assert(v.front() == 42);
        std::pop_heap(v.begin(), v.end());         // moves max to back(), re-heaps [begin, end-1)
        assert(v.back() == 42);
        v.pop_back();
        assert(v.front() == 10);
        // min-heap: pass std::greater<> to every call
        std::make_heap(v.begin(), v.end(), std::greater<>());
        assert(v.front() == 1);
    }

    // ---- std::priority_queue: max by default, min via std::greater, custom via lambda ----
    {
        std::priority_queue<int> maxq;
        for (int x : {3, 1, 4, 1, 5}) maxq.push(x);
        assert(maxq.top() == 5);

        std::priority_queue<int, std::vector<int>, std::greater<int>> minq;
        for (int x : {3, 1, 4, 1, 5}) minq.push(x);
        assert(minq.top() == 1);

        // pairs compare lexicographically: (dist, id) — smallest dist on top
        using P = std::pair<double, int>;
        std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
        pq.push({2.5, 7}); pq.push({0.5, 3}); pq.push({1.0, 9});
        assert(pq.top().second == 3);

        // custom comparator with a lambda: "a comes AFTER b" == a has lower priority
        struct Task { int prio; std::string name; };
        auto cmp = [](const Task& a, const Task& b) { return a.prio < b.prio; };  // max-heap on prio
        std::priority_queue<Task, std::vector<Task>, decltype(cmp)> tasks(cmp);
        tasks.push({2, "plan"}); tasks.push({9, "estop"}); tasks.push({5, "localise"});
        assert(tasks.top().name == "estop");
    }

    // ---- nth_element: O(n) average selection ----
    {
        std::vector<int> v = {9, 2, 7, 4, 5, 1, 8, 3, 6};
        int k = 3;                                  // 3rd smallest (0-based index 2)
        std::nth_element(v.begin(), v.begin() + (k - 1), v.end());
        assert(v[k - 1] == 3);
        // everything left of the pivot is <= it, right is >= it, but NOT sorted
        for (int i = 0; i < k - 1; ++i) assert(v[i] <= v[k - 1]);
        for (int i = k; i < static_cast<int>(v.size()); ++i) assert(v[i] >= v[k - 1]);
    }

    // ---- TreeNode build + traversals ----
    {
        // tree:  3 -> (9, 20),  20 -> (15, 7)
        TreeNode* root = buildTree({3, 9, 20, NONE, NONE, 15, 7});
        std::vector<int> in, pre;
        inorder(root, in);
        preorder(root, pre);
        assert(in == std::vector<int>({9, 3, 15, 20, 7}));
        assert(pre == std::vector<int>({3, 9, 20, 15, 7}));
        assert(inorderIterative(root) == in);
        deleteTree(root);
        assert(buildTree({}) == nullptr);
    }

    // ---- std::list::splice + iterator stability (the LRU building block) ----
    {
        std::list<int> l = {1, 2, 3, 4};
        std::unordered_map<int, std::list<int>::iterator> where;
        for (auto it = l.begin(); it != l.end(); ++it) where[*it] = it;
        // move node "3" to the front in O(1): no allocation, no copy, iterators stay valid
        l.splice(l.begin(), l, where[3]);
        assert(l == std::list<int>({3, 1, 2, 4}));
        assert(*where[3] == 3 && where[3] == l.begin());   // the stored iterator still points at 3
        l.erase(where[2]);
        where.erase(2);
        assert(l == std::list<int>({3, 1, 4}));
    }

    std::cout << "OK 00_heap_tree_toolkit.cpp\n";
    return 0;
}
