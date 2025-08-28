#include "../../../include/Routine/Map/CMapLoader.hpp"


#include <cmath>
#include <sstream>

#include "../../../include/IO/DevLibFileUtility.hpp"
#include "../../../include/Utility/String.hpp"
#include "../../../include/Utility/Geometric.hpp"

namespace DevLib {
	namespace Routine
	{
		CMapLoader::CMapLoader()
			: m_curLayer({}), m_curZoomLevel(0)
		{
			RegisterCallbackDrawMap(&CMapLoader::OnDrawMap, this);
		}

		CMapLoader::~CMapLoader()
		= default;

		bool CMapLoader::CreateMapLoader(int32_t mapWidth, int32_t mapHeight, const std::string& path)
		{
			bool bRet = false;

			if (m_mapBuffer.Create(mapWidth, mapHeight, Graphics::ImageType::IMG_BGRA))
			{
				int8_t delimiter;

#ifdef _MSC_VER
				delimiter = '\\';
#else // Linux
				delimiter = '/';
#endif
				IO::FileInfo fi;
				IO::ReadFolder(fi, path, true);

				const std::vector<std::string> vRootPath = DevLib::Split(path, delimiter);

				for (auto& mapPath : fi)
				{
					if (mapPath.second == false)
					{
						std::vector<std::string> vPath = DevLib::Split(mapPath.first, delimiter);

						if (vPath.size() - vRootPath.size() == 4)
						{
							const std::string strLayer = vPath[vRootPath.size() + 0];
							const std::string strLevel = vPath[vRootPath.size() + 1];
							const std::string strGridX = vPath[vRootPath.size() + 2];
							const std::string strGridY = vPath[vRootPath.size() + 3].substr(0, vPath[vRootPath.size() + 3].rfind('.'));
							m_mapLayer[strLayer][std::stoi(strLevel)][std::stoi(strGridX)][std::stoi(strGridY)] = mapPath.first;
						}
					}
				}

				// Set Default LevelMap
				if (!m_mapLayer.empty())
				{
					m_curLayer = m_mapLayer.begin()->first;

					// Set Default XYTileMap : zoom center
					const auto level = GetZoomLevelInfo();
					SetZoomLevel(level[level.size() / 2]);

					// Worker Caching & UnCaching
					m_workerCaching.CreateAsync("MapCaching", &CMapLoader::OnCachingMap, this, true);
					m_workerCaching.Work();

					bRet = true;
				}
			}

			return bRet;
		}

		uint32_t CMapLoader::GetWidth() const
		{
			return m_mapBuffer.GetWidth();
		}

		uint32_t CMapLoader::GetHeight() const
		{
			return m_mapBuffer.GetHeight();
		}

		bool CMapLoader::SetLayer(const std::string& layer)
		{
			bool bRet = false;

			const auto layerInfo = GetLayerInfo();

			for (auto& value : layerInfo)
			{
				if (layer == value)
				{
					m_curLayer = layer;

					// Clear Old Layer
					m_mapCache.Lock();
					m_mapCache.get().clear();
					m_mapCache.UnLock();

					// Load New Layer
					m_workerCaching.Work();

					bRet = true;
					break;
				}
			}

			return bRet;
		}

		std::string CMapLoader::GetLayer()
		{
			return  m_curLayer;
		}

		std::vector<std::string> CMapLoader::GetLayerInfo() const
		{
			std::vector<std::string> layerInfo;
			for (auto& layer : m_mapLayer)
			{
				layerInfo.push_back(layer.first);
			}

			return layerInfo;
		}

		bool CMapLoader::SetZoomLevel(uint32_t nLevel)
		{
			bool bRet = false;

			auto vZoom = GetZoomLevelInfo();
			const auto itr = std::find(vZoom.begin(), vZoom.end(), nLevel);

			if (itr != vZoom.end())
			{
				m_curZoomLevel = nLevel;
				bRet = true;
			}

			return bRet;
		}

