// 00_basics_demo.cpp — the C++ traps that bite Python developers first.
// Every block asserts what actually happens, so you can read the numbers
// rather than take my word for it.
#include <cassert>
#include <climits>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// ---- pass by value vs by reference --------------------------------------
void byValue(std::vector<int> v) { v.push_back(99); }      // copies the vector
void byRef(std::vector<int>& v) { v.push_back(99); }       // aliases the caller's
int sumConstRef(const std::vector<int>& v) {               // no copy, can't modify
    int s = 0;
    for (int x : v) s += x;
    return s;
}

// Returning a vector by value is fine: the compiler moves/elides the copy.
std::vector<int> makeSquares(int n) {
    std::vector<int> out;
    out.reserve(n);
    for (int i = 0; i < n; ++i) out.push_back(i * i);
    return out;
}

int main() {
    // ---- integer division and modulo -------------------------------------
    assert(7 / 2 == 3);            // Python: 7 / 2 == 3.5, 7 // 2 == 3
    assert(-7 / 2 == -3);          // truncates toward zero; Python: -7 // 2 == -4
    assert(-7 % 2 == -1);          // sign follows the dividend; Python: -7 % 2 == 1
    assert(7.0 / 2 == 3.5);        // one double operand makes it floating division
    assert(static_cast<double>(7) / 2 == 3.5);

    // ---- int overflow (int is 32-bit here; Python ints are unbounded) ----
    int big = INT_MAX;                              // 2147483647
    long long safe = static_cast<long long>(big) + 1; // widen BEFORE adding
    assert(safe == 2147483648LL);
    // `big + 1` as int would be signed overflow = undefined behaviour.
    // -fsanitize=undefined reports it at runtime; -O2 may "optimise" it away.
    int a = 100000, b = 100000;
    long long product = static_cast<long long>(a) * b;   // 1e10 does not fit int
    assert(product == 10000000000LL);

    // ---- size_t subtraction ----------------------------------------------
    std::vector<int> empty;
    // empty.size() - 1 is size_t and wraps to 18446744073709551615, not -1.
    assert(empty.size() - 1 > 1000000);
    // The idiom: cast to int first, then subtract.
    int lastIdx = static_cast<int>(empty.size()) - 1;
    assert(lastIdx == -1);
    // Loop that would run "forever" on an empty vector:  for (size_t i = 0; i <= v.size() - 1; ++i)
    // Loop that is safe:                                 for (int i = 0; i < (int)v.size(); ++i)

    // ---- char arithmetic --------------------------------------------------
    char c = '7';
    assert(c - '0' == 7);              // char is a small integer; '7' - '0' == 7
    assert('c' - 'a' == 2);            // index into a 26-bucket array
    assert(static_cast<char>('a' + 2) == 'c');
    int count[26] = {};                // zero-initialised
    for (char ch : std::string("banana")) ++count[ch - 'a'];
    assert(count['a' - 'a'] == 3 && count['n' - 'a'] == 2);

    // ---- implicit conversions ---------------------------------------------
    double d = 3.99;
    int truncated = static_cast<int>(d);   // 3, not 4 — truncation, not rounding
    assert(truncated == 3);
    int fromBool = true + true;            // bool promotes to int
    assert(fromBool == 2);
    bool fromInt = 42;                     // any non-zero -> true
    assert(fromInt);

    // ---- pass by value vs by reference ------------------------------------
    std::vector<int> v = {1, 2, 3};
    byValue(v);
    assert(v.size() == 3);                 // caller unchanged (Python would see 4)
    byRef(v);
    assert(v.size() == 4);                 // caller sees the push
    assert(sumConstRef(v) == 105);
    std::vector<int> sq = makeSquares(4);
    assert(sq == std::vector<int>({0, 1, 4, 9}));

    // ---- auto copy vs auto& -----------------------------------------------
    std::vector<std::vector<int>> grid(2, std::vector<int>(3, 0));
    for (auto row : grid) row[0] = 1;      // `row` is a COPY; grid untouched
    assert(grid[0][0] == 0 && grid[1][0] == 0);
    for (auto& row : grid) row[0] = 1;     // reference; grid modified
    assert(grid[0][0] == 1 && grid[1][0] == 1);
    for (const auto& row : grid) assert(row.size() == 3);   // read-only, no copy

    // ---- operator[] does not bounds-check --------------------------------
    // v[100] compiles and is undefined behaviour. v.at(100) throws
    // std::out_of_range. Use .at() while debugging, [] once correct.
    bool threw = false;
    try { v.at(100); } catch (const std::out_of_range&) { threw = true; }
    assert(threw);

    std::cout << "OK 00_basics_demo.cpp\n";
    return 0;
}
