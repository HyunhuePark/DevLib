#pragma once

#include "../Base/CThread.hpp"
#include "../Base/CLocker.hpp"
#include "../Graphics/CImage.hpp"
#include "../Visualization/CImageViewer.hpp"
#include "../MultiMedia/CVideoDecoder.hpp"

#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
#include "CVideoDecoder.hpp"
#endif

namespace DevLib { namespace MultiMedia {

	enum class CAMERA_FORMAT
	{
		FORMAT_NV12,
		FORMAT_MJPEG,
		FORMAT_BGR,
		FORMAT_BGRA
	};

class CUSBCamera
{
public:
	CUSBCamera(DevLib::Graphics::ImageType outputType = DevLib::Graphics::ImageType::IMG_BGR, DevLib::MultiMedia::ConvertType cvtType = DevLib::MultiMedia::ConvertType::CVT_BILINEAR);
	~CUSBCamera();

	static uint32_t GetCameraCount();

	static string_t GetCameraName(uint32_t cameraNum);
	static string_t GetCameraPath(uint32_t cameraNum);
	static string_t GetCameraPhyAddress(uint32_t cameraNum);

	bool SelectCamera(uint32_t cameraNum = 0);
	bool SelectCamera(uint32_t width, uint32_t height, uint32_t cameraNum = 0);

	void CloseCamera();

	uint32_t GetSelectedCamera() const;

	std::vector<std::string> GetSupportFormat() const;

	bool StartCamera();
	void StopCamera();

	bool GetImage(DevLib::Graphics::CImage& img);
	bool GetImage(uint8_t* img);
	
	uint32_t GetWidth() const { return cameraWidth; }
	uint32_t GetHeight() const { return cameraHeight; }
	float GetFrameRate() const { return cameraFrameRate;  }

	bool SetCameraSize(uint32_t width, uint32_t height);
	bool SetFrameRate(float_t fps);
	bool SetPixelFormat(CAMERA_FORMAT format);

	bool IsSelectedCamera() const;

	EnableCallback(CameraImage, DevLib::Graphics::CImage& img)

private:
	int32_t						m_idxCamera;
	DevLib::Graphics::CImage	m_inputImage;
	DevLib::CLocker <DevLib::Graphics::CImage>	m_outImage;
	CThread						m_watchDogCamera;
	uint32_t					cameraWidth;
	uint32_t					cameraHeight;
	float						cameraFrameRate;
	DevLib::Graphics::ImageType cameraOutType;

	void WatchDogCamera();


#if defined(_MSC_VER)
	GUID						pixelFormat;
	handle_t					hCamera;
	handle_t					hCameraReader;
#elif defined(__linux__) // linux

private:
	int32_t			m_hCamera;
	CVideoDecoder	m_decoder;
	std::vector<std::pair<void *, size_t>> m_videoBuffer;
#endif
};

} } 