		bool CMapLoader::ZoomIn()
		{
			bool bRet = false;
			auto vZoom = GetZoomLevelInfo();
			auto itr = std::find(vZoom.begin(), vZoom.end(), m_curZoomLevel);

			++itr;
			if (itr != vZoom.end())
			{
				m_curZoomLevel = *itr;
				bRet = false;
			}

			m_workerCaching.Work();

			return bRet;
		}

		bool CMapLoader::ZoomOut()
		{
			bool bRet = false;
			auto vZoom = GetZoomLevelInfo();
			auto itr = std::find(vZoom.begin(), vZoom.end(), m_curZoomLevel);

			if (itr != vZoom.begin())
			{
				--itr;
				m_curZoomLevel = *itr;
				bRet = false;
			}

			m_workerCaching.Work();

			return bRet;
		}

		int32_t CMapLoader::GetZoomLevel() const
		{
			return m_curZoomLevel;
		}

		std::vector<int32_t> CMapLoader::GetZoomLevelInfo()
		{
			std::vector<int32_t> zoomLevelInfo;
			for (const auto& zoom : LevelMap())
			{
				zoomLevelInfo.push_back(zoom.first);
			}

			return zoomLevelInfo;
		}

		void CMapLoader::SelectPosition(double longitude, double latitude)
		{
			m_curLongitude = longitude;
			m_curLatitude = latitude;
		}

		double CMapLoader::GetLongitude() const
		{
			return m_curLongitude;
		}

		double CMapLoader::GetLatitude() const
		{
			return m_curLatitude;
		}

		double CMapLoader::MapPositionX(double longitude) const
		{
			const double offsetX = Geometric::LongitudeToTileX(longitude, GetZoomLevel()) - Geometric::LongitudeToTileX(GetLongitude(), GetZoomLevel());

			return m_mapBuffer.GetWidth() * 0.5 + offsetX * TileWidth;
		}

		double CMapLoader::MapPositionY(double latitude) const
		{
			const double offsetY = Geometric::LatitudeToTileY(latitude, GetZoomLevel()) - Geometric::LatitudeToTileY(GetLatitude(), GetZoomLevel());

			return m_mapBuffer.GetHeight() * 0.5 + offsetY * TileHeight;
		}

		double CMapLoader::MapPositionLongitude(double x) const
		{
			const double curX = (x - m_mapBuffer.GetWidth() * 0.5) / TileWidth + Geometric::LongitudeToTileX(GetLongitude(), GetZoomLevel());
			return Geometric::TileXToLongitude(curX, GetZoomLevel());
		}

		double CMapLoader::MapPositionLatitude(double y) const
		{
			const double curY = Geometric::LatitudeToTileY(GetLatitude(), GetZoomLevel()) - (m_mapBuffer.GetHeight() * 0.5 - y) / TileHeight;
			return Geometric::TileYToLatitude(curY, GetZoomLevel());
		}

		double CMapLoader::MapPositionLength(double x, double y) const
		{
			const auto aLon = MapPositionLongitude(x);
			const auto aLat = MapPositionLatitude(y);
			const auto bLon = GetLongitude();
			const auto bLat = GetLatitude();
			double aEast = 0, aNorth = 0;
			double bEast = 0, bNorth = 0;
			int zone;
			bool a_bNorth{};
			bool b_bNorth{};

			Geometric::LatLonToUTM(aLat, aLon, aEast, aNorth, zone, a_bNorth);
			Geometric::LatLonToUTM(bLat, bLon, bEast, bNorth, zone, b_bNorth);

			return sqrt(pow(aEast - bEast, 2) + pow(aNorth - bNorth, 2));
		}


