#pragma once

#include <cfloat>
#include <cmath>
#include <vector>

namespace DevLib
{
	namespace Mathematics
	{
		static constexpr double PI = 3.14159265358979323846;
		static constexpr double R2D = 180.0 / PI;
		static constexpr double D2R = PI / 180.0;

		inline bool IsEqual(const double x, const double y, const double epsilon = DBL_EPSILON)
		{
			if (fabs(x) < epsilon || fabs(y) < epsilon)
			{
				return fabs(x - y) <= epsilon;
			}

			return fabs(x - y) <= epsilon * fmax(fabs(x), fabs(y));
		}

		inline bool IsEqual(const float x, const float y, const float epsilon = FLT_EPSILON)
		{
			if (std::fabs(x) < epsilon || fabs(y) < epsilon)
			{
				return fabs(x - y) <= epsilon;
			}

			return fabs(x - y) <= epsilon * fmax(fabs(x), fabs(y));
		}

		inline double RadianToDegree(const double rad) { return rad * R2D; }
		inline double DegreeToRadian(const double deg) { return deg * D2R; }

		inline float RadianToDegree(const float rad) { return rad * static_cast<float>(R2D); }
		inline float DegreeToRadian(const float  deg) { return deg * static_cast<float>(D2R); }

		double GaussianRandom();

		class CSinCosTable
		{
		public:
			CSinCosTable(double resDeg = 0.01);

			bool Reset(double resDeg);

			double sin(double radian) const;
			double cos(double radian) const;

			double sinDeg(double deg) const;
			double cosDeg(double deg) const;

		private:
			double resolution;

			std::vector<double> tableSin;
			std::vector<double> tableCos;
		};

	}
}
