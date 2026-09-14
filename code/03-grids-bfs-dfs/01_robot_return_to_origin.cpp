// 01_robot_return_to_origin.cpp — LeetCode 657. Robot Return to Origin (easy)
#include <cassert>
#include <iostream>
#include <string>

// Net displacement is all that matters: sum the unit vectors and check for (0,0).
bool judgeCircle(std::string moves) {
    int x = 0, y = 0;
    for (const char m : moves) {
        switch (m) {
            case 'U': ++y; break;
            case 'D': --y; break;
            case 'R': ++x; break;
            case 'L': --x; break;
            default: break;  // input guarantees only U/D/L/R
        }
    }
    return x == 0 && y == 0;
}

int main() {
    assert(judgeCircle("UD") == true);
    assert(judgeCircle("LL") == false);
    assert(judgeCircle("RRDD") == false);
    assert(judgeCircle("LDRRLRUULR") == false);
    assert(judgeCircle("") == true);      // no moves: already at origin
    assert(judgeCircle("UDLR") == true);
    std::cout << "OK 01_robot_return_to_origin.cpp\n";
    return 0;
}
