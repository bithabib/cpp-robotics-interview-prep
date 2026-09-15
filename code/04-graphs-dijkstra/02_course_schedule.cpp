// LeetCode 207. Course Schedule
#include <cassert>
#include <iostream>
#include <queue>
#include <vector>

// Kahn's algorithm: repeatedly take a course with no remaining prerequisites.
// If we can take all n, the prerequisite graph is a DAG.
bool canFinish(int numCourses, std::vector<std::vector<int>>& prerequisites) {
    std::vector<std::vector<int>> adj(numCourses);   // prereq -> course
    std::vector<int> indeg(numCourses, 0);
    for (const auto& p : prerequisites) {
        adj[p[1]].push_back(p[0]);                   // [a, b] means "b before a"
        ++indeg[p[0]];
    }
    std::queue<int> q;
    for (int c = 0; c < numCourses; ++c)
        if (indeg[c] == 0) q.push(c);
    int taken = 0;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        ++taken;
        for (int v : adj[u])
            if (--indeg[v] == 0) q.push(v);
    }
    return taken == numCourses;                      // nodes left with indeg > 0 sit on a cycle
}

int main() {
    std::vector<std::vector<int>> p1 = {{1, 0}};
    assert(canFinish(2, p1));

    std::vector<std::vector<int>> p2 = {{1, 0}, {0, 1}};   // 2-cycle
    assert(!canFinish(2, p2));

    std::vector<std::vector<int>> p3;                       // no prerequisites at all
    assert(canFinish(1, p3));
    assert(canFinish(5, p3));

    std::vector<std::vector<int>> p4 = {{1, 0}, {2, 1}, {3, 2}, {1, 3}};   // cycle 1->2->3->1 off a chain
    assert(!canFinish(4, p4));

    std::vector<std::vector<int>> p5 = {{1, 0}, {2, 0}, {3, 1}, {3, 2}};   // diamond DAG
    assert(canFinish(4, p5));

    std::cout << "OK 02_course_schedule.cpp\n";
    return 0;
}
