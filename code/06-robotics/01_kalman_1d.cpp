// 1D (scalar-state) Kalman filter with a deterministic simulation.
// State x = position [m]. Control u = commanded velocity [m/s].
// Process model:      x_k = x_{k-1} + u*dt + w,   w ~ N(0, q*dt)
// Measurement model:  z_k = x_k + v,              v ~ N(0, r)
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

class Kalman1D {
public:
    // x0/p0: initial estimate and its variance. q: process noise variance per second.
    // r: measurement noise variance.
    Kalman1D(double x0, double p0, double q, double r) : x_(x0), p_(p0), q_(q), r_(r) {}

    // Predict step: x' = A x + B u with A = 1, B = dt. P' = A P A^T + Q  (A = 1 here).
    void predict(double u, double dt) {
        x_ += u * dt;
        p_ += q_ * dt;                  // uncertainty only ever grows in predict
    }

    // Update step with z = H x + v, H = 1.
    void update(double z) {
        innovation_ = z - x_;           // y = z - H x  ("what the sensor says minus what I expected")
        const double s = p_ + r_;       // S = H P H^T + R, variance of the innovation
        gain_ = p_ / s;                 // K = P H^T S^-1, in [0,1]: 0 = trust model, 1 = trust sensor
        x_ += gain_ * innovation_;
        p_ = (1.0 - gain_) * p_;        // P = (I - K H) P, always <= previous P
    }

    double state() const { return x_; }
    double variance() const { return p_; }
    double gain() const { return gain_; }
    double innovation() const { return innovation_; }

private:
    double x_, p_, q_, r_;
    double gain_ = 0.0;
    double innovation_ = 0.0;
};

double rms(const std::vector<double>& errors) {
    double sum = 0.0;
    for (double e : errors) sum += e * e;
    return std::sqrt(sum / static_cast<double>(errors.size()));
}

int main() {
    // ---- gain limits: the two questions every interviewer asks ----
    {
        Kalman1D trustSensor(0.0, 1.0, 0.0, 1e-9);   // R -> 0
        trustSensor.update(5.0);
        assert(std::fabs(trustSensor.state() - 5.0) < 1e-6);   // K -> 1, estimate jumps to z
        assert(trustSensor.gain() > 0.999);

        Kalman1D trustModel(0.0, 1e-9, 0.0, 1.0);    // P -> 0 (or R huge)
        trustModel.update(5.0);
        assert(std::fabs(trustModel.state()) < 1e-6);          // K -> 0, measurement ignored
        assert(trustModel.gain() < 1e-6);
    }

    // ---- P shrinks on update, grows on predict ----
    {
        Kalman1D kf(0.0, 4.0, 0.5, 1.0);
        const double p0 = kf.variance();
        kf.update(0.3);
        const double p1 = kf.variance();
        assert(p1 < p0);
        assert(std::fabs(p1 - (4.0 * 1.0) / (4.0 + 1.0)) < 1e-12);   // P R / (P + R)
        kf.predict(0.0, 0.1);
        assert(kf.variance() > p1);
    }

    // ---- simulation: target moving at commanded velocity with process noise, noisy sensor ----
    {
        const double dt = 0.1, q = 0.01, r = 1.0;   // sensor sigma = 1 m, model drifts slowly
        std::mt19937 rng(42);                       // fixed seed: the test is reproducible
        std::normal_distribution<double> processNoise(0.0, std::sqrt(q * dt));
        std::normal_distribution<double> measNoise(0.0, std::sqrt(r));

        double xTrue = 0.0;
        Kalman1D kf(0.0, 1.0, q, r);
        std::vector<double> rawErr, filtErr;
        for (int k = 0; k < 500; ++k) {
            const double u = 1.0;                   // commanded 1 m/s
            xTrue += u * dt + processNoise(rng);
            const double z = xTrue + measNoise(rng);

            kf.predict(u, dt);
            kf.update(z);

            rawErr.push_back(z - xTrue);
            filtErr.push_back(kf.state() - xTrue);
        }
        const double rawRms = rms(rawErr), filtRms = rms(filtErr);
        std::cout << "raw RMS = " << rawRms << " m, filtered RMS = " << filtRms
                  << " m, steady-state P = " << kf.variance() << ", K = " << kf.gain() << "\n";
        assert(filtRms < 0.5 * rawRms);            // the filter must beat the sensor clearly
        assert(kf.variance() < r);                  // posterior tighter than one measurement
    }

    std::cout << "OK 01_kalman_1d.cpp\n";
    return 0;
}
