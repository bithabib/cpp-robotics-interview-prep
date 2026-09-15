// LeetCode 236. Lowest Common Ancestor of a Binary Tree
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

// Post-order "return what you found":
//   - a subtree returns nullptr if it contains neither p nor q,
//   - returns p or q if it contains exactly one of them (or that node itself),
//   - the first node where BOTH sides return non-null is the LCA, and it
//     propagates unchanged up to the root.
// If p is an ancestor of q we return p as soon as we hit it, which is correct
// because the problem guarantees both nodes exist in the tree.
// O(n) time, O(h) recursion depth.
TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (root == nullptr || root == p || root == q) return root;
    TreeNode* left = lowestCommonAncestor(root->left, p, q);
    TreeNode* right = lowestCommonAncestor(root->right, p, q);
    if (left && right) return root;          // p and q are on different sides: this is the split
    return left ? left : right;              // pass up whichever side found something
}

// BST variant (LeetCode 235): use the ordering, no recursion needed. O(h).
TreeNode* lowestCommonAncestorBST(TreeNode* root, TreeNode* p, TreeNode* q) {
    while (root) {
        if (p->val < root->val && q->val < root->val) root = root->left;
        else if (p->val > root->val && q->val > root->val) root = root->right;
        else return root;                    // they split here (or one equals root)
    }
    return nullptr;
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
TreeNode* find(TreeNode* root, int val) {    // values are unique in the LeetCode tests
    if (root == nullptr || root->val == val) return root;
    TreeNode* l = find(root->left, val);
    return l ? l : find(root->right, val);
}
void deleteTree(TreeNode* root) {
    if (root == nullptr) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

int main() {
    {
        TreeNode* root = buildTree({3, 5, 1, 6, 2, 0, 8, NONE, NONE, 7, 4});
        assert(lowestCommonAncestor(root, find(root, 5), find(root, 1))->val == 3);
        assert(lowestCommonAncestor(root, find(root, 5), find(root, 4))->val == 5);   // p is ancestor of q
        assert(lowestCommonAncestor(root, find(root, 7), find(root, 4))->val == 2);
        assert(lowestCommonAncestor(root, find(root, 6), find(root, 8))->val == 3);
        deleteTree(root);
    }
    {
        TreeNode* root = buildTree({1, 2});                      // two nodes
        assert(lowestCommonAncestor(root, find(root, 1), find(root, 2))->val == 1);
        deleteTree(root);
    }
    {
        TreeNode* root = buildTree({6, 2, 8, 0, 4, 7, 9, NONE, NONE, 3, 5});   // a BST (LeetCode 235)
        assert(lowestCommonAncestorBST(root, find(root, 2), find(root, 8))->val == 6);
        assert(lowestCommonAncestorBST(root, find(root, 2), find(root, 4))->val == 2);
        assert(lowestCommonAncestor(root, find(root, 3), find(root, 5))->val == 4);  // general version agrees
        deleteTree(root);
    }
    std::cout << "OK 06_lowest_common_ancestor.cpp\n";
    return 0;
}
