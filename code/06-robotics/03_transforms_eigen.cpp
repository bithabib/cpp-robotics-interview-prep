// Rotations, quaternions and rigid transforms with Eigen, plus a hand-written 2D
// transform for the whiteboard. Frame convention: T_a_b maps points in frame b
// to frame a, i.e. p_a = T_a_b * p_b. Chains read right-to-left.
// Compile: g++ -std=c++17 -Wall -Wextra -O2 -I/usr/include/eigen3 03_transforms_eigen.cpp
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <cassert>
#include <cmath>
#include <iostream>

constexpr double kPi = 3.14159265358979323846;

// ---------- the whiteboard version: SE(2) by hand ----------
struct Transform2D {
    double x, y, theta;   // translation and heading of frame b expressed in frame a

    // p_a = R(theta) p_b + t
    void apply(double px, double py, double& outX, double& outY) const {
        const double c = std::cos(theta), s = std::sin(theta);
        outX = c * px - s * py + x;
        outY = s * px + c * py + y;
    }
    // T_a_c = T_a_b * T_b_c : rotate the child's translation into our frame, add headings
    Transform2D compose(const Transform2D& other) const {
        Transform2D out{};
        apply(other.x, other.y, out.x, out.y);
        out.theta = std::remainder(theta + other.theta, 2.0 * kPi);   // wrap to (-pi, pi]
        return out;
    }
    // T_b_a = inverse: R^T and -R^T t
    Transform2D inverse() const {
        const double c = std::cos(theta), s = std::sin(theta);
        return Transform2D{-(c * x + s * y), -(-s * x + c * y), -theta};
    }
};

bool isRotationMatrix(const Eigen::Matrix3d& R, double tol = 1e-9) {
    // Orthonormal columns and right-handed (det +1, not -1 which is a reflection).
    return (R.transpose() * R).isApprox(Eigen::Matrix3d::Identity(), tol) &&
           std::fabs(R.determinant() - 1.0) < tol;
}

