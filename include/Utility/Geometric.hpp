#pragma once

#include "../Base/DevLibTypes.hpp"
#include "Mathematics.hpp"

namespace DevLib {
	namespace Geometric {

		void LatLonToUTM(double latitude, double longitude, double &east, double &north, int32_t &zone, bool &bNorth);
		void ToUTMLatLon(double east, double north, double &latitude, double &longitude, int32_t zone = 52, bool bNorth = true);

		// TileMap
		inline double LongitudeToTileX(const double lon, const uint32_t z)
		{
			return (lon + 180.0) / 360.0 * (1 << z);
		}

		inline double LatitudeToTileY(const double lat, const uint32_t z)
		{
			const double latRad = lat * Mathematics::PI / 180.0;
			return (1.0 - asinh(tan(latRad)) / Mathematics::PI) / 2.0 * (1 << z);
		}

		inline double TileXToLongitude(const double x, const uint32_t z)
		{
			return x / static_cast<double>(1 << z) * 360.0 - 180;
		}

		inline double TileYToLatitude(const double y, const uint32_t z)
		{
			const double n = Mathematics::PI - 2.0 * Mathematics::PI * y / static_cast<double>(1 << z);
			return 180.0 / Mathematics::PI * atan(0.5 * (exp(n) - exp(-n)));
		}

		inline double ZoomLevelToRealMeter(const uint32_t z, double latitude, const uint32_t mapSize = 256)
		{
			constexpr double earthCircumference = 40075016.686;
			const double latitudeRadians = latitude * Mathematics::PI / 180.0;
			const double totalPixels = mapSize * std::pow(2, z);

			return (earthCircumference / totalPixels) * std::cos(latitudeRadians);
		}

	}
}
