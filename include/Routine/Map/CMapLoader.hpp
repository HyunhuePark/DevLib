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
		class CMapLoader
		{
		public:
			CMapLoader();
			virtual ~CMapLoader();

			CMapLoader(const CMapLoader&) = delete;
			CMapLoader& operator=(const CMapLoader&) = delete;

			bool CreateMapLoader(int32_t mapWidth, int32_t mapHeight, const std::string& path);

			uint32_t GetWidth() const;
			uint32_t GetHeight() const;

			bool SetLayer(const std::string& layer);
			std::string GetLayer();
			std::vector<std::string> GetLayerInfo() const;

			bool SetZoomLevel(uint32_t nLevel);
			bool ZoomIn();
			bool ZoomOut();
			int32_t GetZoomLevel() const;
			std::vector<int32_t> GetZoomLevelInfo();

			void SelectPosition(double longitude, double latitude);
			double GetLongitude() const;
			double GetLatitude() const;

			double MapPositionX(double longitude) const;
			double MapPositionY(double latitude) const;

			double MapPositionLongitude(double x) const;
			double MapPositionLatitude(double y) const;

			double MapPositionLength(double x, double y) const;

			Graphics::CImage GetMap(double latitude, double longitude);

			EnableCallback(DrawMap, DevLib::Graphics::CImage& map, DevLib::double_t longitude, DevLib::double_t latitude, DevLib::double_t resolution)


		private:
			std::string m_curLayer;
			int32_t		m_curZoomLevel;

			using  MapXYTiles = std::map<int32_t, std::map<int32_t, std::string>>;
			using  MapLevel = std::map<int32_t, MapXYTiles>;
			using  MapLayer = std::map< std::string, MapLevel>;
			inline MapLevel& LevelMap();
			inline MapXYTiles& XYTileMap();
			MapLayer m_mapLayer;

			using  MapCache = CLocker<std::map< int32_t, std::map<int32_t, std::map<int32_t, Graphics::CImage>>>>;

			Graphics::CImage m_mapBuffer;

			// XY Tile
			const int32_t TileWidth = 256;
			const int32_t TileHeight = 256;
			MapCache m_mapCache;

			double_t m_curLongitude{};
			double_t m_curLatitude{};

			CWorker m_workerCaching;
			void OnCachingMap(bool bAllZoomCaching);
		};

	}
}