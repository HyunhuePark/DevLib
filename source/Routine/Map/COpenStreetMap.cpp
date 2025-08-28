#include "../../../include/Routine/Map/COpenStreetMap.hpp"


#if defined(_MSC_VER)
#define M_PI           3.14159265358979323846  /* pi */
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "wldap32.lib")
#elif defined(__linux__) // Linux

#endif

#include "../../../include/IO/DevLibSocketUtility.hpp"
#include <curl/curl.h>

#include <cmath>

#include "../../../include/IO/CFile.hpp"
#include "../../../include/IO/DevLibFileUtility.hpp"
#include "../../../include/Utility/String.hpp"

#include "../../../include/Base/CElapseTimer.hpp"

namespace DevLib {
	namespace Routine
	{
		static DevLib::Graphics::CImage g_ImageNull;

		inline double long2tilex(const double lon, const int z)
		{
			return (lon + 180.0) / 360.0 * (1 << z);
		}

		inline double lat2tiley(const double lat, const int z)
		{
			const double latrad = lat * M_PI / 180.0;
			return (1.0 - asinh(tan(latrad)) / M_PI) / 2.0 * (1 << z);
		}

		inline double tilex2long(const double x, const int z)
		{
			return x / static_cast<double>(1 << z) * 360.0 - 180;
		}

		inline double tiley2lat(const double y, const int z)
		{
			const double n = M_PI - 2.0 * M_PI * y / static_cast<double>(1 << z);
			return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
		}


		using MapPath = std::tuple<std::string, std::string, std::string>;
		inline std::vector<MapPath> spiralSort(const std::vector<std::vector<MapPath>>& a)
		{
			std::vector<MapPath> aligned;
			const size_t m = a.size();
			const size_t n = a[0].size();
			size_t x = (m - 1) / 2;
			size_t y = (n - 1) / 2;
			size_t layer = 0; // 현재 나선의 레이어 (중앙에서 시작)
			size_t steps = 1; // 현재 레이어에서 이동해야 할 스텝 수

			// 중앙 원소 출력
			aligned.push_back(a[x][y]);

			while (layer < ((((m) > (n)) ? (m) : (n)) + 1) / 2)
			{
				// 오른쪽으로 이동
				size_t dx = 0; size_t dy = 1;
				for (size_t i = 0; i < steps; ++i)
				{
					x += dx; y += dy;
					if ( x < m && y < n)
					{
						aligned.push_back(a[x][y]);
					}
				}

				// 아래로 이동
				dx = 1; dy = 0;
				for (size_t i = 0; i < steps; ++i)
				{
					x += dx; y += dy;
					if (x < m &&  y < n)
					{
						aligned.push_back(a[x][y]);
					}
				}

				// 스텝 수 증가
				++steps;

				// 왼쪽으로 이동
				dx = 0; dy = -1;
				for (size_t i = 0; i < steps; ++i)
				{
					x += dx; y += dy;
					if ( x < m && y < n)
					{
						aligned.push_back(a[x][y]);
					}
				}

				// 위로 이동
				dx = -1; dy = 0;
				for (size_t i = 0; i < steps; ++i)
				{
					x += dx; y += dy;
					if ( x < m && y < n)
					{
						aligned.push_back(a[x][y]);
					}
				}

				// 다음 레이어로 이동
				++layer;
				++steps;
			}

			return aligned;
		}


		size_t WriteCallback(void* contents, const size_t size, const size_t nMemBlock, std::vector<uint8_t>* stream)
		{
			const auto pData = static_cast<uint8_t*>(contents);
			for (size_t idx = 0; idx < size * nMemBlock; idx++)
			{
				stream->push_back(pData[idx]);
			}
			return size * nMemBlock;
		}

		COpenStreetMap::COpenStreetMap()
			: m_curZoomLevel(0)
		{
			RegisterCallbackDrawMap(&COpenStreetMap::OnDrawMap, this);

			g_ImageNull.Create(TileWidth, TileHeight, Graphics::ImageType::IMG_BGRA);
			g_ImageNull.DrawString(100, 100, "Tile File\n Not Found.", Graphics::White);

			curl_global_init(CURL_GLOBAL_DEFAULT);

			m_curl = std::make_shared<CURL*>(curl_easy_init());

			const auto curl = reinterpret_cast<CURL*>(*m_curl);

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mapDownloadBuffer);
			curl_easy_setopt(curl, CURLOPT_REFERER, "");
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36 Edg/125.0.0.0");  // 사용자 에이전트 설정
		}

		COpenStreetMap::~COpenStreetMap()
		{
			const auto curl = reinterpret_cast<CURL*>(*m_curl);
			curl_easy_cleanup(curl);
			curl_global_cleanup();
		}

