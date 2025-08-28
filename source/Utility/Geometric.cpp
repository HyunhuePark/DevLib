#include "../../include/Utility/Geometric.hpp"
#include <GeographicLib/UTMUPS.hpp>

namespace DevLib {
	namespace Geometric {

		void LatLonToUTM(double latitude, double longitude, double& east, double& north, int32_t& zone, bool& bNorth)
		{
			GeographicLib::UTMUPS::Forward(latitude, longitude, zone, bNorth, east, north); 
		}

		void ToUTMLatLon(double east, double north, double& latitude, double& longitude, int32_t zone , bool bNorth)
		{
			GeographicLib::UTMUPS::Reverse(zone, bNorth, east, north, latitude, longitude);
		}
	}
}
