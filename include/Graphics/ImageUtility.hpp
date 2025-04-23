#pragma once

#include <string>
#include "../Base/DevLibTypes.hpp"

namespace DevLib {
	namespace Graphics {

		class CImageObject;

		namespace ImageUtility 
		{
			void DrawBitmapToDC(void* Target, const CImageObject& image, bool bStetch = true);
			void DrawDCToBitmap(void* Source, CImageObject& image);

			bool SaveImageFile(CImageObject& image, const std::string& szFileName);
			bool LoadImageFile(CImageObject& image, const std::string& szFileName);

			bool LoadBitmapFile(CImageObject& image, const std::string& szFileName);
			bool SaveBitmapFile(CImageObject& image, const std::string& szFileName);

			bool LoadJPEGFile(CImageObject& image, const std::string& szFileName);
			bool LoadJPEGData(CImageObject& image, const uint8_t* jpegData, const int32_t size);
			bool SaveJPEGFile(CImageObject& image, const std::string& szFileName, int q = 80);

			bool LoadPNGFile(CImageObject& image, const std::string& szFileName);
			bool LoadPNGData(CImageObject& image, const uint8_t* pngData, const int32_t size);
			bool SavePNGFile(CImageObject& image, const std::string& szFileName);

			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////

		}
	}
}
