// LeetCode 206. Reverse Linked List
#include <cassert>
#include <iostream>
#include <vector>

// LeetCode's definition, verbatim.
struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

// Iterative: walk the list, flipping each `next` to point backwards.
// Three pointers: prev (already reversed), cur (being flipped), next (saved before we overwrite).
ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* cur = head;
    while (cur != nullptr) {
        ListNode* next = cur->next;   // save before we lose it
        cur->next = prev;
        prev = cur;
        cur = next;
    }
    return prev;                      // prev is the new head (nullptr for an empty list)
}

// Recursive: reverse everything after head, then hook head onto the tail.
// O(n) stack depth — mention that to the interviewer.
ListNode* reverseListRecursive(ListNode* head) {
    if (head == nullptr || head->next == nullptr) return head;
    ListNode* newHead = reverseListRecursive(head->next);
    head->next->next = head;          // the node after us now points back at us
    head->next = nullptr;             // we become the tail (until a caller re-hooks us)
    return newHead;
}

// ---- test helpers ------------------------------------------------------
ListNode* buildList(const std::vector<int>& vals) {
    ListNode dummy;
    ListNode* tail = &dummy;
    for (int v : vals) {
        tail->next = new ListNode(v);
        tail = tail->next;
    }
    return dummy.next;
}
std::vector<int> toVector(ListNode* head) {
    std::vector<int> out;
    for (; head != nullptr; head = head->next) out.push_back(head->val);
    return out;
}
void deleteList(ListNode* head) {
    while (head != nullptr) {
        ListNode* next = head->next;
        delete head;
        head = next;
    }
}

int main() {
    {
        ListNode* h = buildList({1, 2, 3, 4, 5});
        h = reverseList(h);
        assert(toVector(h) == std::vector<int>({5, 4, 3, 2, 1}));
        h = reverseListRecursive(h);              // reverse back
        assert(toVector(h) == std::vector<int>({1, 2, 3, 4, 5}));
        deleteList(h);
    }
    {
        ListNode* h = buildList({1, 2});
        h = reverseListRecursive(h);
        assert(toVector(h) == std::vector<int>({2, 1}));
        deleteList(h);
    }
    {
        ListNode* h = buildList({7});             // single node
        assert(toVector(reverseList(h)) == std::vector<int>({7}));
        deleteList(h);
    }
    assert(reverseList(nullptr) == nullptr);      // empty list
    assert(reverseListRecursive(nullptr) == nullptr);

    std::cout << "OK 04_reverse_linked_list.cpp\n";
    return 0;
}