		Graphics::CImage CMapLoader::GetMap(double latitude, double longitude)
		{
			// Update Position
			SelectPosition(longitude, latitude);

			OnCachingMap(false);

			const double_t posX = Geometric::LongitudeToTileX(GetLongitude(), GetZoomLevel());
			const double_t posY = Geometric::LatitudeToTileY(GetLatitude(), GetZoomLevel());

			const auto tileX = static_cast<uint32_t>(floor(posX));
			const auto tileY = static_cast<uint32_t>(floor(posY));

			const double_t posRateX = posX - tileX;
			const double_t posRateY = posY - tileY;

			const double_t offsetX = static_cast<double_t>(TileWidth) * 0.5 - TileWidth * posRateX;
			const double_t offsetY = static_cast<double_t>(TileWidth) * 0.5 - TileWidth * posRateY;

			// Calc Cache Size
			const auto marginLeft = static_cast<int32_t>(ceil((static_cast<float_t>(m_mapBuffer.GetWidth() - TileWidth) / 2.0f + offsetX) / TileWidth));
			const auto marginRight = static_cast<int32_t>(ceil((static_cast<float_t>(m_mapBuffer.GetWidth() - TileWidth) / 2.0f - offsetX) / TileWidth));
			const auto marginTop = static_cast<int32_t>(ceil((static_cast<float_t>(m_mapBuffer.GetHeight() - TileHeight) / 2.0f + offsetY) / TileHeight));
			const auto marginBottom = static_cast<int32_t>(ceil((static_cast<float_t>(m_mapBuffer.GetHeight() - TileHeight) / 2.0f - offsetY) / TileHeight));

			const auto cx = static_cast<int32_t>(static_cast<double_t>(m_mapBuffer.GetWidth()) * 0.5 - static_cast<double_t>(TileWidth) * 0.5);
			const auto cy = static_cast<int32_t>(static_cast<double_t>(m_mapBuffer.GetHeight()) * 0.5 - static_cast<double_t>(TileHeight) * 0.5);

			// Draw Map
			m_mapCache.LockRead();

			auto& level = m_mapCache.get()[m_curZoomLevel];
			for (int32_t idxX = -marginLeft; idxX <= marginRight; idxX++)
			{
				for (int32_t idxY = -marginTop; idxY <= marginBottom; idxY++)
				{
					m_mapBuffer.DrawImage(cx + TileWidth * idxX + static_cast<int32_t>(offsetX), cy + TileHeight * idxY + static_cast<int32_t>(offsetY), level[tileX + idxX][tileY + idxY]);
				}
			}
			m_mapCache.UnLockRead();

			// Drawer Callback
			m_callbackDrawMap(std::ref(m_mapBuffer), GetLongitude(), GetLatitude(), Geometric::ZoomLevelToRealMeter(m_curZoomLevel, GetLatitude(), TileHeight));

			// Work UnCaching
			m_workerCaching.Work();

			return m_mapBuffer;
		}

		CMapLoader::MapLevel& CMapLoader::LevelMap()
		{
			return m_mapLayer[m_curLayer];
		}

		CMapLoader::MapXYTiles& CMapLoader::XYTileMap()
		{
			return LevelMap()[m_curZoomLevel];
		}

