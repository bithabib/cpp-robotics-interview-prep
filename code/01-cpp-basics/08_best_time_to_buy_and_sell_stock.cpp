// LeetCode 121. Best Time to Buy and Sell Stock
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

// One pass: the best sale at day i uses the cheapest buy seen so far.
// Track minPrice and the best profit; never sell before you buy.
int maxProfit(std::vector<int>& prices) {
    int minPrice = prices[0];
    int best = 0;                          // "no transaction" is always allowed
    for (size_t i = 1; i < prices.size(); ++i) {
        best = std::max(best, prices[i] - minPrice);
        minPrice = std::min(minPrice, prices[i]);
    }
    return best;
}

int main() {
    std::vector<int> a = {7, 1, 5, 3, 6, 4};
    assert(maxProfit(a) == 5);            // buy 1, sell 6

    std::vector<int> b = {7, 6, 4, 3, 1};
    assert(maxProfit(b) == 0);            // monotone decreasing: never buy

    std::vector<int> c = {5};
    assert(maxProfit(c) == 0);            // single day

    std::vector<int> d = {2, 4, 1};
    assert(maxProfit(d) == 2);            // the later min (1) must not be paired with earlier 4

    std::cout << "OK 08_best_time_to_buy_and_sell_stock.cpp\n";
    return 0;
}
