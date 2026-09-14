// 08_min_stack.cpp — LeetCode 155. A stack with O(1) getMin: a second stack tracks the running minimum.
#include <cassert>
#include <iostream>
#include <stack>

class MinStack {
public:
    MinStack() = default;

    void push(int val) {
        data_.push(val);
        // mins_ holds the minimum of everything at or below this position.
        // Push on <= (not <) so duplicates of the minimum survive a pop of one copy.
        if (mins_.empty() || val <= mins_.top()) mins_.push(val);
    }
    void pop() {
        if (data_.top() == mins_.top()) mins_.pop();   // this element was the current minimum
        data_.pop();
    }
    int top() const { return data_.top(); }
    int getMin() const { return mins_.top(); }

private:
    std::stack<int> data_;
    std::stack<int> mins_;
};

int main() {
    MinStack s;
    s.push(-2); s.push(0); s.push(-3);
    assert(s.getMin() == -3);
    s.pop();
    assert(s.top() == 0);
    assert(s.getMin() == -2);

    MinStack dup;                    // duplicate minimums: popping one must not lose the other
    dup.push(1); dup.push(1); dup.push(2);
    assert(dup.getMin() == 1);
    dup.pop(); dup.pop();
    assert(dup.getMin() == 1);
    dup.pop();

    MinStack rising;                 // increasing pushes: mins_ stays size 1
    rising.push(5); rising.push(6); rising.push(7);
    assert(rising.getMin() == 5 && rising.top() == 7);
    rising.pop(); rising.pop(); rising.pop();
    rising.push(3);
    assert(rising.getMin() == 3);

    std::cout << "OK 08_min_stack.cpp\n";
    return 0;
}
