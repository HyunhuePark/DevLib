#pragma once

#include "DevLibTypes.hpp"

#define EIGEN_MPL2_ONLY

#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable : 4819)
#endif

#ifdef Success
#undef Success
#endif

#include "../3rd/Eigen3/include/eigen3/Eigen/Core"

#if defined(_MSC_VER)
#pragma warning (pop)
#endif

namespace DevLib
{
	using CPoint = Eigen::Vector2i;
	using CPointf = Eigen::Vector2f;
	using CPointd = Eigen::Vector2d;

	using CPoint3D = Eigen::Vector3i;
	using CPoint3Df = Eigen::Vector3f;
	using CPoint3Dd = Eigen::Vector3d;

	using CPoint4D = Eigen::Vector4i;
	using CPoint4Df = Eigen::Vector4f;
	using CPoint4Dd = Eigen::Vector4d;

	template<typename Type> Type Rotation2D(const Type& point, double angle)
	{
		constexpr double pi = 3.141592653589793;
		const double radian = angle * pi / 180.0;

		Eigen::Matrix<typename Type::Scalar, 2, 2> rotationMatrix;
		rotationMatrix << std::cos(radian), -std::sin(radian), std::sin(radian), std::cos(radian);

		return rotationMatrix * point;
	}

	template<typename Scalar> Eigen::Matrix<Scalar, 3, 1> Rotation3D(const Eigen::Matrix<Scalar, 3, 1>& point, const Eigen::Matrix<Scalar, 3, 1>& axis, Scalar angle)
	{
		constexpr double pi = 3.141592653589793;
		const Scalar radian = angle * pi / 180.0;

		Eigen::AngleAxis<Scalar> rotation(radian, axis.normalized());

		return rotation * point;
	}

}