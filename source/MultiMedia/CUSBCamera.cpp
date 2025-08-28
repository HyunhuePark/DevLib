#include "../../include/MultiMedia/CUSBCamera.hpp"

#include <iostream>

#if defined(_MSC_VER)
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mfobjects.h>

#include <setupapi.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "cfgmgr32.lib")

#include "../../include/Utility/String.hpp"

#elif defined(__linux__) // Linux
#include "../../include/IO/DevLibFileUtility.hpp"
#include <linux/videodev2.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <tuple>
#include <cstring>
#include <unistd.h>

#endif

namespace DevLib
{
	namespace MultiMedia
	{

#if defined(_MSC_VER)

		static std::atomic_int _g_initCount = 0;
		static IMFAttributes* _g_pAttributes = nullptr;
		static IMFActivate** _g_ppDevices = nullptr;
		static UINT32 _g_DeviceCount = 0;

#elif defined(__linux__) // Linux

		// static std::string __ToStringFormat(uint32_t format)
		// {
		// 	std::string s;

		// 	s += format & 0x7f;
		// 	s += (format >> 8) & 0x7f;
		// 	s += (format >> 16) & 0x7f;
		// 	s += (format >> 24) & 0x7f;
		// 	if (format & (1 << 31))
		// 		s += "-BE";
		// 	return s;
		// }

		static bool __InitVideoBuffer(int camera, std::vector<std::pair<void*, size_t>>& buff, uint32_t buffSize = 4)
		{
			bool bRet = false;
			struct v4l2_requestbuffers req;
			memset(&req, 0, sizeof req);

			req.count = buffSize;
			req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			req.memory = V4L2_MEMORY_MMAP;

			if (ioctl(camera, VIDIOC_REQBUFS, &req) == 0)
			{
				buff.resize(req.count);

				for (size_t i = 0; i < req.count; i++)
				{
					struct v4l2_buffer buf;
					memset(&buf, 0, sizeof buf);
					buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
					buf.memory = V4L2_MEMORY_MMAP;
					buf.index = i;

					if (ioctl(camera, VIDIOC_QUERYBUF, &buf) == 0)
					{
						buff[i].first = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, camera, buf.m.offset);
						buff[i].second = buf.length;

						bRet = true;
					}
				}
			}

			return bRet;
		}

		// list '/dev/video'
		static std::vector<std::pair<std::string, v4l2_capability>> __video_list_;

#endif

		static void InitVideoInput_()
		{
#if defined(_MSC_VER)

			if (_g_initCount == 0)
			{
				if (SUCCEEDED(MFStartup(MF_VERSION)))
				{
					if (SUCCEEDED(MFCreateAttributes(&_g_pAttributes, 1)))
					{
						_g_pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
					}
				}
				else
				{
					std::cerr << "Failed to initialize Media Foundation.\n";
				}
			}

			++_g_initCount;

#elif defined(__linux__) // Linux

			if (__video_list_.empty())
			{
				DevLib::IO::FileInfo info;
				DevLib::IO::ReadFolder(info, "/dev/");

				//	Find Device Video
				for (const auto& res : info)
				{
					if (res.second == false)
					{
						if (res.first.find("/dev/video") == 0)
						{
							auto cam = open(res.first.c_str(), O_RDWR);
							if (cam)
							{
								struct v4l2_capability cap;
								if (ioctl(cam, VIDIOC_QUERYCAP, &cap) == 0)
								{
									struct v4l2_format vfmt;
									memset(&vfmt, 0, sizeof(struct v4l2_format));
									vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

									if (ioctl(cam, VIDIOC_G_FMT, &vfmt) == 0)
									{
										// Camera Add
										__video_list_.push_back({ res.first, cap });
									}

									close(cam);
								}
							}
						}
					}
				}
			}
#endif
		}

