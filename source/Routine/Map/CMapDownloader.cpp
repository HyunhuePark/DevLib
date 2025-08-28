#include "../../../include/Routine/Map/CMapDownloader.hpp"

#include "../../../include/IO/DevLibSocketUtility.hpp"
#include "../../../include/Utility/Geometric.hpp"
#include "../../../include/IO/CFile.hpp"
#include "../../../include/IO/DevLibFileUtility.hpp"
#include "../../../include/Graphics/CImage.hpp"

#include <curl/curl.h>
#if defined(_MSC_VER)
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "wldap32.lib")
#elif defined(__linux__) // Linux

#endif

namespace DevLib {
	namespace Routine
	{
		size_t WriteCallbackDownloader(void* contents, const size_t size, const size_t nMemBlock, std::vector<uint8_t>* stream)
		{
			const auto pData = static_cast<uint8_t*>(contents);
			for (size_t idx = 0; idx < size * nMemBlock; idx++)
			{
				stream->push_back(pData[idx]);
			}
			return size * nMemBlock;
		}

		CMapDownloader::CMapDownloader(const std::string& cachePath, MapSource source)
		{
			SetMapPath(cachePath);
			SetMapSource(source);

			curl_global_init(CURL_GLOBAL_DEFAULT);

			m_curl = std::make_shared<CURL*>(curl_easy_init());

			curl_easy_setopt(*m_curl, CURLOPT_WRITEFUNCTION, WriteCallbackDownloader);
			curl_easy_setopt(*m_curl, CURLOPT_WRITEDATA, &mapDownloadBuffer);
			curl_easy_setopt(*m_curl, CURLOPT_REFERER, "");
			curl_easy_setopt(*m_curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/125.0.0.0 Safari/537.36 Edg/125.0.0.0");  // 사용자 에이전트 설정
		}

		CMapDownloader::~CMapDownloader()
		{
			curl_easy_cleanup(*m_curl);
			curl_global_cleanup();
		}

		CMapDownloader::CMapDownloader(const CMapDownloader&)
			= default;

		void CMapDownloader::SetMapSource(MapSource source)
		{
			m_source = source;
		}

		void CMapDownloader::SetMapPath(const std::string& cachePath)
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
		}

		bool CMapDownloader::Download(const double latitude, const double longitude, uint32_t zoomLevel)
		{
			bool bRet = false;
			const auto tileX = Geometric::LongitudeToTileX(longitude, zoomLevel);
			const auto tileY = Geometric::LatitudeToTileY(latitude, zoomLevel);
			
			DevLib::Graphics::CImage img;
#if defined(_MSC_VER)
			const std::string mapPath = m_cachePath + std::to_string(zoomLevel) + "\\" + std::to_string(static_cast<uint32_t>(tileX)) + "\\";
#elif defined(__linux__) // Linux
			const std::string mapPath = m_cachePath + std::to_string(zoomLevel) + "/" + std::to_string(static_cast<uint32_t>(tileX)) + "/";
#endif

			std::string imgPath = mapPath + std::to_string(static_cast<uint32_t>(tileY));
			switch (m_source) {
			case MapSource::OpenStreetMap: 
			case MapSource::NaverBasic:
				imgPath += ".png"; break;
			case MapSource::NaverSatellite: 
				imgPath += ".jpg"; break;
			default:;
			}

			if (img.LoadImageFile(imgPath))
			{
			}
			else
			{
				std::string url;
				switch (m_source) {
				case MapSource::OpenStreetMap:
					url = "https://tile.openstreetmap.org/" + std::to_string(zoomLevel) + "/" + std::to_string(static_cast<uint32_t>(tileX)) + "/" + std::to_string(static_cast<uint32_t>(tileY)) + ".png";
					break;
				case MapSource::NaverBasic:
					url = "https://map.pstatic.net/nrb/styles/basic/1743356643/" + std::to_string(zoomLevel) + "/" + std::to_string(static_cast<uint32_t>(tileX)) + "/" + std::to_string(static_cast<uint32_t>(tileY)) + ".png?mt=bg.oh";
					break;
				case MapSource::NaverSatellite:
					url = "https://map.pstatic.net/nrs/api/v1/raster/satellite/" + std::to_string(zoomLevel) + "/" + std::to_string(static_cast<uint32_t>(tileX)) + "/" + std::to_string(static_cast<uint32_t>(tileY)) + ".jpg?version=6.03";
					break;
				default:;
				}

				curl_easy_setopt(*m_curl, CURLOPT_URL, url.c_str());

				mapDownloadBuffer.clear();
				const auto res = curl_easy_perform(*m_curl);

				if (res == CURLE_OK)
				{
					DevLib::IO::MakeDir(mapPath);

					DevLib::IO::CFile mapFile;
					if (!mapDownloadBuffer.empty())
					{
						if (mapFile.Open(imgPath, "wb"))
						{
							mapFile.Write(mapDownloadBuffer.data(), mapDownloadBuffer.size());
							mapFile.Close();

							bRet = true;
						}
					}
				}
			}


			return bRet;
		}

		bool CMapDownloader::DownloadROI(double start_latitude, double start_longitude, double end_latitude, double end_longitude, uint32_t zoomLevel)
		{
			bool bRet = false;

			auto sTileX = Geometric::LongitudeToTileX(start_longitude, zoomLevel);
			auto sTileY = Geometric::LatitudeToTileY(start_latitude, zoomLevel);

			auto eTileX = Geometric::LongitudeToTileX(end_longitude, zoomLevel);
			auto eTileY = Geometric::LatitudeToTileY(end_latitude, zoomLevel);

			if( eTileX <= sTileX)
			{
				const auto temp = sTileX;
				sTileX = eTileX;
				eTileX = temp;
			}

			if (eTileY <= sTileY)
			{
				const auto temp = sTileY;
				sTileY = eTileY;
				eTileY = temp;
			}

			for(auto idxX = static_cast<uint32_t>(sTileX); idxX <= eTileX; idxX++)
			{
				for(auto idxY = static_cast<uint32_t>(sTileY); idxY <= eTileY; idxY++)
				{
					Download(Geometric::TileYToLatitude(idxY, zoomLevel), Geometric::TileXToLongitude(idxX, zoomLevel), zoomLevel);
					bRet = true;
				}
			}

			return bRet;
		}

		bool CMapDownloader::DownloadROI(double latitude, double longitude, double radius_meter, uint32_t zoomLevel)
		{
			bool bRet = false;

			const auto tileX = Geometric::LongitudeToTileX(longitude, zoomLevel);
			const auto tileY = Geometric::LatitudeToTileY(latitude, zoomLevel);
			const auto pxScale = Geometric::ZoomLevelToRealMeter(zoomLevel, latitude);
			auto tileRadius = 0;

			switch (m_source) {
			case MapSource::OpenStreetMap:
			case MapSource::NaverBasic:
				tileRadius = static_cast<int32_t>(ceil((radius_meter / pxScale) / 256.0)); break;
			case MapSource::NaverSatellite:
				tileRadius = static_cast<int32_t>(ceil((radius_meter / pxScale) / 512.0)); break;
			default:;
			}

			for (auto idxX = static_cast<uint32_t>(tileX) - tileRadius; idxX <= tileX + tileRadius; idxX++)
			{
				for (auto idxY = static_cast<uint32_t>(tileY) - tileRadius; idxY <= tileY + tileRadius; idxY++)
				{
					Download(Geometric::TileYToLatitude(idxY, zoomLevel), Geometric::TileXToLongitude(idxX, zoomLevel), zoomLevel);
					bRet = true;
				}
			}


			return bRet;
		}
	}
}