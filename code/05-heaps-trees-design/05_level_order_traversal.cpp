// LeetCode 102. Binary Tree Level Order Traversal
#include <cassert>
#include <iostream>
#include <queue>
#include <vector>

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode* left, TreeNode* right) : val(x), left(left), right(right) {}
};

// BFS with a queue. The trick that separates levels: snapshot q.size() at the
// start of each round — that many nodes are exactly the current level, and
// everything pushed during the round belongs to the next one.
// O(n) time, O(w) space where w = widest level.
std::vector<std::vector<int>> levelOrder(TreeNode* root) {
    std::vector<std::vector<int>> out;
    if (root == nullptr) return out;
    std::queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        int levelSize = static_cast<int>(q.size());   // freeze it: q grows inside the loop
        std::vector<int> level;
        level.reserve(levelSize);
        for (int i = 0; i < levelSize; ++i) {
            TreeNode* cur = q.front();
            q.pop();
            level.push_back(cur->val);
            if (cur->left) q.push(cur->left);
            if (cur->right) q.push(cur->right);
        }
        out.push_back(std::move(level));
    }
    return out;
}

// ---- test helpers ------------------------------------------------------
const int NONE = -1;
TreeNode* buildTree(const std::vector<int>& vals) {
    if (vals.empty() || vals[0] == NONE) return nullptr;
    TreeNode* root = new TreeNode(vals[0]);
    std::queue<TreeNode*> q;
    q.push(root);
    int i = 1, n = static_cast<int>(vals.size());
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
void deleteTree(TreeNode* root) {
    if (root == nullptr) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

int main() {
    {
        TreeNode* root = buildTree({3, 9, 20, NONE, NONE, 15, 7});
        assert(levelOrder(root) == std::vector<std::vector<int>>({{3}, {9, 20}, {15, 7}}));
        deleteTree(root);
    }
    {
        TreeNode* root = buildTree({1});
        assert(levelOrder(root) == std::vector<std::vector<int>>({{1}}));
        deleteTree(root);
    }
    {
        TreeNode* root = buildTree({1, NONE, 2, NONE, 3});       // right-leaning chain
        assert(levelOrder(root) == std::vector<std::vector<int>>({{1}, {2}, {3}}));
        deleteTree(root);
    }
    assert(levelOrder(nullptr).empty());                          // empty tree
    std::cout << "OK 05_level_order_traversal.cpp\n";
    return 0;
}