int main() {
    // ---- axis-angle -> quaternion -> matrix and back ----
    const Eigen::AngleAxisd aa(kPi / 3.0, Eigen::Vector3d(1.0, 2.0, 3.0).normalized());
    const Eigen::Quaterniond q(aa);
    const Eigen::Matrix3d R = aa.toRotationMatrix();
    assert(std::fabs(q.norm() - 1.0) < 1e-12);            // unit quaternion
    assert(R.isApprox(q.toRotationMatrix()));
    assert(isRotationMatrix(R));
    assert(R.inverse().isApprox(R.transpose()));           // SO(3): inverse == transpose
    const Eigen::AngleAxisd back(q);
    assert(std::fabs(back.angle() - aa.angle()) < 1e-12);
    assert(back.axis().isApprox(aa.axis()));
    // Double cover: q and -q are the same rotation.
    const Eigen::Quaterniond negQ(-q.w(), -q.x(), -q.y(), -q.z());
    assert(negQ.toRotationMatrix().isApprox(R));
    assert(q.angularDistance(negQ) < 1e-12);

    // ---- composition: matrix product == quaternion product, and order matters ----
    const Eigen::Quaterniond q1(Eigen::AngleAxisd(0.7, Eigen::Vector3d::UnitZ()));
    const Eigen::Quaterniond q2(Eigen::AngleAxisd(-0.4, Eigen::Vector3d::UnitX()));
    const Eigen::Matrix3d R12 = q1.toRotationMatrix() * q2.toRotationMatrix();
    assert(R12.isApprox((q1 * q2).toRotationMatrix()));
    assert(!R12.isApprox((q2 * q1).toRotationMatrix()));   // rotations do not commute
    const Eigen::Vector3d v(1.0, 0.0, 0.0);
    assert((q1 * v).isApprox(q1.toRotationMatrix() * v)); // q * v is "rotate v by q"

    // ---- gimbal lock: at pitch = 90 deg roll and yaw become the same axis ----
    {
        const Eigen::Matrix3d Rlock = (Eigen::AngleAxisd(0.3, Eigen::Vector3d::UnitZ()) *
                                       Eigen::AngleAxisd(kPi / 2.0, Eigen::Vector3d::UnitY()) *
                                       Eigen::AngleAxisd(0.5, Eigen::Vector3d::UnitX())).toRotationMatrix();
        const Eigen::Matrix3d Rlock2 = (Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitZ()) *
                                        Eigen::AngleAxisd(kPi / 2.0, Eigen::Vector3d::UnitY()) *
                                        Eigen::AngleAxisd(0.2, Eigen::Vector3d::UnitX())).toRotationMatrix();
        assert(Rlock.isApprox(Rlock2, 1e-9));   // (yaw 0.3, roll 0.5) == (yaw 0, roll 0.2): a DOF is lost
    }

    // ---- SE(3) chain: world <- base <- camera ----
    Eigen::Isometry3d T_world_base = Eigen::Isometry3d::Identity();
    T_world_base.rotate(Eigen::AngleAxisd(kPi / 2.0, Eigen::Vector3d::UnitZ()));   // robot faces +y
    T_world_base.pretranslate(Eigen::Vector3d(2.0, 1.0, 0.0));                     // at (2,1,0)

    // Camera optical frame (x right, y down, z forward) expressed in body axes
    // (x forward, y left, z up). Columns of R_base_cam are the camera axes in base coords.
    Eigen::Matrix3d R_base_cam;
    R_base_cam.col(0) = Eigen::Vector3d(0.0, -1.0, 0.0);   // optical x -> body -y
    R_base_cam.col(1) = Eigen::Vector3d(0.0, 0.0, -1.0);   // optical y -> body -z
    R_base_cam.col(2) = Eigen::Vector3d(1.0, 0.0, 0.0);    // optical z -> body +x
    assert(isRotationMatrix(R_base_cam));
    Eigen::Isometry3d T_base_cam = Eigen::Isometry3d::Identity();
    T_base_cam.linear() = R_base_cam;
    T_base_cam.translation() = Eigen::Vector3d(0.3, 0.0, 0.5);   // 30 cm ahead, 50 cm up

    const Eigen::Isometry3d T_world_cam = T_world_base * T_base_cam;   // chain reads right-to-left
    const Eigen::Vector3d p_cam(0.0, 0.0, 2.0);                        // 2 m straight ahead (optical z)
    const Eigen::Vector3d p_world = T_world_cam * p_cam;
    // Camera looks along the robot's +x, robot's +x is world +y, camera sits at base (0.3,0,0.5):
    // point is at base (2.3, 0, 0.5) -> world (2 - 0, 1 + 2.3, 0.5)
    assert(p_world.isApprox(Eigen::Vector3d(2.0, 3.3, 0.5)));

    // Round trip through the inverse, and inverse of a chain is the reversed chain of inverses.
    assert((T_world_cam.inverse() * p_world).isApprox(p_cam));
    assert(T_world_cam.inverse().isApprox(T_base_cam.inverse() * T_world_base.inverse()));
    assert(isRotationMatrix(T_world_cam.rotation()));
    // Inverse by hand: [R^T, -R^T t]
    Eigen::Isometry3d invByHand = Eigen::Isometry3d::Identity();
    invByHand.linear() = T_world_cam.rotation().transpose();
    invByHand.translation() = -T_world_cam.rotation().transpose() * T_world_cam.translation();
    assert(invByHand.isApprox(T_world_cam.inverse()));

    // ---- slerp: constant angular velocity between two orientations ----
    {
        const Eigen::Quaterniond qa(Eigen::AngleAxisd(0.0, Eigen::Vector3d::UnitZ()));
        const Eigen::Quaterniond qb(Eigen::AngleAxisd(1.0, Eigen::Vector3d::UnitZ()));
        const Eigen::Quaterniond qHalf = qa.slerp(0.5, qb);
        const Eigen::AngleAxisd aaHalf(qHalf);
        assert(std::fabs(aaHalf.angle() - 0.5) < 1e-12);
        assert(qa.slerp(0.0, qb).isApprox(qa) && qa.slerp(1.0, qb).isApprox(qb));
        // Nlerp (normalize(lerp)) hits the same endpoints but is not constant-speed in between.
        Eigen::Quaterniond nlerp(qa.coeffs() * 0.5 + qb.coeffs() * 0.5);
        nlerp.normalize();
        assert(std::fabs(Eigen::AngleAxisd(nlerp).angle() - 0.5) < 1e-12);   // symmetric case: equal
    }

    // ---- 2D whiteboard version agrees with Eigen ----
    {
        const Transform2D T_a_b{1.0, 2.0, kPi / 4.0};
        const Transform2D T_b_c{0.5, -0.5, kPi / 6.0};
        const Transform2D T_a_c = T_a_b.compose(T_b_c);
        double px, py;
        T_a_c.apply(1.0, 1.0, px, py);

        const Eigen::Isometry2d E_a_b = Eigen::Translation2d(1.0, 2.0) * Eigen::Rotation2Dd(kPi / 4.0);
        const Eigen::Isometry2d E_b_c = Eigen::Translation2d(0.5, -0.5) * Eigen::Rotation2Dd(kPi / 6.0);
        const Eigen::Vector2d pe = (E_a_b * E_b_c) * Eigen::Vector2d(1.0, 1.0);
        assert(std::fabs(px - pe.x()) < 1e-12 && std::fabs(py - pe.y()) < 1e-12);

        const Transform2D ident = T_a_c.compose(T_a_c.inverse());
        assert(std::fabs(ident.x) < 1e-12 && std::fabs(ident.y) < 1e-12 && std::fabs(ident.theta) < 1e-12);
    }

    std::cout << "OK 03_transforms_eigen.cpp\n";
    return 0;
}