		static void UnInitVideoInput_()
		{
#if defined(_MSC_VER)

			--_g_initCount;

			if (_g_initCount == 0)
			{
				if (_g_ppDevices)
				{
					CoTaskMemFree(_g_ppDevices);
					_g_ppDevices = nullptr;
				}

				if (_g_pAttributes)
				{
					_g_pAttributes->Release();
					_g_pAttributes = nullptr;
				}

				MFShutdown(); // Media Foundation
			}

#elif defined(__linux__) // Linux
			if (!__video_list_.empty()) __video_list_.clear();

#endif
		}

		CUSBCamera::CUSBCamera(DevLib::Graphics::ImageType outputType, DevLib::MultiMedia::ConvertType cvtType)
			: m_idxCamera(-1), cameraOutType(outputType)
		{
#if defined(_MSC_VER)
			hCamera = nullptr;
#elif defined(__linux__) // Linux
			m_hCamera = 0;
			m_decoder.CreateDecoder(DevLib::MultiMedia::CodecType::MJPEG, outputType, cvtType);
#endif

			InitVideoInput_();
		}

		CUSBCamera::~CUSBCamera()
		{
#if defined(_MSC_VER)
			const auto pSource = static_cast<IMFMediaSource*>(hCamera);
			const auto pReader = static_cast<IMFSourceReader*>(hCameraReader);

			if (pSource)
			{
				pSource->Release();
				hCamera = nullptr;
			}

			if (pReader)
			{
				pReader->Release();
				hCameraReader = nullptr;
			}

			UnInitVideoInput_();
#elif defined(__linux__) // Linux

			enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(m_hCamera, VIDIOC_STREAMOFF, &type) == -1)
			{
				// printf("~CUSBCamera::VIDIOC_STREAMOFF\n");
			}

			for (auto& buf : m_videoBuffer)
			{
				munmap(buf.first, buf.second);
			}
#endif
		}

		uint32_t CUSBCamera::GetCameraCount()
		{
#if defined(_MSC_VER)

			if (_g_initCount == 0)
				InitVideoInput_();

			if (_g_ppDevices)
			{
				CoTaskMemFree(_g_ppDevices);
				_g_ppDevices = nullptr;
			}
			MFEnumDeviceSources(_g_pAttributes, &_g_ppDevices, &_g_DeviceCount);

			return _g_DeviceCount;

#elif defined(__linux__) // Linux

			return __video_list_.size();

#endif
		}

		string_t CUSBCamera::GetCameraName(uint32_t cameraNum)
		{
			string_t sRet = "null camera index";

#if defined(_MSC_VER)
			if (cameraNum < _g_DeviceCount)
			{
				WCHAR* pFriendlyName = nullptr;

				_g_ppDevices[cameraNum]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &pFriendlyName, nullptr);

				sRet = DevLib::UnicodeToMultiByte(pFriendlyName);

				CoTaskMemFree(pFriendlyName);  
			}

			return sRet;

#elif defined(__linux__) // Linux


			if (cameraNum < __video_list_.size())
			{
				sRet = string_t(reinterpret_cast<char_t*>(__video_list_[cameraNum].second.card));
			}

			return sRet;

#endif
		}

		string_t CUSBCamera::GetCameraPath(uint32_t cameraNum)
		{
			string_t sRet = "null camera index";

#if defined(_MSC_VER)
			if (cameraNum < _g_DeviceCount)
			{
				WCHAR devicePath[256] = { 0 };
				_g_ppDevices[cameraNum]->GetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, devicePath, sizeof(devicePath) / sizeof(WCHAR), nullptr);

				sRet = DevLib::UnicodeToMultiByte(devicePath);
			}

			return sRet;

#elif defined(__linux__) // Linux


			if (cameraNum < __video_list_.size())
			{
				sRet = __video_list_[cameraNum].first;
			}

			return sRet;

