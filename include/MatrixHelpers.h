#pragma once

#include <iostream>

#include "Eigen"

extern "C" {
#include "apriltag.h"
#include "apriltag_pose.h"
#include "tag36h11.h"
//#include "tag16h5.h"
//#include "common/getopt.h"
}

template <typename T>
inline std::string to_string( const T& value )
{
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

inline double Deg2Rad(double deg){
    return deg * M_PI / 180.0;
}

inline double Rad2Deg(double rad){
    return rad * 180.0 / M_PI;
}

inline Eigen::Vector3d Rad2Deg(Eigen::Vector3d& vec){
    Eigen::Vector3d ret_vec;
    for (int i=0; i<3; i++){
        ret_vec(i) = vec(i) * 180.0 / M_PI;
    }
    return ret_vec;
}

inline Eigen::Vector3d Deg2Rad(Eigen::Vector3d& vec){
    Eigen::Vector3d ret_vec;
    for (int i=0; i<3; i++){
        ret_vec(i) = vec(i) * M_PI / 180.0;
    }
    return ret_vec;
}

// Template function to convert Eigen matrix or vector from radians to degrees
template <typename Derived>
inline Eigen::MatrixBase<Derived>& Rad2Deg(Eigen::MatrixBase<Derived>& matrix) {
    matrix = matrix.unaryExpr([](typename Derived::Scalar angle) {
        return angle * 180.0 / M_PI;
    });
    return matrix;
}

// Templated function to convert a raw C++ array to an Eigen matrix
template <typename Scalar, int Rows, int Cols>
inline Eigen::Matrix<Scalar, Rows, Cols> Array2EM(const Scalar* array) {
    Eigen::Matrix<Scalar, Rows, Cols> matrix;
    for (int i = 0; i < Rows; ++i) {
        for (int j = 0; j < Cols; ++j) {
            matrix(i, j) = array[i * Cols + j];
        }
    }
    return matrix;
}

// Overloading the << operator for Eigen matrices and vectors
template <typename Derived>
std::ostream& operator<<(std::ostream& os, const Eigen::MatrixBase<Derived>& matrix) {
    os << "[";
    for (int i = 0; i < matrix.rows(); ++i) {
        os << "[";
        for (int j = 0; j < matrix.cols(); ++j) {
            os << matrix(i, j);
            if (j != matrix.cols() - 1) {
                os << ", ";
            }
        }
        os << "]";
        if (i != matrix.rows() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

// Helper function to create a rotation matrix from roll, pitch, yaw, in degrees. 3-2-1 conversion i.e. yaw, pitch, roll
inline Eigen::Matrix3d CreateRotationMatrix(const Eigen::Vector3d& V) {
    double roll = Deg2Rad(V(0));
    double pitch = Deg2Rad(V(1));
    double yaw = Deg2Rad(V(2));

    Eigen::Matrix3d R_x;

    R_x << 1, 0, 0,
            0, cos(roll), -sin(roll),
            0, sin(roll), cos(roll);

    // Rotation matrix around the Y-axis (pitch)
    Eigen::Matrix3d R_y;
    R_y << cos(pitch), 0, sin(pitch),
            0, 1, 0,
            -sin(pitch), 0, cos(pitch);

    // Rotation matrix around the Z-axis (yaw)
    Eigen::Matrix3d R_z;
    R_z << cos(yaw), -sin(yaw), 0,
            sin(yaw), cos(yaw), 0,
            0, 0, 1;

    // Combined rotation matrix
    Eigen::Matrix3d R = R_z * R_y * R_x;

    return R;
}



// Function to convert a rotation matrix to roll, pitch, and yaw (in degrees)
inline Eigen::Vector3d RotationMatrixToRPY(const Eigen::Matrix3d& R) {
    // Calculate pitch
    double roll, pitch, yaw;
    Eigen::Vector3d rpy;
    pitch = asin(-R(2, 0));

    // Handle gimbal lock case where cos(pitch) is near zero
    if (abs(cos(pitch)) > 1e-6) {
        roll = atan2(R(2, 1), R(2, 2));
        yaw = atan2(R(1, 0), R(0, 0));
    } else {
        // Gimbal lock, we have to set one of the angles to zero
        roll = 0.0;
        yaw = atan2(-R(0, 1), R(1, 1));
    }

    // Convert to degrees
    roll = Rad2Deg(roll);
    pitch = Rad2Deg(pitch);
    yaw = Rad2Deg(yaw);
    rpy = {roll, pitch, yaw};
    return rpy;
}
inline Eigen::Vector3d RotationMatrixToRPY(const matd_t* R) {
    Eigen::Matrix3d E_R;
    for (int i=0; i<3; i++){
        for (int j=0; j<3; j++){
            E_R(i,j) = MATD_EL(R, i, j);
        }
    }
    return RotationMatrixToRPY(E_R);
}