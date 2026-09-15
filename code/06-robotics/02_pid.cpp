// Discrete PID controller with output saturation, integral anti-windup and a
// low-pass-filtered derivative on measurement. Plus a mass-damper simulation
// that shows a naive controller winding up and the anti-windup one not.
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <vector>

enum class AntiWindup { None, Clamp, BackCalculation };

class PID {
public:
    struct Gains { double kp, ki, kd; };

    PID(Gains g, double outMin, double outMax, AntiWindup mode, double dFilterTau = 0.0)
        : g_(g), outMin_(outMin), outMax_(outMax), mode_(mode), dTau_(dFilterTau) {}

    // One control step. Returns the saturated command.
    double update(double setpoint, double measurement, double dt) {
        const double error = setpoint - measurement;

        // Derivative on MEASUREMENT, not on error: a setpoint step then produces no
        // "derivative kick" (a one-sample spike of size kd*step/dt).
        double dMeas = 0.0;
        if (hasPrev_) dMeas = (measurement - prevMeas_) / dt;
        // First-order low-pass on the derivative; raw differences amplify sensor noise.
        const double alpha = dTau_ > 0.0 ? dt / (dTau_ + dt) : 1.0;
        dFiltered_ += alpha * (dMeas - dFiltered_);
        prevMeas_ = measurement;
        hasPrev_ = true;

        double integralCandidate = integral_ + error * dt;
        const double p = g_.kp * error;
        const double d = -g_.kd * dFiltered_;   // minus: d(error)/dt = -d(meas)/dt for fixed setpoint

        double unsat = p + g_.ki * integralCandidate + d;
        double out = std::clamp(unsat, outMin_, outMax_);

        switch (mode_) {
            case AntiWindup::None:
                integral_ = integralCandidate;               // keeps growing while saturated
                break;
            case AntiWindup::Clamp:
                // Conditional integration: only integrate if we are not saturated, or if
                // the error would drive the output back out of saturation.
                if (unsat == out || (unsat > outMax_ && error < 0.0) || (unsat < outMin_ && error > 0.0))
                    integral_ = integralCandidate;
                break;
            case AntiWindup::BackCalculation: {
                // Feed the saturation excess (out - unsat) back into the integrator so it
                // is pulled toward the value that would make unsat == out.
                const double kb = g_.ki > 0.0 ? 1.0 / g_.ki : 0.0;   // typical: kb ~ 1/Ti
                integral_ = integralCandidate + kb * (out - unsat) * dt;
                break;
            }
        }
        lastUnsat_ = unsat;
        return out;
    }

    void reset() { integral_ = 0.0; dFiltered_ = 0.0; hasPrev_ = false; }
    double integral() const { return integral_; }
    double lastUnsaturated() const { return lastUnsat_; }

private:
    Gains g_;
    double outMin_, outMax_;
    AntiWindup mode_;
    double dTau_;
    double integral_ = 0.0;
    double dFiltered_ = 0.0;
    double prevMeas_ = 0.0;
    double lastUnsat_ = 0.0;
    bool hasPrev_ = false;
};

// Plant: mass on a damper, force input.  m*a = u - b*v.  Output = position.
struct MassDamper {
    double m = 1.0, b = 0.8;
    double x = 0.0, v = 0.0;
    void step(double u, double dt) {
        const double a = (u - b * v) / m;
        v += a * dt;
        x += v * dt;
    }
};

struct SimResult {
    double overshoot;   // max(x) - setpoint, in metres
    double finalError;  // |x - setpoint| at the end
    double settleTime;  // first time after which |error| stays < 2 %
};

SimResult simulate(AntiWindup mode, bool print) {
    const double dt = 0.01, tEnd = 80.0, setpoint = 10.0;
    const double uMax = 1.0;                       // a weak actuator: forces a long saturated run-up
    PID pid({1.0, 0.4, 1.2}, -uMax, uMax, mode, 0.05);
    MassDamper plant;

    SimResult res{0.0, 0.0, tEnd};
    double lastOutside = 0.0;
    const int steps = static_cast<int>(tEnd / dt);
    if (print) std::printf("   t    setpoint   position   command   integral\n");
    for (int k = 0; k <= steps; ++k) {
        const double t = k * dt;
        const double u = pid.update(setpoint, plant.x, dt);
        if (print && k % 800 == 0)
            std::printf("%5.1f  %8.2f  %9.3f  %8.3f  %9.3f\n", t, setpoint, plant.x, u, pid.integral());
        plant.step(u, dt);
        res.overshoot = std::max(res.overshoot, plant.x - setpoint);
        if (std::fabs(plant.x - setpoint) > 0.02 * setpoint) lastOutside = t;
    }
    res.finalError = std::fabs(plant.x - setpoint);
    res.settleTime = lastOutside;
    return res;
}

int main() {
    // ---- unit behaviour: no derivative kick on a setpoint step ----
    {
        PID pid({1.0, 0.0, 1.0}, -100.0, 100.0, AntiWindup::None);
        pid.update(0.0, 0.0, 0.1);                       // prime prevMeas_
        const double out = pid.update(1.0, 0.0, 0.1);    // setpoint jumps, measurement does not
        assert(std::fabs(out - 1.0) < 1e-12);            // pure P; derivative-on-error would give 11
    }
    // ---- unit behaviour: clamped integral stops growing while saturated ----
    {
        PID pid({1.0, 1.0, 0.0}, -1.0, 1.0, AntiWindup::Clamp);
        for (int i = 0; i < 100; ++i) pid.update(10.0, 0.0, 0.1);
        assert(pid.integral() < 1e-9);                   // never integrated: always saturated positive
        PID naive({1.0, 1.0, 0.0}, -1.0, 1.0, AntiWindup::None);
        for (int i = 0; i < 100; ++i) naive.update(10.0, 0.0, 0.1);
        assert(std::fabs(naive.integral() - 100.0) < 1e-9);   // 100 steps * 10 * 0.1
    }

    std::cout << "--- naive (no anti-windup) ---\n";
    const SimResult naive = simulate(AntiWindup::None, true);
    std::cout << "--- integral clamping ---\n";
    const SimResult clamp = simulate(AntiWindup::Clamp, true);
    const SimResult backCalc = simulate(AntiWindup::BackCalculation, false);

    std::printf("overshoot: naive %.3f m, clamp %.3f m, back-calc %.3f m\n",
                naive.overshoot, clamp.overshoot, backCalc.overshoot);
    std::printf("settle 2%%: naive %.1f s, clamp %.1f s, back-calc %.1f s\n",
                naive.settleTime, clamp.settleTime, backCalc.settleTime);

    assert(clamp.finalError < 0.05);
    assert(backCalc.finalError < 0.05);
    std::printf("final error: naive %.3f, clamp %.3f\n", naive.finalError, clamp.finalError);
    assert(naive.finalError < 0.5);                   // it gets there eventually
    assert(clamp.overshoot < 0.5 * naive.overshoot);  // anti-windup clearly helps
    assert(backCalc.overshoot < 0.5 * naive.overshoot);
    assert(clamp.settleTime < naive.settleTime);

    std::cout << "OK 02_pid.cpp\n";
    return 0;
}