#endif
		}

		string_t CUSBCamera::GetCameraPhyAddress(uint32_t cameraNum)
		{
			string_t sRet = "null camera index";
#if defined(_MSC_VER)
			if (cameraNum < _g_DeviceCount)
			{
				const std::string strSymbolicLinkOrigin = GetCameraPath(cameraNum);

				const std::string::size_type pos = strSymbolicLinkOrigin.find('#');
				if (pos != std::string::npos)
				{

					// Device
					std::string devicePath = strSymbolicLinkOrigin.substr(4, strSymbolicLinkOrigin.find("#{") - 4);
					std::replace(devicePath.begin(), devicePath.end(), '#', '\\');
					std::transform(devicePath.begin(), devicePath.end(), devicePath.begin(), ::toupper);

					// SetupDiGetClassDevs
					const HDEVINFO deviceInfoSet = SetupDiGetClassDevs(nullptr, "USB", nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES);

					if (deviceInfoSet != INVALID_HANDLE_VALUE)
					{
						SP_DEVINFO_DATA deviceInfoData = {};
						deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

						// Device 
						DWORD index = 0;
						while (SetupDiEnumDeviceInfo(deviceInfoSet, index, &deviceInfoData))
						{
							index++;

							char deviceID[1024] = { 0 };
							if (SetupDiGetDeviceInstanceId(deviceInfoSet, &deviceInfoData, deviceID, sizeof(deviceID) / sizeof(char), nullptr))
							{
								if (devicePath == deviceID)
								{
									// Retrieve the location paths property (SPDRP_LOCATION_PATHS)
									constexpr char locationPaths[1024] = { 0 };
									if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, SPDRP_LOCATION_PATHS, // Fetch physical location paths
										nullptr, reinterpret_cast<PBYTE>(const_cast<char*>(locationPaths)), sizeof(locationPaths), nullptr))
									{
										sRet = locationPaths;
									}
								}
							}
						}

					}

					SetupDiDestroyDeviceInfoList(deviceInfoSet);
				}

			}

			return sRet;

#elif defined(__linux__) // Linux

			if (cameraNum < __video_list_.size())
			{
				sRet = string_t(reinterpret_cast<char_t*>(__video_list_[cameraNum].second.bus_info));
			}

			return sRet;

#endif
		}

		bool CUSBCamera::SelectCamera(uint32_t cameraNum)
		{
			bool bRet = false;
#if defined(_MSC_VER)

			if (cameraNum < _g_DeviceCount)
			{
				auto pSource = static_cast<IMFMediaSource*>(hCamera);

				if (pSource)
				{
					pSource->Release();
					hCamera = nullptr;
					pSource = static_cast<IMFMediaSource*>(hCamera);
				}

				if (SUCCEEDED(_g_ppDevices[cameraNum]->ActivateObject(IID_PPV_ARGS(&pSource))))
				{
					hCamera = pSource;

					auto pReader = static_cast<IMFSourceReader*>(hCameraReader);

					if (pReader)
					{
						pReader->Release();
						hCameraReader = nullptr;
						pReader = static_cast<IMFSourceReader*>(hCameraReader);
					}

					if (SUCCEEDED(MFCreateSourceReaderFromMediaSource(pSource, NULL, &pReader)))
					{
						hCameraReader = pReader;

						IMFMediaType* pMediaType = nullptr;

						HRESULT hr = pReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pMediaType);
						if (SUCCEEDED(hr))
						{
							if (SUCCEEDED(MFGetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, &cameraWidth, &cameraHeight)))
							{
								if (SUCCEEDED(pMediaType->GetGUID(MF_MT_SUBTYPE, &pixelFormat)))
								{
									if (pixelFormat == MFVideoFormat_YUY2)
									{
										m_inputImage.Create(cameraWidth, cameraHeight);
									}
									else if (pixelFormat == MFVideoFormat_NV12)
									{
										m_inputImage.Create(cameraWidth, cameraHeight, DevLib::Graphics::ImageType::IMG_NV12);
									}
									else if (pixelFormat == MFVideoFormat_RGB32)
									{
										m_inputImage.Create(cameraWidth, cameraHeight, DevLib::Graphics::ImageType::IMG_RGBA);
									}
									else if (pixelFormat == MFVideoFormat_RGB24)
									{
										m_inputImage.Create(cameraWidth, cameraHeight, DevLib::Graphics::ImageType::IMG_RGB);
									}
									else
									{
									}

									UINT32 numerator = 0, denominator = 0;
									hr = MFGetAttributeRatio(pMediaType, MF_MT_FRAME_RATE, &numerator, &denominator);
									if (SUCCEEDED(hr))
									{
										cameraFrameRate = denominator != 0 ? static_cast<float>(numerator) / denominator : 0;
									}

									m_idxCamera = cameraNum;
									bRet = true;
								}
							}

							pMediaType->Release();
						}
					}
				}
			}

