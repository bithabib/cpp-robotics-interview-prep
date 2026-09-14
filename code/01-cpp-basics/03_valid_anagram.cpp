// LeetCode 242. Valid Anagram
#include <cassert>
#include <iostream>
#include <string>

// Lowercase letters only -> a fixed 26-slot count array beats a hash map.
// +1 for s, -1 for t; anagram iff every slot ends at zero.
bool isAnagram(std::string s, std::string t) {
    if (s.size() != t.size()) return false;
    int count[26] = {};                        // {} zero-initialises; plain int count[26]; does not
    for (size_t i = 0; i < s.size(); ++i) {
        ++count[s[i] - 'a'];
        --count[t[i] - 'a'];
    }
    for (int c : count) {
        if (c != 0) return false;
    }
    return true;
}

int main() {
    assert(isAnagram("anagram", "nagaram") == true);
    assert(isAnagram("rat", "car") == false);
    assert(isAnagram("", "") == true);         // empty strings
    assert(isAnagram("a", "ab") == false);     // length mismatch
    assert(isAnagram("aacc", "ccac") == false);// same letters, different multiset
    std::cout << "OK 03_valid_anagram.cpp\n";
    return 0;
}
