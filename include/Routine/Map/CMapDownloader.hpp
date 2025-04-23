#pragma once

#include "../../Base/DevLibTypes.hpp"

namespace DevLib {
	namespace Routine
	{
		enum class MapSource
		{
			OpenStreetMap,
			NaverBasic,
			NaverSatellite
		};

		class CMapDownloader
		{
		public:
			CMapDownloader(const std::string& cachePath = "./mapCache", MapSource source = MapSource::OpenStreetMap);
			~CMapDownloader();

			CMapDownloader(const CMapDownloader&);

			void SetMapSource(MapSource source);
			void SetMapPath(const std::string& cachePath);

			bool Download(double latitude, double longitude,  uint32_t zoomLevel);
			bool DownloadROI(double start_latitude, double start_longitude, double end_latitude, double end_longitude, uint32_t zoomLevel);
			bool DownloadROI(double latitude, double longitude, double radius_meter, uint32_t zoomLevel);

		private:
			std::string m_cachePath;
			sharedHandle m_curl;
			MapSource m_source;
			std::vector<uint8_t> mapDownloadBuffer;
		};

	}
}