#elif defined(__linux__) // Linux

			if (cameraNum < __video_list_.size())
			{
				if (m_hCamera > 0)
				{
					close(m_hCamera);
					m_hCamera = 0;
				}

				m_hCamera = open(__video_list_[cameraNum].first.c_str(), O_RDWR);

				if (m_hCamera)
				{
					struct v4l2_format vfmt;
					memset(&vfmt, 0, sizeof(struct v4l2_format));
					vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

					vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;

					if (ioctl(m_hCamera, VIDIOC_S_FMT, &vfmt) == 0)
					{
						if (ioctl(m_hCamera, VIDIOC_G_FMT, &vfmt) == 0)
						{
							if (__InitVideoBuffer(m_hCamera, m_videoBuffer))
							{
								m_inputImage.Destroy();
								m_inputImage.Create(vfmt.fmt.pix.width, vfmt.fmt.pix.height);

								for (size_t i = 0; i < m_videoBuffer.size(); i++)
								{
									struct v4l2_buffer buf;
									memset(&buf, 0, sizeof buf);
									buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
									buf.memory = V4L2_MEMORY_MMAP;
									buf.index = i;

									if (ioctl(m_hCamera, VIDIOC_QBUF, &buf) == -1)
									{
										// printf("SelectCamera::VIDIOC_QBUF\n");
									}
								}

								enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
								if (ioctl(m_hCamera, VIDIOC_STREAMON, &type) == -1)
								{
									// printf("SelectCamera::VIDIOC_STREAMON\n");
								}

								m_idxCamera = cameraNum;
								bRet = true;
							}
						}
					}
				}
			}
#endif

			return bRet;
		}

		bool CUSBCamera::SelectCamera(uint32_t width, uint32_t height, uint32_t cameraNum /*= 0*/)
		{
			bool bRet = false;
#if defined(_MSC_VER)

			if (cameraNum < _g_DeviceCount)
			{
				auto pSource = static_cast<IMFMediaSource*>(hCamera);

				if (pSource)
				{
					pSource->Release();
					hCamera = nullptr;
					pSource = static_cast<IMFMediaSource*>(hCamera);
				}

				if (SUCCEEDED(_g_ppDevices[cameraNum]->ActivateObject(IID_PPV_ARGS(&pSource))))
				{
					hCamera = pSource;

					auto pReader = static_cast<IMFSourceReader*>(hCameraReader);

					if (pReader)
					{
						pReader->Release();
						hCameraReader = nullptr;
						pReader = static_cast<IMFSourceReader*>(hCameraReader);
					}

					if (SUCCEEDED(MFCreateSourceReaderFromMediaSource(pSource, NULL, &pReader)))
					{
						hCameraReader = pReader;

						IMFMediaType* pSizeMediaType = nullptr;

						if (SUCCEEDED(MFCreateMediaType(&pSizeMediaType)))
						{
							pSizeMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video); 
							pSizeMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_MJPG); 

							MFSetAttributeSize(pSizeMediaType, MF_MT_FRAME_SIZE, width, height); 

							// Source Reader
							if (SUCCEEDED(pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, pSizeMediaType)))
							{
								const auto imageType = m_inputImage.GetImageType();
								m_inputImage.Destroy();
								m_inputImage.Create(width, height, imageType);
							}

							pSizeMediaType->Release();
						}

						IMFMediaType* pMediaType = nullptr;

						if (SUCCEEDED(pReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pMediaType)))
						{
							if (SUCCEEDED(MFGetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, &cameraWidth, &cameraHeight)))
							{
								if (SUCCEEDED(pMediaType->GetGUID(MF_MT_SUBTYPE, &pixelFormat)))
								{
									if (pixelFormat == MFVideoFormat_YUY2)
									{
										m_inputImage.Create(cameraWidth, cameraHeight);
									}
									else if (pixelFormat == MFVideoFormat_NV12)
									{
										m_inputImage.Create(cameraWidth, cameraHeight, DevLib::Graphics::ImageType::IMG_NV12);
									}
									else if (pixelFormat == MFVideoFormat_RGB32)
									{
										m_inputImage.Create(cameraWidth, cameraHeight, DevLib::Graphics::ImageType::IMG_RGBA);
									}
									else if (pixelFormat == MFVideoFormat_RGB24)
									{
										m_inputImage.Create(cameraWidth, cameraHeight, DevLib::Graphics::ImageType::IMG_RGB);
									}
									else
									{
									}

									UINT32 numerator = 0, denominator = 0;
									if (SUCCEEDED(MFGetAttributeRatio(pMediaType, MF_MT_FRAME_RATE, &numerator, &denominator)))
									{
										cameraFrameRate = denominator != 0 ? static_cast<float>(numerator) / denominator : 0;
									}

									m_idxCamera = static_cast<int32_t>(cameraNum);
									bRet = true;
								}
							}

							pMediaType->Release();
						}
					}
				}
			}


			//if (__g_videoInpu_->setupDevice(cameraNum, width, height))
			//{
			//	m_idxCamera = cameraNum;

			//	m_inputImage.Destroy();
			//	m_inputImage.Create(__g_videoInpu_->getWidth(cameraNum), __g_videoInpu_->getHeight(cameraNum));

			//	bRet = true;
			//}

