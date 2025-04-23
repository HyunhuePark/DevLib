#pragma once

#define EIGEN_MPL2_ONLY

#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable : 4819)
#endif
/**
 * \brief Eigen3 MPL2 
 */

#ifdef Success
#undef Success
#endif

#include "../3rd/Eigen3/include/eigen3/Eigen/Core"
#include "../3rd/Eigen3/include/eigen3/Eigen/Geometry"

#if defined(_MSC_VER)
#pragma warning (pop)
#endif

namespace DevLib
{
	// Point Types
	using CMatrix22 = Eigen::Matrix2i;
	using CMatrix22f = Eigen::Matrix2f;
	using CMatrix22d = Eigen::Matrix2d;

	using CMatrix33 = Eigen::Matrix3i;
	using CMatrix33f = Eigen::Matrix3f;
	using CMatrix33d = Eigen::Matrix3d;

	using CMatrix44 = Eigen::Matrix4i;
	using CMatrix44f = Eigen::Matrix4f;
	using CMatrix44d = Eigen::Matrix4d;

	using CTransform33f = Eigen::Transform<float, 3,3>;

};