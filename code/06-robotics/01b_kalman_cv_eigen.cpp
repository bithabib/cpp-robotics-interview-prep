// Constant-velocity Kalman filter in matrix form with Eigen.
// State x = [p, v]^T. Only position is measured.
//   x_k = A x_{k-1} + w,   A = [1 dt; 0 1],  w ~ N(0, Q)
//   z_k = H x_k + v,       H = [1 0],        v ~ N(0, R)
// Compile: g++ -std=c++17 -Wall -Wextra -O2 -I/usr/include/eigen3 01b_kalman_cv_eigen.cpp
#include <Eigen/Dense>
#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

class KalmanCV {
public:
    // accelSigma: standard deviation of the unmodelled acceleration (drives Q).
    // r: position measurement variance.
    KalmanCV(double dt, double accelSigma, double r) : dt_(dt), r_(r) {
        A_ << 1.0, dt,
              0.0, 1.0;
        H_ << 1.0, 0.0;
        // Discrete white-noise-acceleration model: Q = sigma_a^2 * [dt^4/4 dt^3/2; dt^3/2 dt^2]
        const double s2 = accelSigma * accelSigma;
        Q_ << s2 * dt * dt * dt * dt / 4.0, s2 * dt * dt * dt / 2.0,
              s2 * dt * dt * dt / 2.0,      s2 * dt * dt;
        x_.setZero();
        P_ = Eigen::Matrix2d::Identity() * 10.0;   // "I don't know much yet"
    }

    void predict() {
        x_ = A_ * x_;
        P_ = A_ * P_ * A_.transpose() + Q_;
    }

    void update(double z) {
        const double y = z - (H_ * x_)(0);                    // innovation (scalar)
        const double S = (H_ * P_ * H_.transpose())(0) + r_;  // innovation covariance (1x1)
        const Eigen::Vector2d K = P_ * H_.transpose() / S;    // 2x1 gain
        x_ += K * y;
        // Joseph form keeps P symmetric positive-definite under round-off; the
        // textbook (I - K H) P is fine on a whiteboard, this one is what you ship.
        const Eigen::Matrix2d IKH = Eigen::Matrix2d::Identity() - K * H_;
        P_ = IKH * P_ * IKH.transpose() + K * r_ * K.transpose();
    }

    const Eigen::Vector2d& state() const { return x_; }
    const Eigen::Matrix2d& covariance() const { return P_; }

private:
    double dt_, r_;
    Eigen::Matrix2d A_, Q_, P_;
    Eigen::RowVector2d H_;
    Eigen::Vector2d x_;
};

int main() {
    const double dt = 0.1, accelSigma = 0.2, r = 1.0;
    std::mt19937 rng(7);
    std::normal_distribution<double> accelNoise(0.0, accelSigma);
    std::normal_distribution<double> measNoise(0.0, std::sqrt(r));

    double pTrue = 0.0, vTrue = 1.0;
    KalmanCV kf(dt, accelSigma, r);
    double rawSq = 0.0, filtSq = 0.0;
    const int steps = 600;
    for (int k = 0; k < steps; ++k) {
        const double a = accelNoise(rng);
        pTrue += vTrue * dt + 0.5 * a * dt * dt;
        vTrue += a * dt;
        const double z = pTrue + measNoise(rng);

        kf.predict();
        kf.update(z);

        rawSq += (z - pTrue) * (z - pTrue);
        filtSq += (kf.state()(0) - pTrue) * (kf.state()(0) - pTrue);
    }
    const double rawRms = std::sqrt(rawSq / steps), filtRms = std::sqrt(filtSq / steps);
    std::cout << "raw RMS = " << rawRms << ", filtered RMS = " << filtRms
              << ", v_est = " << kf.state()(1) << " (true " << vTrue << ")\n";
    std::cout << "P =\n" << kf.covariance() << "\n";

    assert(filtRms < 0.5 * rawRms);
    assert(std::fabs(kf.state()(1) - vTrue) < 0.5);              // velocity is never measured, yet estimated
    assert(kf.covariance().isApprox(kf.covariance().transpose()));  // symmetric
    assert(kf.covariance()(0, 0) > 0.0 && kf.covariance()(1, 1) > 0.0);
    assert(kf.covariance()(0, 0) < r);                               // better than a single measurement

    std::cout << "OK 01b_kalman_cv_eigen.cpp\n";
    return 0;
}