#elif defined(__linux__) // Linux

			if (cameraNum < __video_list_.size())
			{
				if (m_hCamera > 0)
				{
					close(m_hCamera);
					m_hCamera = 0;
				}

				m_hCamera = open(__video_list_[cameraNum].first.c_str(), O_RDWR);
				if (m_hCamera)
				{
					struct v4l2_format vfmt;
					memset(&vfmt, 0, sizeof(struct v4l2_format));
					vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

					if (ioctl(m_hCamera, VIDIOC_G_FMT, &vfmt) == 0)
					{
						vfmt.fmt.pix.width = width;
						vfmt.fmt.pix.height = height;
						vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;

						if (ioctl(m_hCamera, VIDIOC_S_FMT, &vfmt) == 0)
						{
							if (__InitVideoBuffer(m_hCamera, m_videoBuffer))
							{
								m_inputImage.Destroy();
								m_inputImage.Create(vfmt.fmt.pix.width, vfmt.fmt.pix.height);

#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
								for (size_t i = 0; i < m_videoBuffer.size(); i++)
								{
									struct v4l2_buffer buf;
									memset(&buf, 0, sizeof buf);
									buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
									buf.memory = V4L2_MEMORY_MMAP;
									buf.index = i;

									if (ioctl(m_hCamera, VIDIOC_QBUF, &buf) == -1)
									{
										// printf("SelectCamera::VIDIOC_QBUF\n");
									}
								}

								enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
								if (ioctl(m_hCamera, VIDIOC_STREAMON, &type) == -1)
								{
									// printf("SelectCamera::VIDIOC_STREAMON\n");
								}
#endif

								m_idxCamera = cameraNum;
								bRet = true;
							}
						}
					}
				}
			}

#endif
			return bRet;

		}

		void CUSBCamera::CloseCamera()
		{
			// Thread Stop
			StopCamera();

#if defined(_MSC_VER)
#elif defined(__linux__) // linux
			if (m_hCamera > 0) close(m_hCamera);
			m_hCamera = 0;

			// mem free
			for (auto& buff : m_videoBuffer)
			{
				munmap(buff.first, buff.second);
			}
			m_videoBuffer.clear();
#endif // linux
			// Re Init 
			UnInitVideoInput_();
			InitVideoInput_();

		}

		uint32_t CUSBCamera::GetSelectedCamera() const
		{
			return m_idxCamera;
		}