		bool COpenStreetMap::CreateMapLoader(const int32_t mapWidth, const int32_t mapHeight, const std::string& cachePath)
		{
			bool bRet = false;

			if (m_mapBuffer.Create(mapWidth, mapHeight, Graphics::ImageType::IMG_BGRA))
			{
				m_cachePath = cachePath;

#if defined(_MSC_VER)
				if (m_cachePath[m_cachePath.size() - 1] != '\\')
				{
					m_cachePath.append("\\");
				}
#elif defined(__linux__) // Linux
				if (m_cachePath[m_cachePath.size() - 1] != '/')
				{
					m_cachePath.append("/");
				}
#endif

				DevLib::IO::MakeDir(m_cachePath);

				// Set Default XYTileMap : zoom center
				SetZoomLevel(13);

				// Worker Caching & UnCaching
				m_workerCaching.CreateAsync("MapCaching", &COpenStreetMap::OnCachingMap, this);
				m_workerDownload.CreateAsync("MapDownload", &COpenStreetMap::OnDownloadMap, this);
				m_workerDownload.Work();
				m_workerCaching.Work();

				bRet = true;
			}

			return bRet;
		}

		bool COpenStreetMap::IsCreated() const
		{
			return !m_mapBuffer.IsNull();
		}

		void COpenStreetMap::MapResize(const int32_t mapWidth, const int32_t mapHeight)
		{
			m_mapCache.Lock();
			m_mapBuffer.Destroy();
			m_mapBuffer.Create(mapWidth, mapHeight, Graphics::ImageType::IMG_BGRA);
			m_mapCache.UnLock();
		}

		uint32_t COpenStreetMap::GetWidth() const
		{
			return m_mapBuffer.GetWidth();
		}

		uint32_t COpenStreetMap::GetHeight() const
		{
			return m_mapBuffer.GetHeight();
		}

		bool COpenStreetMap::SetZoomLevel(const uint32_t nLevel)
		{
			bool bRet = false;

			if (nLevel > 0 && nLevel < 20)
			{
				m_curZoomLevel = nLevel;
				bRet = true;
			}

			m_workerDownload.Work();
			m_workerCaching.Work();


			return bRet;
		}

		bool COpenStreetMap::ZoomIn()
		{
			auto curZoom = GetZoomLevel();
			curZoom++;

			return SetZoomLevel(curZoom);
		}

		bool COpenStreetMap::ZoomOut()
		{
			auto curZoom = GetZoomLevel();
			curZoom--;

			return SetZoomLevel(curZoom);
		}

		int32_t COpenStreetMap::GetZoomLevel() const
		{
			return m_curZoomLevel;
		}

		void COpenStreetMap::SelectPosition(const double longitude, const double latitude)
		{
			m_curLongitude = longitude;
			m_curLatitude = latitude;
		}

		double COpenStreetMap::GetLongitude() const
		{
			return m_curLongitude;
		}

		double COpenStreetMap::GetLatitude() const
		{
			return m_curLatitude;
		}

		double COpenStreetMap::MapPositionX(const double longitude) const
		{
			const double offsetX = long2tilex(longitude, GetZoomLevel()) - long2tilex(GetLongitude(), GetZoomLevel());

			return m_mapBuffer.GetWidth() * 0.5 + offsetX * TileWidth;
		}

		double COpenStreetMap::MapPositionY(const double latitude) const
		{
			const double offsetY = lat2tiley(latitude, GetZoomLevel()) - lat2tiley(GetLatitude(), GetZoomLevel());

			return m_mapBuffer.GetHeight() * 0.5 + offsetY * TileHeight;
		}

		double COpenStreetMap::MapPositionLongitude(const double x) const
		{
			const double curX = (x - m_mapBuffer.GetWidth() * 0.5) / TileWidth + long2tilex(GetLongitude(), GetZoomLevel());
			return tilex2long(curX, GetZoomLevel());
		}

		double COpenStreetMap::MapPositionLatitude(const double y) const
		{
			const double curY = lat2tiley(GetLatitude(), GetZoomLevel()) - (m_mapBuffer.GetHeight() * 0.5 - y) / TileHeight;
			return tiley2lat(curY, GetZoomLevel());
		}

		double COpenStreetMap::MapPositionLength(const double x, const double y) const
		{
			return m_curResolution * sqrt(pow(x, 2) + pow(y, 2));
		}

		Graphics::CImage COpenStreetMap::GetMap(const double longitude, const double latitude)
		{
			// Update Resolution
			m_curResolution = 156543.03 * cos(latitude * 0.5) / pow(2, static_cast<double>(m_curZoomLevel));

			// Update Position
			SelectPosition(longitude, latitude);

			const double_t posX = long2tilex(GetLongitude(), GetZoomLevel());
			const double_t posY = lat2tiley(GetLatitude(), GetZoomLevel());

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
			auto& level = m_mapCache.get();

			for (int32_t idxX = -marginLeft; idxX <= marginRight; idxX++)
			{
				for (int32_t idxY = -marginTop; idxY <= marginBottom; idxY++)
				{
					m_mapCache.Lock();
					if (level[tileX + idxX][tileY + idxY].first)
					{
						m_mapBuffer.DrawImage(cx + TileWidth * idxX + static_cast<int32_t>(offsetX), cy + TileHeight * idxY + static_cast<int32_t>(offsetY), level[tileX + idxX][tileY + idxY].second);
					}
					else
					{
						m_mapBuffer.DrawImage(cx + TileWidth * idxX + static_cast<int32_t>(offsetX), cy + TileHeight * idxY + static_cast<int32_t>(offsetY),
						                      g_ImageNull);
					}
					m_mapCache.UnLock();
				}
			}

			// Drawer Callback
			m_callbackDrawMap(std::ref(m_mapBuffer), GetLongitude(), GetLatitude(), m_curResolution);

			// Work UnCaching
			m_workerDownload.Work();
			m_workerCaching.Work();

			return m_mapBuffer;
		}

