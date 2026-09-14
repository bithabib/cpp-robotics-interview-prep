// 09_word_search.cpp — LeetCode 79. Word Search (medium) — backtracking
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

// Backtracking: mark the cell as used, recurse on the next letter, unmark on the
// way out so a different path can reuse it. Unmarking is what makes this
// backtracking rather than plain DFS.
bool dfs(std::vector<std::vector<char>>& board, const std::string& word, int r, int c, int i) {
    const int rows = static_cast<int>(board.size());
    const int cols = static_cast<int>(board[0].size());
    if (i == static_cast<int>(word.size())) return true;
    if (r < 0 || r >= rows || c < 0 || c >= cols || board[r][c] != word[i]) return false;
    const char saved = board[r][c];
    board[r][c] = '#';  // in-place visited mark; '#' never matches a letter
    const bool found = dfs(board, word, r + 1, c, i + 1) || dfs(board, word, r - 1, c, i + 1) ||
                       dfs(board, word, r, c + 1, i + 1) || dfs(board, word, r, c - 1, i + 1);
    board[r][c] = saved;  // unmark: the board is restored for the next start cell
    return found;
}

bool exist(std::vector<std::vector<char>>& board, std::string word) {
    if (board.empty() || board[0].empty() || word.empty()) return false;
    const int rows = static_cast<int>(board.size());
    const int cols = static_cast<int>(board[0].size());
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            if (dfs(board, word, r, c, 0)) return true;
    return false;
}

int main() {
    std::vector<std::vector<char>> board = {
        {'A', 'B', 'C', 'E'},
        {'S', 'F', 'C', 'S'},
        {'A', 'D', 'E', 'E'}};
    assert(exist(board, "ABCCED") == true);
    assert(exist(board, "SEE") == true);
    assert(exist(board, "ABCB") == false);  // would need to reuse the B
    // Board is restored after every search.
    assert(board[0][0] == 'A' && board[1][1] == 'F');

    std::vector<std::vector<char>> one = {{'a'}};
    assert(exist(one, "a") == true);
    assert(exist(one, "ab") == false);
    std::vector<std::vector<char>> empty;
    assert(exist(empty, "a") == false);

    // Backtracking matters: seven A's ring the B. Starting at (0,0) the DFS
    // walks a short arc, fails to find C, unmarks, and a later start at (1,2)
    // snakes the long way round: (1,2)(0,2)(0,1)(0,0)(1,0)(2,0)(2,1) then C.
    std::vector<std::vector<char>> tricky = {
        {'A', 'A', 'A'},
        {'A', 'B', 'A'},
        {'A', 'A', 'C'}};
    assert(exist(tricky, "AAAAAAAC") == true);   // 7 A's then C
    assert(exist(tricky, "AAAAAAAAC") == false); // 8 A's: one too many
    assert(exist(tricky, "ABA") == true);
    assert(exist(tricky, "ABC") == false);       // B and C are only diagonal neighbours
    std::cout << "OK 09_word_search.cpp\n";
    return 0;
}