#if defined(_MSC_VER)
		inline std::string GetPixelFormatName(const GUID& subtype)
		{
			std::string ret = "Unknown Format";
			if (subtype == MFVideoFormat_YUY2) ret = "YUY2";
			else if (subtype == MFVideoFormat_NV12) ret = "NV12";
			else if (subtype == MFVideoFormat_RGB32) ret = "RGB32";
			else if (subtype == MFVideoFormat_RGB24) ret = "RGB";
			else if (subtype == MFVideoFormat_MJPG) ret = "MJPEG";
			else if (subtype == MFVideoFormat_H264) ret = "H264";

			return ret;
		}
#endif
		std::vector<std::string> CUSBCamera::GetSupportFormat() const
		{
			std::vector<std::string> vRet;

#if defined(_MSC_VER)
			if (IsSelectedCamera())
			{
				const auto pReader = static_cast<IMFSourceReader*>(hCameraReader);

				DWORD dwMediaTypeIndex = 0;
				IMFMediaType* pMediaType = nullptr;

				while (SUCCEEDED(pReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, dwMediaTypeIndex, &pMediaType)))
				{
					UINT32 width = 0, height = 0;
					MFGetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, &width, &height);

					GUID subtype;
					pMediaType->GetGUID(MF_MT_SUBTYPE, &subtype);

					UINT32 numerator = 0, denominator = 0;
					MFGetAttributeRatio(pMediaType, MF_MT_FRAME_RATE, &numerator, &denominator);

					std::string support = "Resolution : " + std::to_string(width) + "x" + std::to_string(height)
						+ ", Pixel Format : " + GetPixelFormatName(subtype) + ", FPS : " + std::to_string((denominator != 0 ? (numerator / denominator) : 0));

					vRet.push_back(support);

					pMediaType->Release();
					dwMediaTypeIndex++;
				}
			}
