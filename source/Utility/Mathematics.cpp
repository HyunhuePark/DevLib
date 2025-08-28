#include "../../include/Utility/Mathematics.hpp"

#include <random>
#include <cmath>

namespace DevLib {
	namespace Mathematics {

		double GaussianRandom()
		{
			static std::mt19937 generator(std::random_device{}());
			static std::uniform_real_distribution distribution(0.0, 1.0);

			double v1, s;

			do {
				v1 = 2.0 * distribution(generator) - 1.0; // -1.0 ~ 1.0 범위
				const double v2 = 2.0 * distribution(generator) - 1.0; // -1.0 ~ 1.0 범위
				s = v1 * v1 + v2 * v2;
			} while (s >= 1 || s == 0);

			s = sqrt((-2 * log(s)) / s);

			return v1 * s;
		}

		CSinCosTable::CSinCosTable(const double resDeg)
			: resolution(1.0 / resDeg)
		{
			Reset(resDeg);
		}

		bool CSinCosTable::Reset(const double resDeg)
		{
			bool bRet = false;

			if (resDeg >= 0.0)
			{
				const auto totalSize = 360.0 * resolution;
				const auto totalSizei = static_cast<uint64_t>(totalSize);

				tableSin.resize(totalSizei);
				tableCos.resize(totalSizei);

				for (uint64_t idx = 0; idx < totalSizei; idx++)
				{
					tableSin[idx] = std::sin(DegreeToRadian(static_cast<double>(idx) / resolution));
					tableCos[idx] = std::cos(DegreeToRadian(static_cast<double>(idx) / resolution));
				}

				bRet = true;
			}

			return bRet;
		}

		double CSinCosTable::sin(const double radian) const
		{
			return tableSin[static_cast<uint64_t>(fmod(RadianToDegree(radian), 360.0) * resolution)];
		}

		double CSinCosTable::cos(const double radian) const
		{
			return tableCos[static_cast<uint64_t>(fmod(RadianToDegree(radian), 360.0) * resolution)];
		}

		double CSinCosTable::sinDeg(double deg) const
		{
			return tableSin[static_cast<uint64_t>(fmod(deg, 360.0) * resolution)];
		}

		double CSinCosTable::cosDeg(double deg) const
		{
			return tableCos[static_cast<uint64_t>(fmod(deg, 360.0) * resolution)];
		}
	}
}
