// LeetCode 20. Valid Parentheses
#include <cassert>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>

// Stack of pending openers. A closer must match the most recent opener.
bool isValid(std::string s) {
    static const std::unordered_map<char, char> closeToOpen = {
        {')', '('}, {']', '['}, {'}', '{'}};
    std::stack<char> st;
    for (char c : s) {
        auto it = closeToOpen.find(c);
        if (it == closeToOpen.end()) {
            st.push(c);                          // opener
        } else {
            if (st.empty() || st.top() != it->second) return false;
            st.pop();                            // std::stack::pop() returns void — read top() first
        }
    }
    return st.empty();                           // leftover openers mean unbalanced
}

int main() {
    assert(isValid("()") == true);
    assert(isValid("()[]{}") == true);
    assert(isValid("(]") == false);
    assert(isValid("([])") == true);
    assert(isValid("") == true);          // empty string is valid
    assert(isValid("(") == false);        // unmatched opener
    assert(isValid(")") == false);        // closer with empty stack
    assert(isValid("([)]") == false);     // interleaved
    std::cout << "OK 09_valid_parentheses.cpp\n";
    return 0;
}
