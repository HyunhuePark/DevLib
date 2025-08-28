#pragma once

#include <map>

#include "../Process/CWorker.hpp"
#include "../../Base/DevLibTypes.hpp"
#include "../../Base/DevLibCallback.hpp"
#include "../../Base/CLocker.hpp"
#include "../../Graphics/CImage.hpp"

namespace DevLib {
	namespace Routine
	{
		class COpenStreetMap
		{
		public:
			COpenStreetMap();
			virtual ~COpenStreetMap();

			COpenStreetMap(const COpenStreetMap&) = delete;
			COpenStreetMap& operator=(const COpenStreetMap&) = delete;

			bool CreateMapLoader(int32_t mapWidth, int32_t mapHeight, const std::string& cachePath = "mapCache");
			bool IsCreated() const;

			void MapResize(int32_t mapWidth, int32_t mapHeight);

			uint32_t GetWidth() const;
			uint32_t GetHeight() const;

			bool SetZoomLevel(uint32_t nLevel);
			bool ZoomIn();
			bool ZoomOut();
			int32_t GetZoomLevel() const;

			void SelectPosition(double longitude, double latitude);
			double GetLongitude() const;
			double GetLatitude() const;

			double MapPositionX(double longitude) const;
			double MapPositionY(double latitude) const;

			double MapPositionLongitude(double x) const;
			double MapPositionLatitude(double y) const;

			double MapPositionLength(double x, double y) const;

			Graphics::CImage GetMap(double longitude, double latitude);

			EnableCallback(DrawMap, DevLib::Graphics::CImage& map, DevLib::double_t longitude, DevLib::double_t latitude, DevLib::double_t resolution)


		private:
			std::string m_cachePath;
			int32_t		m_curZoomLevel;
			double		m_curResolution{};

			std::vector<uint8_t> mapDownloadBuffer;

			sharedHandle m_curl;

			using  MapCache = CLocker<std::map<int32_t, std::map<int32_t, std::pair<bool, Graphics::CImage >>>>; // Zoom, East, North, Map
			MapCache m_mapCache;

			Graphics::CImage m_mapBuffer;

			// XY Tile
			const int32_t TileWidth = 256;
			const int32_t TileHeight = 256;

			double_t m_curLongitude{};
			double_t m_curLatitude{};

			CWorker m_workerCaching;
			CWorker m_workerDownload;
			
			void OnCachingMap();
			void OnDownloadMap();
		};

	}
}
