// LeetCode 21. Merge Two Sorted Lists
#include <cassert>
#include <iostream>
#include <vector>

struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

// Dummy-head idiom: a stack-allocated sentinel node means "append to tail"
// never has to special-case the empty result. Return dummy.next.
ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
    ListNode dummy;                   // lives on the stack, never returned itself
    ListNode* tail = &dummy;
    while (list1 != nullptr && list2 != nullptr) {
        if (list1->val <= list2->val) {   // <= keeps the merge stable
            tail->next = list1;
            list1 = list1->next;
        } else {
            tail->next = list2;
            list2 = list2->next;
        }
        tail = tail->next;
    }
    tail->next = (list1 != nullptr) ? list1 : list2;   // splice the leftover run
    return dummy.next;
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
        ListNode* m = mergeTwoLists(buildList({1, 2, 4}), buildList({1, 3, 4}));
        assert(toVector(m) == std::vector<int>({1, 1, 2, 3, 4, 4}));
        deleteList(m);
    }
    assert(mergeTwoLists(nullptr, nullptr) == nullptr);        // both empty
    {
        ListNode* m = mergeTwoLists(nullptr, buildList({0}));  // one empty
        assert(toVector(m) == std::vector<int>({0}));
        deleteList(m);
    }
    {
        ListNode* m = mergeTwoLists(buildList({5, 6, 7}), buildList({1, 2}));  // no interleaving
        assert(toVector(m) == std::vector<int>({1, 2, 5, 6, 7}));
        deleteList(m);
    }
    std::cout << "OK 05_merge_two_sorted_lists.cpp\n";
    return 0;
}
