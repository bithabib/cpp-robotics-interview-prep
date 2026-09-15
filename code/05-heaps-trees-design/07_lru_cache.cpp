// LeetCode 146. LRU Cache
#include <cassert>
#include <iostream>
#include <list>
#include <unordered_map>
#include <utility>

// Recency order lives in a doubly linked list (front = most recently used).
// The hash map gives O(1) key -> list node. std::list iterators stay valid
// through splice/erase of OTHER nodes, which is exactly what makes this O(1);
// a std::vector would invalidate them on every move.
class LRUCache {
public:
    explicit LRUCache(int capacity) : capacity_(capacity) {}

    int get(int key) {
        auto it = map_.find(key);
        if (it == map_.end()) return -1;
        touch(it->second);                       // move to front, O(1)
        return it->second->second;
    }

    void put(int key, int value) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            it->second->second = value;          // update in place, then mark as recent
            touch(it->second);
            return;
        }
        if (static_cast<int>(items_.size()) == capacity_) {
            map_.erase(items_.back().first);     // evict least recently used (back)
            items_.pop_back();
        }
        items_.emplace_front(key, value);
        map_[key] = items_.begin();
    }

private:
    using Node = std::pair<int, int>;            // (key, value): key needed to erase from map on eviction
    using Iter = std::list<Node>::iterator;

    void touch(Iter it) {
        // relink the node at the front without copying or reallocating; `it` stays valid
        items_.splice(items_.begin(), items_, it);
    }

    int capacity_;
    std::list<Node> items_;
    std::unordered_map<int, Iter> map_;
};

int main() {
    {
        LRUCache c(2);                           // LeetCode example
        c.put(1, 1);
        c.put(2, 2);
        assert(c.get(1) == 1);
        c.put(3, 3);                             // evicts key 2 (1 was just touched)
        assert(c.get(2) == -1);
        c.put(4, 4);                             // evicts key 1
        assert(c.get(1) == -1);
        assert(c.get(3) == 3);
        assert(c.get(4) == 4);
    }
    {
        LRUCache c(1);                           // capacity 1: every put of a new key evicts
        c.put(5, 50);
        assert(c.get(5) == 50);
        c.put(6, 60);
        assert(c.get(5) == -1);
        assert(c.get(6) == 60);
        c.put(6, 61);                            // overwrite existing key, no eviction
        assert(c.get(6) == 61);
    }
    {
        LRUCache c(2);
        c.put(2, 1);
        c.put(2, 2);                             // update keeps size at 1
        assert(c.get(2) == 2);
        c.put(1, 1);
        c.put(4, 1);                             // evicts 2 (1 is more recent)
        assert(c.get(2) == -1);
        assert(c.get(1) == 1);
    }
    {
        LRUCache c(3);
        assert(c.get(42) == -1);                 // get on empty cache
    }
    std::cout << "OK 07_lru_cache.cpp\n";
    return 0;
}