		void CMapLoader::OnCachingMap(bool bAllZoomCaching)
		{
			int32_t marginWeight = 1;
			// Check Cache
			std::vector listZoom{ m_curZoomLevel };

			if (bAllZoomCaching)
			{
				marginWeight = 3;
				listZoom = GetZoomLevelInfo();
			}

			for (const auto nZoom : listZoom)
			{
				const double_t posX = Geometric::LongitudeToTileX(GetLongitude(), nZoom);
				const double_t posY = Geometric::LatitudeToTileY(GetLatitude(), nZoom);

				const auto tileX = static_cast<int32_t>(floor(posX));
				const auto tileY = static_cast<int32_t>(floor(posY));

				const double_t posRateX = posX - tileX;
				const double_t posRateY = posY - tileY;

				const double_t offsetX = TileWidth * 0.5f - TileWidth * posRateX;
				const double_t offsetY = TileWidth * 0.5f - TileWidth * posRateY;

				// Calc Cache Size : View x 3
				const auto marginLeft = static_cast<int32_t>(ceil((static_cast<float_t>(m_mapBuffer.GetWidth() - TileWidth) / 2.0f + offsetX) / TileWidth)) * marginWeight;
				const auto marginRight = static_cast<int32_t>(ceil((static_cast<float_t>(m_mapBuffer.GetWidth() - TileWidth) / 2.0f - offsetX) / TileWidth)) * marginWeight;
				const auto marginTop = static_cast<int32_t>(ceil((static_cast<float_t>(m_mapBuffer.GetHeight() - TileHeight) / 2.0f + offsetY) / TileHeight)) * marginWeight;
				const auto marginBottom = static_cast<int32_t>(ceil((static_cast<float_t>(m_mapBuffer.GetHeight() - TileHeight) / 2.0f - offsetY) / TileHeight)) * marginWeight;

				auto& TileMap = LevelMap()[nZoom];

				for (int idxX = -marginLeft; idxX <= marginRight; idxX++)
				{
					for (int idxY = -marginTop; idxY <= marginBottom; idxY++)
					{
						// Check Y Map
						if (m_mapCache.get()[nZoom][tileX + idxX][tileY + idxY].empty())
						{
							m_mapCache.Lock();
							if (!TileMap[tileX + idxX][tileY + idxY].empty())
							{
								m_mapCache.get()[nZoom][tileX + idxX][tileY + idxY].LoadImageFile(TileMap[tileX + idxX][tileY + idxY]);
							}
							else
							{
								m_mapCache.get()[nZoom][tileX + idxX][tileY + idxY].Create(TileWidth, TileHeight, Graphics::ImageType::IMG_BGRA);
								m_mapCache.get()[nZoom][tileX + idxX][tileY + idxY].DrawString(100, 100, "Tile File\n Not Found.", Graphics::White);
							}
							m_mapCache.UnLock();
						}
					}
				}
			}

			// Remove cache
			if (bAllZoomCaching)
			{
				for (const auto nZoom : GetZoomLevelInfo())
				{
					const double_t posX = Geometric::LongitudeToTileX(GetLongitude(), nZoom);
					const double_t posY = Geometric::LatitudeToTileY(GetLatitude(), nZoom);

					const auto tileX = static_cast<int32_t>(floor(posX));
					const auto tileY = static_cast<int32_t>(floor(posY));

					const double_t posRateX = posX - tileX;
					const double_t posRateY = posY - tileY;

					const double_t offsetX = TileWidth * 0.5f - TileWidth * posRateX;
					const double_t offsetY = TileWidth * 0.5f - TileWidth * posRateY;

					// Calc Cache Size : View x 3
					const auto marginLeft = static_cast<int32_t>(ceil((static_cast<float_t>(m_mapBuffer.GetWidth() - TileWidth) / 2.0f + offsetX) / TileWidth)) * marginWeight;
					const auto marginRight = static_cast<int32_t>(ceil((static_cast<float_t>(m_mapBuffer.GetWidth() - TileWidth) / 2.0f - offsetX) / TileWidth)) * marginWeight;
					const auto marginTop = static_cast<int32_t>(ceil((static_cast<float_t>(m_mapBuffer.GetHeight() - TileHeight) / 2.0f + offsetY) / TileHeight)) * marginWeight;
					const auto marginBottom = static_cast<int32_t>(ceil((static_cast<float_t>(m_mapBuffer.GetHeight() - TileHeight) / 2.0f - offsetY) / TileHeight)) * marginWeight;

					constexpr int32_t marginRemove = 2;

					for (auto itrX = m_mapCache.get()[nZoom].begin(); itrX != m_mapCache.get()[nZoom].end(); )
					{

						if ((itrX->first < tileX - (marginLeft + marginRemove)) || (itrX->first > tileX + (marginRight + marginRemove)))
						{
							// Remove Y Image 
							m_mapCache.Lock();
							itrX = m_mapCache.get()[nZoom].erase(itrX);
							m_mapCache.UnLock();
						}
						else
						{
							for (auto itrY = itrX->second.begin(); itrY != itrX->second.end(); )
							{
								// Remove X Image 
								if ((itrY->first < tileY - (marginTop + marginRemove)) || (itrY->first > tileY + (marginBottom + marginRemove)))
								{
									m_mapCache.Lock();
									itrY = itrX->second.erase(itrY);
									m_mapCache.UnLock();
								}
								else
								{
									++itrY;
								}
							}
							++itrX;
						}
					}
				}
			}
		}
	}
}