		void COpenStreetMap::OnCachingMap()
		{
			constexpr int32_t marginWeight = 3;

			const auto nZoom = GetZoomLevel();

			const double_t posX = long2tilex(GetLongitude(), nZoom);
			const double_t posY = lat2tiley(GetLatitude(), nZoom);

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


			for (int idxX = -marginLeft; idxX <= marginRight; idxX++)
			{
				for (int idxY = -marginTop; idxY <= marginBottom; idxY++)
				{
					// Check Y Map
					if (m_mapCache.get()[tileX + idxX][tileY + idxY].second.empty())
					{
						std::string mapPath = m_cachePath + std::to_string(nZoom) + "\\" + std::to_string(tileX + idxX) + "\\" + std::to_string(tileY + idxY) + ".png";

						m_mapCache.Lock();
						if( m_mapCache.get()[tileX + idxX][tileY + idxY].second.LoadImageFile(mapPath) )
						{
							m_mapCache.get()[tileX + idxX][tileY + idxY].first = true;
						}
						else
						{
							DevLib::IO::CFile testFile;
							if( testFile.Open(mapPath, "rb") )
							{
								// Crash File : Remove
								testFile.Close();
								DevLib::IO::RemoveFile(mapPath);
							}
							m_mapCache.get()[tileX + idxX][tileY + idxY].first = false;
						}
						m_mapCache.UnLock();
					}
				}
			}

			// Remove Out Map
			auto itrX = m_mapCache->begin();
			while (itrX != m_mapCache->end())
			{
				if (itrX->first < tileX - marginLeft || itrX->first > tileX + marginRight)
				{
					m_mapCache.Lock();
					itrX = m_mapCache->erase(itrX);
					m_mapCache.UnLock();
				}
				else
				{
					auto itrY = itrX->second.begin();
					while (itrY != itrX->second.end())
					{
						if (itrY->first < tileY - marginTop || itrY->first > tileY + marginBottom)
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

		void COpenStreetMap::OnDownloadMap()
		{
			constexpr int32_t marginWeight = 3;

			const auto nZoom = GetZoomLevel();

			const double_t posX = long2tilex(GetLongitude(), nZoom);
			const double_t posY = lat2tiley(GetLatitude(), nZoom);

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

			std::vector<std::vector<MapPath>> downloadList;

			for (int idxX = -marginLeft; idxX <= marginRight; idxX++)
			{
				std::vector<MapPath> listY;
				for (int idxY = -marginTop; idxY <= marginBottom; idxY++)
				{
					std::string url = "https://tile.openstreetmap.org/" + std::to_string(nZoom) + "/" + std::to_string(tileX + idxX) + "/" + std::to_string(tileY + idxY) + ".png";

#if defined(_MSC_VER)
					std::string mapPath = m_cachePath + std::to_string(nZoom) + "\\" + std::to_string(tileX + idxX) + "\\" + std::to_string(tileY + idxY) + ".png";
					listY.emplace_back(m_cachePath + std::to_string(nZoom) + "\\" + std::to_string(tileX + idxX), mapPath, url);
#elif defined(__linux__) // Linux
					std::string mapPath = m_cachePath + std::to_string(nZoom) + "/" + std::to_string(tileX + idxX) + "/" + std::to_string(tileY + idxY) + ".png";
					listY.emplace_back(m_cachePath + std::to_string(nZoom) + "/" + std::to_string(tileX + idxX), mapPath, url);
#endif
				}

				downloadList.push_back(listY);
			}

			auto alignedList = spiralSort(downloadList);

			for (auto& mapPath : alignedList)
			{
				IO::CFile testFile;
				if (!testFile.Open(std::get<1>(mapPath), "rb"))
				{
					const auto curl = reinterpret_cast<CURL*>(*m_curl);

					DevLib::IO::MakeDir(std::get<0>(mapPath));
					curl_easy_setopt(curl, CURLOPT_URL, std::get<2>(mapPath).c_str());


					mapDownloadBuffer.clear();
					DevLib::CElapseTimer t;
					const auto res = curl_easy_perform(curl);

					if (res == CURLE_OK)
					{
						m_mapCache.Lock();

						DevLib::IO::CFile mapFile;
						if (mapFile.Open(std::get<1>(mapPath), "wb"))
						{
							mapFile.Write(mapDownloadBuffer.data(), mapDownloadBuffer.size());
							mapFile.Close();
						}

						m_mapCache.UnLock();
					}
				}
				testFile.Close();
			}
		}
	}
}