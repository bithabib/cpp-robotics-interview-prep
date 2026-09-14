// 00_memory_demo.cpp — pointers vs references, const, RAII, unique_ptr/shared_ptr,
// stack vs heap, and the size_t trap (shown safely).
#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ---------- m1: pointers vs references ----------
void byValue(int a) { ++a; }           // ++a touches a private copy; caller sees nothing
void byRef(int& a) { ++a; }            // alias of the caller's variable
void byPtr(int* a) { if (a) ++*a; }    // may be null, must be checked; *a is the pointee

// ---------- m2: const-correctness ----------
class Pose2D {
public:
    Pose2D(double x, double y) : x_(x), y_(y) {}      // member init list
    double x() const { return x_; }                   // const method: callable on a const Pose2D
    void translate(double dx) { x_ += dx; }           // non-const: mutates
    std::size_t hits() const { ++cacheHits_; return cacheHits_; } // mutable: bookkeeping in a const method
private:
    double x_, y_;
    mutable std::size_t cacheHits_ = 0;
};

// const T& — read-only view, no copy. Passing std::string by value would copy the buffer.
std::size_t countA(const std::string& s) {
    std::size_t n = 0;
    for (char c : s) if (c == 'a') ++n;
    return n;
}

// ---------- m3: RAII ----------
// The destructor runs when the object leaves scope, in every exit path (return, exception).
struct ScopeLog {
    ScopeLog(std::string name, std::vector<std::string>& log) : name_(std::move(name)), log_(log) {
        log_.push_back("open " + name_);
    }
    ~ScopeLog() { log_.push_back("close " + name_); }
    ScopeLog(const ScopeLog&) = delete;            // a resource handle should not be copied
    ScopeLog& operator=(const ScopeLog&) = delete;
private:
    std::string name_;
    std::vector<std::string>& log_;
};

// ---------- m4: ownership with smart pointers ----------
struct Sensor {
    virtual ~Sensor() = default;                   // virtual: deleting through a base pointer must run the derived dtor
    virtual std::string name() const = 0;
};
struct Lidar : Sensor {
    std::string name() const override { return "lidar"; }
};

// Non-owning observer: a raw pointer is fine when the callee never deletes.
std::string describe(const Sensor* s) { return s ? s->name() : "none"; }

// ---------- m5: stack vs heap ----------
// Returning a reference to a local is a dangling reference — never do this.
// int& bad() { int x = 1; return x; }   // g++ -Wall warns: reference to local variable
std::vector<int> good() { std::vector<int> v = {1, 2, 3}; return v; } // moved/elided, no copy, no leak

int main() {
    // m1
    int x = 1;
    byValue(x); assert(x == 1);
    byRef(x);   assert(x == 2);
    byPtr(&x);  assert(x == 3);
    byPtr(nullptr);                       // safe because byPtr checks
    int* p = &x; *p = 10; assert(x == 10);
    int& r = x; r = 11; assert(x == 11);  // r cannot be reseated: "r = y" would assign into x
    Pose2D pose(1.0, 2.0);
    Pose2D* pp = &pose;
    assert(pp->x() == 1.0);               // -> is (*pp).x()

    // m2
    const Pose2D cp(5.0, 0.0);
    assert(cp.x() == 5.0);                // OK: x() is const
    // cp.translate(1.0);                 // compile error: non-const method on const object
    assert(cp.hits() == 1);               // mutable member changed inside a const method
    assert(countA("banana") == 3);
    const int* ptrToConst = &x;           // cannot write *ptrToConst; can repoint
    int* const constPtr = &x;             // can write *constPtr; cannot repoint
    *constPtr = 12; assert(*ptrToConst == 12);

    // m3
    std::vector<std::string> log;
    {
        ScopeLog a("A", log);
        ScopeLog b("B", log);
        log.push_back("work");
    }                                     // destroyed in reverse order: B then A
    assert((log == std::vector<std::string>{"open A", "open B", "work", "close B", "close A"}));

    // m4
    std::unique_ptr<Sensor> owner = std::make_unique<Lidar>();  // sole owner, zero overhead
    assert(describe(owner.get()) == "lidar");                   // lend a non-owning view
    std::unique_ptr<Sensor> other = std::move(owner);            // ownership transferred, not copied
    assert(owner == nullptr && other->name() == "lidar");

    std::shared_ptr<Sensor> s1 = std::make_shared<Lidar>();
    { std::shared_ptr<Sensor> s2 = s1; assert(s1.use_count() == 2); }
    assert(s1.use_count() == 1);                                 // s2 released its share
    std::weak_ptr<Sensor> w = s1;                                // observes without owning
    assert(!w.expired());
    s1.reset();
    assert(w.expired());                                          // weak_ptr knows the object is gone

    // m5
    assert(good().size() == 3);
    {
        std::vector<int> heapBacked(1000, 7);   // the vector's buffer is on the heap, freed by its dtor
        assert(heapBacked[999] == 7);
    }
    int* raw = new int(5);                      // legacy style: you now own a delete
    assert(*raw == 5);
    delete raw;                                 // forget this (or throw before it) and you leak
    raw = nullptr;

    // m6: size_t is unsigned
    std::vector<int> empty;
    std::size_t wrapped = empty.size() - 1;     // 0 - 1 wraps to SIZE_MAX, not -1
    assert(wrapped == static_cast<std::size_t>(-1));
    // Safe backwards loops:
    std::vector<int> v = {1, 2, 3};
    int sum = 0;
    for (int i = static_cast<int>(v.size()) - 1; i >= 0; --i) sum += v[i];   // idiom 1: cast once
    assert(sum == 6);
    sum = 0;
    for (std::size_t i = v.size(); i-- > 0;) sum += v[static_cast<int>(i)];   // idiom 2: test-then-decrement
    assert(sum == 6);
    // while (--n >= 0) with std::size_t n would never be false; with int n it is fine:
    int n = 3, ticks = 0;
    while (--n >= 0) ++ticks;
    assert(ticks == 3);

    std::cout << "OK 00_memory_demo.cpp\n";
    return 0;
}