#elif defined(__linux__) // linux
#endif
			return { vRet };
		}

		bool CUSBCamera::StartCamera()
		{
			bool bRet = false;
			if (IsSelectedCamera())
			{
				bRet = m_watchDogCamera.StartThread(&CUSBCamera::WatchDogCamera, this);
			}

			return bRet;
		}

		void CUSBCamera::StopCamera()
		{
			m_watchDogCamera.StopThread();
		}

		void CUSBCamera::WatchDogCamera()
		{
#if defined(_MSC_VER)
			const auto pReader = static_cast<IMFSourceReader*>(hCameraReader);
#elif defined(__linux__) // Linux

			struct v4l2_buffer buf;
			memset(&buf, 0, sizeof buf);
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
#endif
			while (m_watchDogCamera.IsRunThread())
			{
#if defined(_MSC_VER)
				DWORD streamIndex = 0;
				DWORD flags = 0;
				LONGLONG timestamp = 0;
				IMFSample* pSample = nullptr;

				if (SUCCEEDED(pReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &streamIndex, &flags, &timestamp, &pSample)))
				{
					if (pSample)
					{
						IMFMediaBuffer* pBuffer = nullptr;

						if (SUCCEEDED(pSample->ConvertToContiguousBuffer(&pBuffer)))
						{
							if (pBuffer)
							{
								BYTE* pData = nullptr;
								DWORD cbMaxLength = 0, cbCurrentLength = 0;

								if (SUCCEEDED(pBuffer->Lock(&pData, &cbMaxLength, &cbCurrentLength)))
								{
									if (pixelFormat == MFVideoFormat_MJPG)
									{
										m_outImage->LoadImageJpegData(pData, cbCurrentLength);

										if (m_callbackCameraImage) m_callbackCameraImage(*m_outImage);
									}
									else
									{
										std::memcpy(m_inputImage.GetImage(), pData, cbCurrentLength);

										m_outImage.Lock();
										m_inputImage.ColorConvert(cameraOutType, *m_outImage);
										m_outImage.UnLock();

										if (m_callbackCameraImage) m_callbackCameraImage(*m_outImage);
									}
									pBuffer->Unlock();
								}

								pBuffer->Release();
							}
						}
						pSample->Release();
					}
				}

#elif defined(__linux__) // Linux

				if (ioctl(m_hCamera, VIDIOC_DQBUF, &buf) == -1)
				{
					// printf("WatchDogCamera::VIDIOC_DQBUF\n");
				}

				m_decoder.Decode(reinterpret_cast<uint8_t*>(m_videoBuffer[buf.index].first), buf.bytesused);
				if (m_callbackCameraImage)
				{
					m_callbackCameraImage(m_decoder.GetDecodedImage());
				}

				if (ioctl(m_hCamera, VIDIOC_QBUF, &buf) == -1)
				{
					// printf("WatchDogCamera::VIDIOC_QBUF\n");
				}
#endif
			}
		}

		bool CUSBCamera::GetImage(DevLib::Graphics::CImage& img)
		{
			bool bRet = false;
			if (IsSelectedCamera())
			{
#if defined(_MSC_VER)
				bRet = true;
				m_outImage.LockRead();
				img = *m_outImage;
				m_outImage.UnLockRead();
#elif defined(__linux__) // Linux

				struct v4l2_buffer buf;
				memset(&buf, 0, sizeof buf);
				buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				buf.memory = V4L2_MEMORY_MMAP;

				if (ioctl(m_hCamera, VIDIOC_QBUF, &buf) == -1)
				{
					// printf("GetImage::VIDIOC_QBUF\n");
				}

				if (ioctl(m_hCamera, VIDIOC_DQBUF, &buf) == -1)
				{
					// printf("GetImage::VIDIOC_DQBUF\n");
				}

				if (m_decoder.Decode(reinterpret_cast<uint8_t*>(m_videoBuffer[buf.index].first), buf.bytesused) > 0)
				{
					bRet = true;
					img = m_decoder.GetDecodedImage();
				}
#endif
			}
			return bRet;
		}

		bool CUSBCamera::GetImage(uint8_t* img)
		{
			bool bRet = false;
			if (IsSelectedCamera())
			{
#if defined(_MSC_VER)
				bRet = true;
				m_outImage.LockRead();
				std::memcpy(img, m_outImage->GetImage(), m_outImage->GetSize());
				m_outImage.UnLockRead();
#elif defined(__linux__) // Linux

				struct v4l2_buffer buf;
				memset(&buf, 0, sizeof buf);
				buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				buf.memory = V4L2_MEMORY_MMAP;

				if (ioctl(m_hCamera, VIDIOC_QBUF, &buf) == -1)
				{
					// printf("GetImage::VIDIOC_QBUF\n");
				}

				if (ioctl(m_hCamera, VIDIOC_DQBUF, &buf) == -1)
				{
					// printf("GetImage::VIDIOC_DQBUF\n");
				}

				if (m_decoder.Decode(reinterpret_cast<uint8_t*>(m_videoBuffer[buf.index].first), buf.bytesused) > 0)
				{
					bRet = true;
					memcpy(img, m_decoder.GetDecodedImage().GetImage(), m_decoder.GetDecodedImage().GetSize());
				}
#endif
			}
			return bRet;
		}

		bool CUSBCamera::SetCameraSize(uint32_t width, uint32_t height)
		{
			bool bRet = false;

			if (IsSelectedCamera())
			{
				const bool bRun = m_watchDogCamera.IsRunThread();
				if (bRun)
				{
					m_watchDogCamera.StopThread();
					m_watchDogCamera.WaitForEndThread();
				}
#if defined(_MSC_VER)

				IMFMediaType* pMediaType = nullptr;

				const HRESULT hr = MFCreateMediaType(&pMediaType);
				if (SUCCEEDED(hr))
				{
					pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video); 
					pMediaType->SetGUID(MF_MT_SUBTYPE, pixelFormat); 

					if (SUCCEEDED(MFSetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, width, height)))
					{
						// Source Reader
						const auto pReader = static_cast<IMFSourceReader*>(hCameraReader);
						if (SUCCEEDED(pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, pMediaType)))
						{
							if (SUCCEEDED(MFGetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, &cameraWidth, &cameraHeight)))
							{
								const auto imageType = m_inputImage.GetImageType();
								m_inputImage.Destroy();
								m_inputImage.Create(cameraWidth, cameraHeight, imageType);
							}
						}
					}
					pMediaType->Release();


					if (bRun)
					{
						StartCamera();
					}
				}
