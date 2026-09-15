// Closest pair of 2D points (brute force and divide & conquer) and line fitting:
// total least squares via the 2x2 covariance eigenvector, and RANSAC on top of it.
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <utility>
#include <vector>

struct Point { double x, y; };

double dist(const Point& a, const Point& b) { return std::hypot(a.x - b.x, a.y - b.y); }

// ---------- closest pair: O(n^2) reference ----------
double closestPairBrute(const std::vector<Point>& pts) {
    double best = std::numeric_limits<double>::infinity();
    const int n = static_cast<int>(pts.size());
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j) best = std::min(best, dist(pts[i], pts[j]));
    return best;
}

// ---------- closest pair: O(n log n) divide & conquer ----------
// px sorted by x; py the same points sorted by y (maintained through the recursion).
double closestRec(const std::vector<Point>& px, const std::vector<Point>& py) {
    const int n = static_cast<int>(px.size());
    if (n <= 3) return closestPairBrute(px);

    const int mid = n / 2;
    const double midX = px[mid].x;
    std::vector<Point> lx(px.begin(), px.begin() + mid), rx(px.begin() + mid, px.end());
    std::vector<Point> ly, ry;                       // split py by side, keeping y order
    for (const auto& p : py) {
        if (p.x < midX || (p.x == midX && ly.size() < lx.size())) ly.push_back(p);
        else ry.push_back(p);
    }
    double d = std::min(closestRec(lx, ly), closestRec(rx, ry));

    // Strip: points within d of the split line, in y order. Any pair closer than d
    // inside the strip is at most 7 positions apart in y order (packing argument).
    std::vector<Point> strip;
    for (const auto& p : py)
        if (std::fabs(p.x - midX) < d) strip.push_back(p);
    const int m = static_cast<int>(strip.size());
    for (int i = 0; i < m; ++i)
        for (int j = i + 1; j < m && strip[j].y - strip[i].y < d; ++j)
            d = std::min(d, dist(strip[i], strip[j]));
    return d;
}

double closestPairFast(std::vector<Point> pts) {
    if (pts.size() < 2) return std::numeric_limits<double>::infinity();
    std::vector<Point> px = pts, py = pts;
    std::sort(px.begin(), px.end(), [](const Point& a, const Point& b) { return a.x < b.x; });
    std::sort(py.begin(), py.end(), [](const Point& a, const Point& b) { return a.y < b.y; });
    return closestRec(px, py);
}

// ---------- line fitting ----------
// Line in normal form: nx*x + ny*y = d with (nx,ny) unit. Works for vertical lines,
// unlike y = m x + c.
struct Line { double nx, ny, d; };

double pointLineDistance(const Line& l, const Point& p) {
    return std::fabs(l.nx * p.x + l.ny * p.y - l.d);
}

// Total least squares: minimise perpendicular distances. The direction is the
// eigenvector of the 2x2 covariance with the LARGEST eigenvalue; the normal is the
// one with the smallest. Closed form for a symmetric 2x2, no Eigen needed.
Line fitLineTLS(const std::vector<Point>& pts) {
    const double n = static_cast<double>(pts.size());
    double mx = 0.0, my = 0.0;
    for (const auto& p : pts) { mx += p.x; my += p.y; }
    mx /= n; my /= n;
    double sxx = 0.0, sxy = 0.0, syy = 0.0;
    for (const auto& p : pts) {
        const double dx = p.x - mx, dy = p.y - my;
        sxx += dx * dx; sxy += dx * dy; syy += dy * dy;
    }
    // Smallest eigenvalue of [[sxx, sxy],[sxy, syy]]
    const double tr = sxx + syy, det = sxx * syy - sxy * sxy;
    const double lamMin = tr / 2.0 - std::sqrt(std::max(0.0, tr * tr / 4.0 - det));
    // Eigenvector for lamMin: (A - lam I) v = 0 -> pick the better-conditioned row.
    double nx, ny;
    if (std::fabs(sxy) > 1e-12) { nx = sxy; ny = lamMin - sxx; }
    else if (sxx < syy)           { nx = 1.0; ny = 0.0; }     // points spread along y: vertical line
    else                          { nx = 0.0; ny = 1.0; }
    const double len = std::hypot(nx, ny);
    nx /= len; ny /= len;
    return Line{nx, ny, nx * mx + ny * my};   // passes through the centroid
}

Line lineThrough(const Point& a, const Point& b) {
    double nx = -(b.y - a.y), ny = b.x - a.x;    // normal = perpendicular to the direction
    const double len = std::hypot(nx, ny);
    nx /= len; ny /= len;
    return Line{nx, ny, nx * a.x + ny * a.y};
}

struct RansacResult { Line line; std::vector<int> inliers; };