#elif defined(__linux__) // Linux
				struct v4l2_format vfmt;
				memset(&vfmt, 0, sizeof(struct v4l2_format));
				vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

				if (ioctl(m_hCamera, VIDIOC_G_FMT, &vfmt) == 0)
				{
					vfmt.fmt.pix.width = width;
					vfmt.fmt.pix.height = height;

					if (ioctl(m_hCamera, VIDIOC_S_FMT, &vfmt) == 0)
					{
						m_inputImage.Destroy();
						m_inputImage.Create(vfmt.fmt.pix.width, vfmt.fmt.pix.height);

						bRet = true;
					}
				}
#endif
			}
			return bRet;
		}

		bool CUSBCamera::SetFrameRate(float_t fps)
		{
			bool bRet = false;

			if (IsSelectedCamera())
			{
				const bool bRun = m_watchDogCamera.IsRunThread();
				if (bRun)
				{
					m_watchDogCamera.StopThread();
					m_watchDogCamera.WaitForEndThread();
				}

#if defined(_MSC_VER)

				IMFMediaType* pMediaType = nullptr;

				const HRESULT hr = MFCreateMediaType(&pMediaType);
				if (SUCCEEDED(hr))
				{
					pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video); 
					pMediaType->SetGUID(MF_MT_SUBTYPE, pixelFormat); 

					if (SUCCEEDED(MFSetAttributeSize(pMediaType, MF_MT_FRAME_SIZE, cameraWidth, cameraHeight)))
					{
						if (SUCCEEDED(MFSetAttributeRatio(pMediaType, MF_MT_FRAME_RATE, static_cast<uint32_t>(fps), 1)))
						{
							// Source Reader
							const auto pReader = static_cast<IMFSourceReader*>(hCameraReader);
							if (SUCCEEDED(pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, pMediaType)))
							{
								cameraFrameRate = fps;
								bRet = true;
							}
						}
					}
					pMediaType->Release();


					if (bRun)
					{
						StartCamera();
					}
				}

#elif defined(__linux__) // Linux

#endif
			}

			return bRet;
		}

		bool CUSBCamera::SetPixelFormat(CAMERA_FORMAT format)
		{
			bool bRet = false;

			if (IsSelectedCamera())
			{
				const bool bRun = m_watchDogCamera.IsRunThread();
				if (bRun)
				{
					m_watchDogCamera.StopThread();
					m_watchDogCamera.WaitForEndThread();
				}

#if defined(_MSC_VER)

				IMFMediaType* pMediaType = nullptr;

				// 
				const HRESULT hr = MFCreateMediaType(&pMediaType);
				if (SUCCEEDED(hr))
				{
					// 
					pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video); // 
					GUID newFormat = pixelFormat;
					switch (format)
					{
					case CAMERA_FORMAT::FORMAT_NV12:
						newFormat = MFVideoFormat_NV12;
						break;
					case CAMERA_FORMAT::FORMAT_MJPEG:
						newFormat = MFVideoFormat_MJPG;
						break;
					case CAMERA_FORMAT::FORMAT_BGR:
						newFormat = MFVideoFormat_RGB24;
						break;
					case CAMERA_FORMAT::FORMAT_BGRA:
						newFormat = MFVideoFormat_RGB32;
						break;
					default:;
					}

					pMediaType->SetGUID(MF_MT_SUBTYPE, newFormat); //

					const auto pReader = static_cast<IMFSourceReader*>(hCameraReader);
					if (SUCCEEDED(pReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, pMediaType)))
					{
						pixelFormat = newFormat;
						bRet = true;
					}

					pMediaType->Release();


					if (bRun)
					{
						StartCamera();
					}
				}

#elif defined(__linux__) // Linux

#endif
			}
			return bRet;

		}

		bool CUSBCamera::IsSelectedCamera() const
		{
			bool bRet;

			if (m_idxCamera >= 0)
			{
				bRet = true;
			}
			else
			{
				bRet = false;
			}

			return bRet;
		}

	}
}