RansacResult ransacLine(const std::vector<Point>& pts, double threshold, int iterations, std::mt19937& rng) {
    const int n = static_cast<int>(pts.size());
    std::uniform_int_distribution<int> pick(0, n - 1);
    RansacResult best;
    for (int it = 0; it < iterations; ++it) {
        int i = pick(rng), j = pick(rng);
        if (i == j) continue;                                   // degenerate minimal set
        const Line cand = lineThrough(pts[i], pts[j]);
        std::vector<int> inl;
        for (int k = 0; k < n; ++k)
            if (pointLineDistance(cand, pts[k]) < threshold) inl.push_back(k);
        if (inl.size() > best.inliers.size()) { best.line = cand; best.inliers = std::move(inl); }
    }
    // Refit on the consensus set: the 2-point hypothesis is noisy, the TLS fit is not.
    std::vector<Point> inlierPts;
    for (int k : best.inliers) inlierPts.push_back(pts[k]);
    if (inlierPts.size() >= 2) best.line = fitLineTLS(inlierPts);
    return best;
}

// Iterations needed so that with probability p at least one sample is all-inlier.
int ransacIterations(double outlierRatio, int sampleSize, double p = 0.99) {
    const double allInlier = std::pow(1.0 - outlierRatio, sampleSize);
    return static_cast<int>(std::ceil(std::log(1.0 - p) / std::log(1.0 - allInlier)));
}

int main() {
    std::mt19937 rng(2024);

    // ---- closest pair: fast == brute on random sets, plus edge cases ----
    {
        std::uniform_real_distribution<double> u(0.0, 100.0);
        for (int trial = 0; trial < 20; ++trial) {
            const int n = 2 + trial * 25;
            std::vector<Point> pts;
            for (int i = 0; i < n; ++i) pts.push_back({u(rng), u(rng)});
            const double a = closestPairBrute(pts), b = closestPairFast(pts);
            assert(std::fabs(a - b) < 1e-9);
        }
        std::vector<Point> dup = {{1, 1}, {5, 5}, {1, 1}};    // duplicates -> distance 0
        assert(closestPairFast(dup) == 0.0);
        std::vector<Point> vertical = {{3, 0}, {3, 10}, {3, 4}, {3, 7}};   // all same x
        assert(std::fabs(closestPairFast(vertical) - 3.0) < 1e-12);
        assert(std::isinf(closestPairFast({{1, 2}})));         // single point: no pair
    }

    // ---- TLS handles a vertical line, where y = mx + c cannot ----
    {
        std::vector<Point> v = {{2, 0}, {2, 1}, {2, 2}, {2, 3}};
        const Line l = fitLineTLS(v);
        assert(std::fabs(std::fabs(l.nx) - 1.0) < 1e-12 && std::fabs(l.ny) < 1e-12);
        assert(std::fabs(std::fabs(l.d) - 2.0) < 1e-12);
        std::vector<Point> diag = {{0, 0}, {1, 1}, {2, 2}, {3, 3.1}};
        const Line ld = fitLineTLS(diag);
        for (const auto& p : diag) assert(pointLineDistance(ld, p) < 0.1);
    }

    // ---- RANSAC: 70 % inliers on a known line, 30 % uniform outliers ----
    {
        const Line truth = lineThrough({0.0, 1.0}, {10.0, 4.0});   // y = 0.3 x + 1
        std::normal_distribution<double> noise(0.0, 0.05);
        std::uniform_real_distribution<double> ux(0.0, 10.0), uy(-5.0, 10.0);
        std::vector<Point> pts;
        const int nIn = 140, nOut = 60;
        for (int i = 0; i < nIn; ++i) {
            const double x = ux(rng);
            pts.push_back({x, 0.3 * x + 1.0 + noise(rng)});
        }
        for (int i = 0; i < nOut; ++i) pts.push_back({ux(rng), uy(rng)});
        std::shuffle(pts.begin(), pts.end(), rng);

        const int iters = ransacIterations(0.3, 2);   // = 7 for p = 0.99
        assert(iters == 7);
        const RansacResult res = ransacLine(pts, 0.2, 50, rng);   // use a margin over the minimum

        const double cosAngle = std::fabs(res.line.nx * truth.nx + res.line.ny * truth.ny);
        assert(cosAngle > 0.9999);                                 // same direction (sign-free)
        const double dSigned = res.line.d * (res.line.nx * truth.nx + res.line.ny * truth.ny > 0 ? 1.0 : -1.0);
        assert(std::fabs(dSigned - truth.d) < 0.05);
        const int nInl = static_cast<int>(res.inliers.size());
        assert(nInl >= nIn - 5 && nInl <= nIn + 15);               // ~all inliers, few lucky outliers
        std::cout << "RANSAC: " << nInl << " inliers of " << pts.size()
                  << ", line normal (" << res.line.nx << ", " << res.line.ny << "), d = " << res.line.d << "\n";
    }

    std::cout << "OK 04_closest_pair_and_ransac.cpp\n";
    return 0;
}
