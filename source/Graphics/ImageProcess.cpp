#include "../../include/Graphics/ImageProcess.hpp"

#include <cmath>
#include <cstring>
#include <vector>

#define LIMIT(x) (((uint8_t)(x)) & 0xff)
#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#define MAX(a, b) ((a) <= (b) ? (b) : (a))
#define MIN3(a, b, c) (MIN(a, MIN(b, c)))
#define MAX3(a, b, c) (MAX(a, MAX(b, c)))

namespace DevLib
{
	namespace Graphics
	{
		namespace ImageProcess
		{
			// [클램프 함수] - 값이 0~255 범위를 벗어나지 않도록 합니다.
			inline unsigned char clamp(int32_t value)
			{
				return static_cast<unsigned char>(value < 0 ? 0 : (value > 255 ? 255 : value));
			}

			void Convert888To565(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				uint32_t pos, dst_pos;
				const uint32_t size = src_width * src_height * 3;
				auto* pDst = reinterpret_cast<uint16_t*>(dst);

				for (pos = 0, dst_pos = 0; pos < size; pos += 3, dst_pos++)
				{
					pDst[dst_pos] = (src[pos + 2] >> 3) << 11 | (src[pos + 1] >> 2) << 5 | (src[pos + 0] >> 3);
				}
			}

			void Convert888To555(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				uint32_t pos, dst_pos;

				const uint32_t size = src_width * src_height * 3;
				auto* pDst = reinterpret_cast<uint16_t*>(dst);

				for (pos = 0, dst_pos = 0; pos < size; pos += 3, dst_pos++)
				{
					pDst[dst_pos] = (src[pos + 2] >> 3) << 10 | (src[pos + 1] >> 3) << 5 | (src[pos + 0] >> 3);
				}
			}

			void ConvertBGRToGray(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 3;
				uint32_t dstPos = 0;

				for (uint32_t pos = 0; pos < size; pos += 3, dstPos++)
				{
					dst[dstPos] = static_cast<uint8_t>((src[pos + 2] * 0.299) + // B
						(src[pos + 1] * 0.587) + // G
						(src[pos + 0] * 0.114)); // R
				}
			}

			void ConvertBGRToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t srcPos = 0, pos = 0; pos < size; pos += 4, srcPos += 3)
				{
					dst[pos + 0] = src[srcPos + 0];
					dst[pos + 1] = src[srcPos + 1];
					dst[pos + 2] = src[srcPos + 2];
					dst[pos + 3] = 255;
				}
			}

			void ConvertBGRToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t srcPos = 0, pos = 0; pos < size; pos += 4, srcPos += 3)
				{
					dst[pos + 0] = src[srcPos + 2];
					dst[pos + 1] = src[srcPos + 1];
					dst[pos + 2] = src[srcPos + 0];
					dst[pos + 3] = 255;
				}
			}

			void ConvertBGRToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t srcPos = 0, pos = 0; pos < size; pos += 4, srcPos += 3)
				{
					dst[pos + 0] = 255;
					dst[pos + 1] = src[srcPos + 0];
					dst[pos + 2] = src[srcPos + 1];
					dst[pos + 3] = src[srcPos + 2];
				}
			}

			void ConvertBGRToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t srcPos = 0, pos = 0; pos < size; pos += 4, srcPos += 3)
				{
					dst[pos + 0] = 255;
					dst[pos + 1] = src[srcPos + 2];
					dst[pos + 2] = src[srcPos + 1];
					dst[pos + 3] = src[srcPos + 0];
				}
			}

			void ConvertRGBToGray(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 3;
				uint32_t dstPos = 0;

				for (uint32_t pos = 0; pos < size; pos += 3, dstPos++)
				{
					dst[dstPos] = static_cast<uint8_t>((src[pos + 0] * 0.299) + // R
						(src[pos + 1] * 0.587) + // G
						(src[pos + 2] * 0.114)); // B
				}
			}

			void ConvertRGBToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t srcPos = 0, pos = 0; pos < size; pos += 4, srcPos += 3)
				{
					dst[pos + 0] = src[srcPos + 0];
					dst[pos + 1] = src[srcPos + 1];
					dst[pos + 2] = src[srcPos + 2];
					dst[pos + 3] = 255;
				}
			}

			void ConvertRGBToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t srcPos = 0, pos = 0; pos < size; pos += 4, srcPos += 3)
				{
					dst[pos + 0] = src[srcPos + 2];
					dst[pos + 1] = src[srcPos + 1];
					dst[pos + 2] = src[srcPos + 0];
					dst[pos + 3] = 255;
				}
			}

			void ConvertRGBToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t srcPos = 0, pos = 0; pos < size; pos += 4, srcPos += 3)
				{
					dst[pos + 0] = 255;
					dst[pos + 1] = src[srcPos + 0];
					dst[pos + 2] = src[srcPos + 1];
					dst[pos + 3] = src[srcPos + 2];
				}
			}

			void ConvertRGBToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t srcPos = 0, pos = 0; pos < size; pos += 4, srcPos += 3)
				{
					dst[pos + 0] = 255;
					dst[pos + 1] = src[srcPos + 2];
					dst[pos + 2] = src[srcPos + 1];
					dst[pos + 3] = src[srcPos + 0];
				}
			}

			void ConvertBGRAToGray(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;
				uint32_t dstPos = 0;

				for (uint32_t pos = 0; pos < size; pos += 4, dstPos++)
				{
					dst[dstPos] = static_cast<uint8_t>((src[pos + 0] * 0.299) + // R
						(src[pos + 1] * 0.587) + // G
						(src[pos + 2] * 0.114)); // B
				}
			}

			void ConvertRGBAToGray(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;
				uint32_t dstPos = 0;

				for (uint32_t pos = 0; pos < size; pos += 4, dstPos++)
				{
					dst[dstPos] = static_cast<uint8_t>((src[pos + 0] * 0.299) + // R
						(src[pos + 1] * 0.587) + // G
						(src[pos + 2] * 0.114)); // B
				}
			}

			void ConvertBGRAToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t posSrc = 0, dstPos = 0; posSrc < size; posSrc += 4, dstPos += 3)
				{
					dst[dstPos + 0] = src[posSrc + 0];
					dst[dstPos + 1] = src[posSrc + 1];
					dst[dstPos + 2] = src[posSrc + 2];
				}
			}

			void ConvertBGRAToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t posSrc = 0, dstPos = 0; posSrc < size; posSrc += 4, dstPos += 3)
				{
					dst[dstPos + 0] = src[posSrc + 2];
					dst[dstPos + 1] = src[posSrc + 1];
					dst[dstPos + 2] = src[posSrc + 0];
				}
			}

			void ConvertBGRAToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t pos = 0; pos < size; pos += 4)
				{
					dst[pos + 0] = src[pos + 3];
					dst[pos + 1] = src[pos + 2];
					dst[pos + 2] = src[pos + 1];
					dst[pos + 3] = src[pos + 0];
				}
			}

			void ConvertBGRAToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t pos = 0; pos < size; pos += 4)
				{
					dst[pos + 0] = src[pos + 3];
					dst[pos + 1] = src[pos + 0];
					dst[pos + 2] = src[pos + 1];
					dst[pos + 3] = src[pos + 2];
				}
			}

			void ConvertBGRAToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t pos = 0; pos < size; pos += 4)
				{
					dst[pos + 0] = src[pos + 2];
					dst[pos + 1] = src[pos + 1];
					dst[pos + 2] = src[pos + 0];
					dst[pos + 3] = src[pos + 3];
				}
			}

			void ConvertRGBAToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t posSrc = 0, dstPos = 0; posSrc < size; posSrc += 4, dstPos += 3)
				{
					dst[dstPos + 0] = src[posSrc + 2];
					dst[dstPos + 1] = src[posSrc + 1];
					dst[dstPos + 2] = src[posSrc + 0];
				}
			}

			void ConvertRGBAToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t posSrc = 0, dstPos = 0; posSrc < size; posSrc += 4, dstPos += 3)
				{
					dst[dstPos + 0] = src[posSrc + 0];
					dst[dstPos + 1] = src[posSrc + 1];
					dst[dstPos + 2] = src[posSrc + 2];
				}
			}

			void ConvertRGBAToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t pos = 0; pos < size; pos += 4)
				{
					dst[pos + 0] = src[pos + 2];
					dst[pos + 1] = src[pos + 1];
					dst[pos + 2] = src[pos + 0];
					dst[pos + 3] = src[pos + 3];
				}
			}

			void ConvertRGBAToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t pos = 0; pos < size; pos += 4)
				{
					dst[pos + 0] = src[pos + 3];
					dst[pos + 1] = src[pos + 0];
					dst[pos + 2] = src[pos + 1];
					dst[pos + 3] = src[pos + 2];
				}
			}

			void ConvertRGBAToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t pos = 0; pos < size; pos += 4)
				{
					dst[pos + 0] = src[pos + 3];
					dst[pos + 1] = src[pos + 0];
					dst[pos + 2] = src[pos + 1];
					dst[pos + 3] = src[pos + 2];
				}
			}

			void ConvertARGBToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t posSrc = 0, dstPos = 0; posSrc < size; posSrc += 4, dstPos += 3)
				{
					dst[dstPos + 0] = src[posSrc + 3];
					dst[dstPos + 1] = src[posSrc + 2];
					dst[dstPos + 2] = src[posSrc + 1];
				}
			}

			void ConvertARGBToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t posSrc = 0, dstPos = 0; posSrc < size; posSrc += 4, dstPos += 3)
				{
					dst[dstPos + 0] = src[posSrc + 1];
					dst[dstPos + 1] = src[posSrc + 2];
					dst[dstPos + 2] = src[posSrc + 3];
				}
			}

			void ConvertARGBToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t pos = 0; pos < size; pos += 4)
				{
					dst[pos + 0] = src[pos + 3];
					dst[pos + 1] = src[pos + 2];
					dst[pos + 2] = src[pos + 1];
					dst[pos + 3] = src[pos + 0];
				}
			}

			void ConvertARGBToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t pos = 0; pos < size; pos += 4)
				{
					dst[pos + 0] = src[pos + 3];
					dst[pos + 1] = src[pos + 0];
					dst[pos + 2] = src[pos + 1];
					dst[pos + 3] = src[pos + 2];
				}
			}


			void ConvertARGBToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t pos = 0; pos < size; pos += 4)
				{
					dst[pos + 0] = src[pos + 0];
					dst[pos + 1] = src[pos + 2];
					dst[pos + 2] = src[pos + 1];
					dst[pos + 3] = src[pos + 3];
				}
			}

			void ConvertARGBToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t pos = 0; pos < size; pos += 4)
				{
					dst[pos + 0] = src[pos + 0];
					dst[pos + 1] = src[pos + 1];
					dst[pos + 2] = src[pos + 2];
					dst[pos + 3] = src[pos + 3];
				}
			}

			void ConvertGrayToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height;
				uint32_t dstPos = 0;

				for (uint32_t pos = 0; pos < size; pos++, dstPos += 3)
				{
					dst[dstPos + 0] = dst[dstPos + 1] = dst[dstPos + 2] = static_cast<uint8_t>(src[pos]);
				}
			}

			void ConvertGrayToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				ConvertGrayToBGR(src, src_width, src_height, dst);
			}

			void ConvertGrayToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height;

				for (uint32_t pos = 0; pos < size; pos++)
				{
					dst[pos * 4 + 0] = src[pos];
					dst[pos * 4 + 1] = src[pos];
					dst[pos * 4 + 2] = src[pos];
					dst[pos * 4 + 3] = 255;
				}
			}

			void ConvertGrayToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height;

				for (uint32_t pos = 0; pos < size; pos++)
				{
					dst[pos * 4 + 0] = src[pos];
					dst[pos * 4 + 1] = src[pos];
					dst[pos * 4 + 2] = src[pos];
					dst[pos * 4 + 3] = 255;
				}
			}

			void ConvertGrayToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height;

				for (uint32_t pos = 0; pos < size; pos++)
				{
					dst[pos * 4 + 0] = 255;
					dst[pos * 4 + 1] = src[pos];
					dst[pos * 4 + 2] = src[pos];
					dst[pos * 4 + 3] = src[pos];
				}
			}

			void ConvertGrayToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height;

				for (uint32_t pos = 0; pos < size; pos++)
				{
					dst[pos * 4 + 0] = 255;
					dst[pos * 4 + 1] = src[pos];
					dst[pos * 4 + 2] = src[pos];
					dst[pos * 4 + 3] = src[pos];
				}
			}

			void ConvertBGRToYCbCr(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t pos = 0; pos < size; pos += 3)
				{
					dst[pos + 0] = static_cast<uint8_t>(((66 * src[pos + 2] + 129 * src[pos + 1] + 25 * src[pos + 0] + 128) >> 8) + 16);   // Y
					dst[pos + 1] = static_cast<uint8_t>(((-38 * src[pos + 2] - 74 * src[pos + 1] + 112 * src[pos + 0] + 128) >> 8) + 128); // Cb
					dst[pos + 2] = static_cast<uint8_t>(((112 * src[pos + 2] - 94 * src[pos + 1] - 18 * src[pos + 0] + 128) >> 8) + 128);  // Cr
				}
			}

			void ConvertRGBToYCbCr(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t pos = 0; pos < size; pos += 3)
				{
					dst[pos + 0] = static_cast<uint8_t>(((66 * src[pos + 0] + 129 * src[pos + 1] + 25 * src[pos + 2] + 128) >> 8) + 16);   // Y
					dst[pos + 1] = static_cast<uint8_t>(((-38 * src[pos + 0] - 74 * src[pos + 1] + 112 * src[pos + 2] + 128) >> 8) + 128); // Cb
					dst[pos + 2] = static_cast<uint8_t>(((112 * src[pos + 0] - 94 * src[pos + 1] - 18 * src[pos + 2] + 128) >> 8) + 128);  // Cr
				}
			}

			void ConvertYCbCrToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t pos = 0; pos < size; pos += 3)
				{
					dst[pos + 2] = LIMIT((298 * (src[pos + 0] - 16) + 409 * (src[pos + 2] - 128) + 128) >> 8);								// Y
					dst[pos + 1] = LIMIT((298 * (src[pos + 0] - 16) - 100 * (src[pos + 1] - 128) - 208 * (src[pos + 2] - 128) + 128) >> 8); // Cb
					dst[pos + 0] = LIMIT((298 * (src[pos + 0] - 16) + 516 * (src[pos + 1] - 128) + 128) >> 8);								// Cr
				}
			}

			void ConvertYCbCrToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t pos = 0; pos < size; pos += 3)
				{
					dst[pos + 0] = LIMIT((298 * (src[pos + 0] - 16) + 409 * (src[pos + 2] - 128) + 128) >> 8);								// Y
					dst[pos + 1] = LIMIT((298 * (src[pos + 0] - 16) - 100 * (src[pos + 1] - 128) - 208 * (src[pos + 2] - 128) + 128) >> 8); // Cb
					dst[pos + 2] = LIMIT((298 * (src[pos + 0] - 16) + 516 * (src[pos + 1] - 128) + 128) >> 8);								// Cr
				}
			}

			void ConvertYCbCrToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t posSrc = 0, posDst = 0; posSrc < size; posSrc += 3, posDst += 4)
				{
					dst[posDst + 2] = LIMIT((298 * (src[posSrc + 0] - 16) + 409 * (src[posSrc + 2] - 128) + 128) >> 8);								// Y
					dst[posDst + 1] = LIMIT((298 * (src[posSrc + 0] - 16) - 100 * (src[posSrc + 1] - 128) - 208 * (src[posSrc + 2] - 128) + 128) >> 8); // Cb
					dst[posDst + 0] = LIMIT((298 * (src[posSrc + 0] - 16) + 516 * (src[posSrc + 1] - 128) + 128) >> 8);								// Cr
					dst[posDst + 3] = 255;
				}
			}

			void ConvertYCbCrToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t posSrc = 0, posDst = 0; posSrc < size; posSrc += 3, posDst += 4)
				{
					dst[posDst + 0] = LIMIT((298 * (src[posSrc + 0] - 16) + 409 * (src[posSrc + 2] - 128) + 128) >> 8);								// Y
					dst[posDst + 1] = LIMIT((298 * (src[posSrc + 0] - 16) - 100 * (src[posSrc + 1] - 128) - 208 * (src[posSrc + 2] - 128) + 128) >> 8); // Cb
					dst[posDst + 2] = LIMIT((298 * (src[posSrc + 0] - 16) + 516 * (src[posSrc + 1] - 128) + 128) >> 8);								// Cr
					dst[posDst + 3] = 255;
				}
			}

			void ConvertYCbCrToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t posSrc = 0, posDst = 0; posSrc < size; posSrc += 3, posDst += 4)
				{
					dst[posDst + 0] = 255;
					dst[posDst + 3] = LIMIT((298 * (src[posSrc + 0] - 16) + 409 * (src[posSrc + 2] - 128) + 128) >> 8);								// Y
					dst[posDst + 2] = LIMIT((298 * (src[posSrc + 0] - 16) - 100 * (src[posSrc + 1] - 128) - 208 * (src[posSrc + 2] - 128) + 128) >> 8); // Cb
					dst[posDst + 1] = LIMIT((298 * (src[posSrc + 0] - 16) + 516 * (src[posSrc + 1] - 128) + 128) >> 8);								// Cr
				}
			}

			void ConvertYCbCrToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t posSrc = 0, posDst = 0; posSrc < size; posSrc += 3, posDst += 4)
				{
					dst[posDst + 0] = 255;
					dst[posDst + 1] = LIMIT((298 * (src[posSrc + 0] - 16) + 409 * (src[posSrc + 2] - 128) + 128) >> 8);								// Y
					dst[posDst + 2] = LIMIT((298 * (src[posSrc + 0] - 16) - 100 * (src[posSrc + 1] - 128) - 208 * (src[posSrc + 2] - 128) + 128) >> 8); // Cb
					dst[posDst + 3] = LIMIT((298 * (src[posSrc + 0] - 16) + 516 * (src[posSrc + 1] - 128) + 128) >> 8);								// Cr
				}
			}

			void ConvertBGRToHSV(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t pos = 0; pos < size; pos += 3)
				{
					const uint8_t* r = &src[pos + 2];
					const uint8_t* g = &src[pos + 1];
					const uint8_t* b = &src[pos + 0];
					uint8_t* h = &dst[pos + 0];
					uint8_t* s = &dst[pos + 1];
					uint8_t* v = &dst[pos + 2];

					const uint8_t rgb_min = MIN3(*b, *g, *r);
					const uint8_t rgb_max = MAX3(*b, *g, *r);

					*v = rgb_max;
					if (*v == 0)
						*h = *s = 0;
					else
					{
						*s = 255 * (rgb_max - rgb_min) / *v;
						if (*s == 0)
							*h = 0;
						else
						{
							if (rgb_max == *r)
								*h = 0 + 43 * (*g - *b) / (rgb_max - rgb_min);
							else if (rgb_max == *g)
								*h = 85 + 43 * (*b - *r) / (rgb_max - rgb_min);
							else
								*h = 171 + 43 * (*r - *g) / (rgb_max - rgb_min); /* rgb_max == rgb.b */
						}
					}
				}
			}

			void ConvertRGBToHSV(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t pos = 0; pos < size; pos += 3)
				{
					const uint8_t* r = &src[pos + 0];
					const uint8_t* g = &src[pos + 1];
					const uint8_t* b = &src[pos + 2];
					uint8_t* h = &dst[pos + 0];
					uint8_t* s = &dst[pos + 1];
					uint8_t* v = &dst[pos + 2];

					const uint8_t rgb_min = MIN3(*b, *g, *r);
					const uint8_t rgb_max = MAX3(*b, *g, *r);

					*v = rgb_max;
					if (*v == 0)
						*h = *s = 0;
					else
					{
						*s = 255 * (rgb_max - rgb_min) / *v;
						if (*s == 0)
							*h = 0;
						else
						{
							if (rgb_max == *r)
								*h = 0 + 43 * (*g - *b) / (rgb_max - rgb_min);
							else if (rgb_max == *g)
								*h = 85 + 43 * (*b - *r) / (rgb_max - rgb_min);
							else
								*h = 171 + 43 * (*r - *g) / (rgb_max - rgb_min); /* rgb_max == rgb.b */
						}
					}
				}
			}

			void ConvertHSVToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				double_t R, G, B;
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t posSrc = 0; posSrc < size; posSrc += 3)
				{
					constexpr double_t angle = 0.00277777777777777777777777777778;
					constexpr double_t normal = 0.0039215686274509803921568627451;
					constexpr double_t normalHue = 0.00390625;

					// H = (src[ pos + 0])/256.0 * 360.0; S = src[ pos + 1]/255.0; V = src[ pos + 2]/255.0;
					double_t H = (src[posSrc + 0]) * normalHue * 360.0;
					const double_t S = src[posSrc + 1] * normal;
					const double_t V = src[posSrc + 2] * normal;
					uint8_t* r = &dst[posSrc + 2];
					uint8_t* g = &dst[posSrc + 1];
					uint8_t* b = &dst[posSrc + 0];

					const double_t C = S * V;
					const double_t Min = V - C;

					H -= 360 * floor(H * angle);
					H *= 0.01666666666666666666666666666667;
					const double_t X = C * (1 - fabs(H - 2 * floor(H * 0.5) - 1));

					switch (static_cast<int>(H))
					{
					case 0:
						R = Min + C;
						G = Min + X;
						B = Min;
						break;
					case 1:
						R = Min + X;
						G = Min + C;
						B = Min;
						break;
					case 2:
						R = Min;
						G = Min + C;
						B = Min + X;
						break;
					case 3:
						R = Min;
						G = Min + X;
						B = Min + C;
						break;
					case 4:
						R = Min + X;
						G = Min;
						B = Min + C;
						break;
					case 5:
						R = Min + C;
						G = Min;
						B = Min + X;
						break;
					default:
						R = G = B = 0;
					}
					*r = static_cast<uint8_t>(R * 255.0);
					*g = static_cast<uint8_t>(G * 255.0);
					*b = static_cast<uint8_t>(B * 255.0);
				}
			}

			void ConvertHSVToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				double_t R, G, B;
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t posSrc = 0; posSrc < size; posSrc += 3)
				{
					constexpr double_t angle = 0.00277777777777777777777777777778;
					constexpr double_t normal = 0.0039215686274509803921568627451;
					constexpr double_t normalHue = 0.00390625;

					// H = (src[ pos + 0])/256.0 * 360.0; S = src[ pos + 1]/255.0; V = src[ pos + 2]/255.0;
					double_t H = (src[posSrc + 0]) * normalHue * 360.0;
					const double_t S = src[posSrc + 1] * normal;
					const double_t V = src[posSrc + 2] * normal;
					uint8_t* r = &dst[posSrc + 0];
					uint8_t* g = &dst[posSrc + 1];
					uint8_t* b = &dst[posSrc + 2];

					const double_t C = S * V;
					const double_t Min = V - C;

					H -= 360 * floor(H * angle);
					H *= 0.01666666666666666666666666666667;
					const double_t X = C * (1 - fabs(H - 2 * floor(H * 0.5) - 1));

					switch (static_cast<int>(H))
					{
					case 0:
						R = Min + C;
						G = Min + X;
						B = Min;
						break;
					case 1:
						R = Min + X;
						G = Min + C;
						B = Min;
						break;
					case 2:
						R = Min;
						G = Min + C;
						B = Min + X;
						break;
					case 3:
						R = Min;
						G = Min + X;
						B = Min + C;
						break;
					case 4:
						R = Min + X;
						G = Min;
						B = Min + C;
						break;
					case 5:
						R = Min + C;
						G = Min;
						B = Min + X;
						break;
					default:
						R = G = B = 0;
					}
					*r = static_cast<uint8_t>(R * 255.0);
					*g = static_cast<uint8_t>(G * 255.0);
					*b = static_cast<uint8_t>(B * 255.0);
				}
			}

			void ConvertHSVToBGRA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				double_t R, G, B;
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t posSrc = 0, posDst = 0; posSrc < size; posSrc += 3, posDst += 4)
				{
					constexpr double_t angle = 0.00277777777777777777777777777778;
					constexpr double_t normal = 0.0039215686274509803921568627451;
					constexpr double_t normalHue = 0.00390625;

					// H = (src[ pos + 0])/256.0 * 360.0; S = src[ pos + 1]/255.0; V = src[ pos + 2]/255.0;
					double_t H = (src[posSrc + 0]) * normalHue * 360.0;
					const double_t S = src[posSrc + 1] * normal;
					const double_t V = src[posSrc + 2] * normal;
					uint8_t* r = &dst[posDst + 2];
					uint8_t* g = &dst[posDst + 1];
					uint8_t* b = &dst[posDst + 0];
					uint8_t* a = &dst[posDst + 3];

					const double_t C = S * V;
					const double_t Min = V - C;

					H -= 360 * floor(H * angle);
					H *= 0.01666666666666666666666666666667;
					const double_t X = C * (1 - fabs(H - 2 * floor(H * 0.5) - 1));

					switch (static_cast<int>(H))
					{
					case 0:
						R = Min + C;
						G = Min + X;
						B = Min;
						break;
					case 1:
						R = Min + X;
						G = Min + C;
						B = Min;
						break;
					case 2:
						R = Min;
						G = Min + C;
						B = Min + X;
						break;
					case 3:
						R = Min;
						G = Min + X;
						B = Min + C;
						break;
					case 4:
						R = Min + X;
						G = Min;
						B = Min + C;
						break;
					case 5:
						R = Min + C;
						G = Min;
						B = Min + X;
						break;
					default:
						R = G = B = 0;
					}
					*r = static_cast<uint8_t>(R * 255.0);
					*g = static_cast<uint8_t>(G * 255.0);
					*b = static_cast<uint8_t>(B * 255.0);
					*a = 255;
				}
			}

			void ConvertHSVToRGBA(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				double_t R, G, B;
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t posSrc = 0, posDst = 0; posSrc < size; posSrc += 3, posDst += 4)
				{
					constexpr double_t angle = 0.00277777777777777777777777777778;
					constexpr double_t normal = 0.0039215686274509803921568627451;
					constexpr double_t normalHue = 0.00390625;

					// H = (src[ pos + 0])/256.0 * 360.0; S = src[ pos + 1]/255.0; V = src[ pos + 2]/255.0;
					double_t H = (src[posSrc + 0]) * normalHue * 360.0;
					const double_t S = src[posSrc + 1] * normal;
					const double_t V = src[posSrc + 2] * normal;
					uint8_t* r = &dst[posDst + 0];
					uint8_t* g = &dst[posDst + 1];
					uint8_t* b = &dst[posDst + 2];
					uint8_t* a = &dst[posDst + 3];

					const double_t C = S * V;
					const double_t Min = V - C;

					H -= 360 * floor(H * angle);
					H *= 0.01666666666666666666666666666667;
					const double_t X = C * (1 - fabs(H - 2 * floor(H * 0.5) - 1));

					switch (static_cast<int>(H))
					{
					case 0:
						R = Min + C;
						G = Min + X;
						B = Min;
						break;
					case 1:
						R = Min + X;
						G = Min + C;
						B = Min;
						break;
					case 2:
						R = Min;
						G = Min + C;
						B = Min + X;
						break;
					case 3:
						R = Min;
						G = Min + X;
						B = Min + C;
						break;
					case 4:
						R = Min + X;
						G = Min;
						B = Min + C;
						break;
					case 5:
						R = Min + C;
						G = Min;
						B = Min + X;
						break;
					default:
						R = G = B = 0;
					}
					*r = static_cast<uint8_t>(R * 255.0);
					*g = static_cast<uint8_t>(G * 255.0);
					*b = static_cast<uint8_t>(B * 255.0);
					*a = 255;
				}
			}

			void ConvertHSVToABGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				double_t R, G, B;
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t posSrc = 0, posDst = 0; posSrc < size; posSrc += 3, posDst += 4)
				{
					constexpr double_t angle = 0.00277777777777777777777777777778;
					constexpr double_t normal = 0.0039215686274509803921568627451;
					constexpr double_t normalHue = 0.00390625;

					// H = (src[ pos + 0])/256.0 * 360.0; S = src[ pos + 1]/255.0; V = src[ pos + 2]/255.0;
					double_t H = (src[posSrc + 0]) * normalHue * 360.0;
					const double_t S = src[posSrc + 1] * normal;
					const double_t V = src[posSrc + 2] * normal;
					uint8_t* r = &dst[posDst + 3];
					uint8_t* g = &dst[posDst + 2];
					uint8_t* b = &dst[posDst + 1];
					uint8_t* a = &dst[posDst + 0];

					const double_t C = S * V;
					const double_t Min = V - C;

					H -= 360 * floor(H * angle);
					H *= 0.01666666666666666666666666666667;
					const double_t X = C * (1 - fabs(H - 2 * floor(H * 0.5) - 1));

					switch (static_cast<int>(H))
					{
					case 0:
						R = Min + C;
						G = Min + X;
						B = Min;
						break;
					case 1:
						R = Min + X;
						G = Min + C;
						B = Min;
						break;
					case 2:
						R = Min;
						G = Min + C;
						B = Min + X;
						break;
					case 3:
						R = Min;
						G = Min + X;
						B = Min + C;
						break;
					case 4:
						R = Min + X;
						G = Min;
						B = Min + C;
						break;
					case 5:
						R = Min + C;
						G = Min;
						B = Min + X;
						break;
					default:
						R = G = B = 0;
					}
					*r = static_cast<uint8_t>(R * 255.0);
					*g = static_cast<uint8_t>(G * 255.0);
					*b = static_cast<uint8_t>(B * 255.0);
					*a = 255;
				}
			}

			void ConvertHSVToARGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				double_t R, G, B;
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t posSrc = 0, posDst = 0; posSrc < size; posSrc += 3, posDst += 4)
				{
					constexpr double_t angle = 0.00277777777777777777777777777778;
					constexpr double_t normal = 0.0039215686274509803921568627451;
					constexpr double_t normalHue = 0.00390625;

					// H = (src[ pos + 0])/256.0 * 360.0; S = src[ pos + 1]/255.0; V = src[ pos + 2]/255.0;
					double_t H = (src[posSrc + 0]) * normalHue * 360.0;
					const double_t S = src[posSrc + 1] * normal;
					const double_t V = src[posSrc + 2] * normal;
					uint8_t* r = &dst[posDst + 1];
					uint8_t* g = &dst[posDst + 2];
					uint8_t* b = &dst[posDst + 3];
					uint8_t* a = &dst[posDst + 0];

					const double_t C = S * V;
					const double_t Min = V - C;

					H -= 360 * floor(H * angle);
					H *= 0.01666666666666666666666666666667;
					const double_t X = C * (1 - fabs(H - 2 * floor(H * 0.5) - 1));

					switch (static_cast<int>(H))
					{
					case 0:
						R = Min + C;
						G = Min + X;
						B = Min;
						break;
					case 1:
						R = Min + X;
						G = Min + C;
						B = Min;
						break;
					case 2:
						R = Min;
						G = Min + C;
						B = Min + X;
						break;
					case 3:
						R = Min;
						G = Min + X;
						B = Min + C;
						break;
					case 4:
						R = Min + X;
						G = Min;
						B = Min + C;
						break;
					case 5:
						R = Min + C;
						G = Min;
						B = Min + X;
						break;
					default:
						R = G = B = 0;
					}
					*r = static_cast<uint8_t>(R * 255.0);
					*g = static_cast<uint8_t>(G * 255.0);
					*b = static_cast<uint8_t>(B * 255.0);
					*a = 255;
				}
			}

			void ConvertNV12ToGray(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				std::memcpy(dst, src, src_width * src_height);
			}

			void ConvertNV12ToBGR(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint8_t* YPlane = src;
				const uint8_t* UVPlane = src + src_width * src_height;
				
				// 각 행마다 포인터를 미리 구해서 반복문 내 연산을 줄입니다.
				for (int32_t y = 0; y < src_height; ++y) 
				{
					const uint8_t* yRow = YPlane + y * src_width;
					// NV12의 UV 평면은 2행에 한 행씩 있으므로, y를 2로 나눈 결과를 사용합니다.
					const uint8_t* uvRow = UVPlane + ((y >> 1) * src_width);

					for (int32_t x = 0; x < src_width; ++x)
					{
						// Y 성분
						const int32_t Y_val = yRow[x];
						// x를 2로 나눠 UV 평면 내 인덱스(인터리브드: U, V)를 계산
						const int32_t uvIndex = (x >> 1) << 1;  // (x / 2) * 2

						const int32_t U = uvRow[uvIndex];
						const int32_t V = uvRow[uvIndex + 1];

						// 변환 공식의 중간 계산 값들
						const int32_t C = Y_val - 16;
						const int32_t D = U - 128;
						const int32_t E = V - 128;

						// YUV -> RGB 변환 (정수 연산)
						int32_t r = (298 * C + 409 * E + 128) >> 8;
						int32_t g = (298 * C - 100 * D - 208 * E + 128) >> 8;
						int32_t b = (298 * C + 516 * D + 128) >> 8;

						// 결과 클램프
						r = clamp(r);
						g = clamp(g);
						b = clamp(b);

						// 계산된 BGR 값을 최종 배열에 저장 (B, G, R 순)
						const int32_t index = (y * src_width + x) * 3;
						dst[index] = static_cast<uint8_t>(b);
						dst[index + 1] = static_cast<uint8_t>(g);
						dst[index + 2] = static_cast<uint8_t>(r);
					}
				}
			}

			void ConvertNV12ToRGB(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint8_t* YPlane = src;
				const uint8_t* UVPlane = src + src_width * src_height;

				// 각 행마다 포인터를 미리 구해서 반복문 내 연산을 줄입니다.
				for (int32_t y = 0; y < src_height; ++y)
				{
					const uint8_t* yRow = YPlane + y * src_width;
					// NV12의 UV 평면은 2행에 한 행씩 있으므로, y를 2로 나눈 결과를 사용합니다.
					const uint8_t* uvRow = UVPlane + ((y >> 1) * src_width);

					for (int32_t x = 0; x < src_width; ++x)
					{
						// Y 성분
						const int32_t Y_val = yRow[x];
						// x를 2로 나눠 UV 평면 내 인덱스(인터리브드: U, V)를 계산
						const int32_t uvIndex = (x >> 1) << 1;  // (x / 2) * 2

						const int32_t U = uvRow[uvIndex];
						const int32_t V = uvRow[uvIndex + 1];

						// 변환 공식의 중간 계산 값들
						const int32_t C = Y_val - 16;
						const int32_t D = U - 128;
						const int32_t E = V - 128;

						// YUV -> RGB 변환 (정수 연산)
						int32_t r = (298 * C + 409 * E + 128) >> 8;
						int32_t g = (298 * C - 100 * D - 208 * E + 128) >> 8;
						int32_t b = (298 * C + 516 * D + 128) >> 8;

						// 결과 클램프
						r = clamp(r);
						g = clamp(g);
						b = clamp(b);

						// 계산된 BGR 값을 최종 배열에 저장 (B, G, R 순)
						const int32_t index = (y * src_width + x) * 3;
						dst[index] = static_cast<uint8_t>(r);
						dst[index + 1] = static_cast<uint8_t>(g);
						dst[index + 2] = static_cast<uint8_t>(b);
					}
				}
			}

			void ConvertGrayToNV12(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				std::memcpy(dst, src, src_width * src_height);
				std::memset(&dst[src_width * src_height], 128, src_width * src_height);
			}

			void ConvertBGRToNV12(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const int32_t numPixels = src_width * src_height;
				// Y 평면 (전체 해상도)

				std::vector<uint8_t> yPlane(numPixels);
				// U, V 성분은 임시로 각 픽셀별로 계산
				std::vector<int32_t> uFull(numPixels);
				std::vector<int32_t> vFull(numPixels);

				// 1. 각 픽셀에 대해 BGR -> YUV 변환 (BT.601 계수 사용)
				//    - 공식: 
				//        Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16
				//        U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128
				//        V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128
				//
				//    참고: 변환 공식은 상황이나 표준에 따라 다를 수 있습니다.
				for (int32_t i = 0; i < numPixels; i++)
				{
					const int32_t idx = i * 3;
					const int32_t B = src[idx + 0];
					const int32_t G = src[idx + 1];
					const int32_t R = src[idx + 2];

					const int32_t Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
					const int32_t U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
					const int32_t V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;

					yPlane[i] = clamp(Y);
					uFull[i] = U;
					vFull[i] = V;
				}

				// 2. U, V 성분을 다운샘플링하여 UV 평면(인터리브: [U, V, U, V, ...])을 생성합니다.
				//    NV12 포맷에서는 2x2 픽셀 블록마다 U와 V 각각 1개씩 (평균값)을 저장합니다.
				//
				//    예를 들어, 좌상, 우상, 좌하, 우하 픽셀의 U 성분 평균을 내어 하나의 U 값을 결정합니다.
				const int32_t uvWidth = src_width;         // UV 평면의 각 행은 width 바이트 (각 블록 2바이트씩, 총 (width/2) 블록)
				const int32_t uvHeight = src_width / 2;     // 원래 이미지의 2행당 한 행의 UV 정보
				std::vector<uint8_t> uvInterleaved(uvWidth * uvHeight);

				size_t uvIndex = 0;
				// 2x2 블록 단위로 처리 (y, x 모두 2씩 증가)
				for (int32_t y = 0; y < src_height; y += 2)
				{
					for (int32_t x = 0; x < src_width; x += 2)
					{
						const int32_t index0 = y * src_width + x;         // 좌상
						const int32_t index1 = index0 + 1;              // 우상
						const int32_t index2 = index0 + src_width;          // 좌하
						const int32_t index3 = index2 + 1;              // 우하

						const int32_t u_sum = uFull[index0] + uFull[index1] + uFull[index2] + uFull[index3];
						const int32_t v_sum = vFull[index0] + vFull[index1] + vFull[index2] + vFull[index3];

						// 네 값의 평균 (반올림을 위해 +2를 하고 4로 나눕니다)
						const int32_t u_avg = (u_sum + 2) >> 2; // == (u_sum + 2) / 4;
						const int32_t v_avg = (v_sum + 2) >> 2; // == (v_sum + 2) / 4;

						uvInterleaved[uvIndex++] = clamp(u_avg);
						uvInterleaved[uvIndex++] = clamp(v_avg);
					}
				}

				// 결과 복사
				std::memcpy(dst, yPlane.data(), yPlane.size());
				std::memcpy(&dst[numPixels], uvInterleaved.data(), uvInterleaved.size());
			}

			void ConvertRGBToNV12(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const int32_t numPixels = src_width * src_height;
				// Y 평면 (전체 해상도)

				std::vector<uint8_t> yPlane(numPixels);
				// U, V 성분은 임시로 각 픽셀별로 계산
				std::vector<int32_t> uFull(numPixels);
				std::vector<int32_t> vFull(numPixels);

				// 1. 각 픽셀에 대해 BGR -> YUV 변환 (BT.601 계수 사용)
				//    - 공식: 
				//        Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16
				//        U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128
				//        V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128
				//
				//    참고: 변환 공식은 상황이나 표준에 따라 다를 수 있습니다.
				for (int32_t i = 0; i < numPixels; i++)
				{
					const int32_t idx = i * 3;
					const int32_t R = src[idx + 0];
					const int32_t G = src[idx + 1];
					const int32_t B = src[idx + 2];

					const int32_t Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
					const int32_t U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
					const int32_t V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;

					yPlane[i] = clamp(Y);
					uFull[i] = U;
					vFull[i] = V;
				}

				// 2. U, V 성분을 다운샘플링하여 UV 평면(인터리브: [U, V, U, V, ...])을 생성합니다.
				//    NV12 포맷에서는 2x2 픽셀 블록마다 U와 V 각각 1개씩 (평균값)을 저장합니다.
				//
				//    예를 들어, 좌상, 우상, 좌하, 우하 픽셀의 U 성분 평균을 내어 하나의 U 값을 결정합니다.
				const int32_t uvWidth = src_width;         // UV 평면의 각 행은 width 바이트 (각 블록 2바이트씩, 총 (width/2) 블록)
				const int32_t uvHeight = src_width / 2;     // 원래 이미지의 2행당 한 행의 UV 정보
				std::vector<uint8_t> uvInterleaved(uvWidth * uvHeight);

				size_t uvIndex = 0;
				// 2x2 블록 단위로 처리 (y, x 모두 2씩 증가)
				for (int32_t y = 0; y < src_height; y += 2)
				{
					for (int32_t x = 0; x < src_width; x += 2)
					{
						const int32_t index0 = y * src_width + x;         // 좌상
						const int32_t index1 = index0 + 1;              // 우상
						const int32_t index2 = index0 + src_width;          // 좌하
						const int32_t index3 = index2 + 1;              // 우하

						const int32_t u_sum = uFull[index0] + uFull[index1] + uFull[index2] + uFull[index3];
						const int32_t v_sum = vFull[index0] + vFull[index1] + vFull[index2] + vFull[index3];

						// 네 값의 평균 (반올림을 위해 +2를 하고 4로 나눕니다)
						const int32_t u_avg = (u_sum + 2) >> 2; // == (u_sum + 2) / 4;
						const int32_t v_avg = (v_sum + 2) >> 2; // == (v_sum + 2) / 4;

						uvInterleaved[uvIndex++] = clamp(u_avg);
						uvInterleaved[uvIndex++] = clamp(v_avg);
					}
				}

				// 결과 복사
				std::memcpy(dst, yPlane.data(), yPlane.size());
				std::memcpy(&dst[numPixels], uvInterleaved.data(), uvInterleaved.size());
			}

			void ConvertSwap24(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 3;

				for (uint32_t pos = 0; pos < size; pos += 3)
				{
					dst[pos + 2] = src[pos + 0];
					dst[pos + 1] = src[pos + 1];
					dst[pos + 0] = src[pos + 2];
				}
			}

			void ConvertSwap32(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t pos = 0; pos < size; pos += 4)
				{
					dst[pos + 2] = src[pos + 0];
					dst[pos + 1] = src[pos + 1];
					dst[pos + 0] = src[pos + 2];
					dst[pos + 3] = src[pos + 3];
				}
			}

			void ConvertSwap32Alpha(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				const uint32_t size = src_width * src_height * 4;

				for (uint32_t pos = 0; pos < size; pos += 4)
				{
					dst[pos + 0] = src[pos + 3];
					dst[pos + 1] = src[pos + 0];
					dst[pos + 2] = src[pos + 1];
					dst[pos + 3] = src[pos + 2];
				}
			}

			void FlipHorizontal1(uint8_t* src, int32_t src_width, int32_t src_height)
			{
				for (int32_t y = 0; y < src_height; ++y)
				{
					uint8_t* row = src + y * src_width; // 행 시작 위치 캐싱
					for (int32_t x = 0; x < src_width / 2; ++x)
					{
						// 왼쪽 픽셀과 오른쪽 픽셀 교환
						const auto pos = src_width - 1 - x;

						const uint8_t temp = row[x];
						row[x] = row[pos];
						row[pos] = temp;
					}
				}
			}

			void FlipHorizontal1(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				for (int32_t y = 0; y < src_height; ++y)
				{
					// 현재 행의 시작 주소 계산 및 캐싱
					const uint8_t* srcRow = src + y * src_width;
					uint8_t* dstRow = dst + y * src_width;

					// 좌우 반전: 각 행을 처리
					for (int32_t x = 0; x < src_width; ++x)
					{
						// 한쪽 픽셀 계산
						dstRow[src_width - 1 - x] = srcRow[x];
					}
				}
			}

			void FlipHorizontal2(uint8_t* src, int32_t src_width, int32_t src_height)
			{
				for (int32_t y = 0; y < src_height; ++y)
				{
					uint8_t* row = src + y * src_width * 2; // 행 시작 위치 캐싱
					for (int32_t x = 0; x < src_width / 2; ++x)
					{
						// 왼쪽 픽셀과 오른쪽 픽셀 교환
						const auto pos1 = (src_width - 1 - x) * 2;
						const auto pos2 = x * 2;
						uint8_t temp = row[pos2 + 0];
						row[pos2 + 0] = row[pos1 + 0];
						row[pos1 + 0] = temp;

						temp = row[pos2 + 1];
						row[pos2 + 1] = row[pos1 + 1];
						row[pos1 + 1] = temp;
					}
				}
			}

			void FlipHorizontal2(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				for (int32_t y = 0; y < src_height; ++y)
				{
					// 현재 행의 시작 주소 계산 및 캐싱
					const uint8_t* srcRow = src + y * src_width * 2;
					uint8_t* dstRow = dst + y * src_width * 2;

					// 좌우 반전: 각 행을 처리
					for (int32_t x = 0; x < src_width; ++x)
					{
						// 한쪽 픽셀 계산
						const auto pos1 = (src_width - 1 - x) * 2;
						const auto pos2 = x * 2;
						dstRow[pos1 + 0] = srcRow[pos2 + 0];
						dstRow[pos1 + 1] = srcRow[pos2 + 1];
					}
				}
			}

			void FlipHorizontal3(uint8_t* src, int32_t src_width, int32_t src_height)
			{
				for (int32_t y = 0; y < src_height; ++y)
				{
					uint8_t* row = src + y * src_width * 3; // 행 시작 위치 캐싱
					for (int32_t x = 0; x < src_width / 2; ++x)
					{
						// 왼쪽 픽셀과 오른쪽 픽셀 교환
						const auto pos1 = (src_width - 1 - x) * 3;
						const auto pos2 = x * 3;

						uint8_t temp = row[pos2 + 0];
						row[pos2 + 0] = row[pos1 + 0];
						row[pos1 + 0] = temp;

						temp = row[pos2 + 1];
						row[pos2 + 1] = row[pos1 + 1];
						row[pos1 + 1] = temp;

						temp = row[pos2 + 2];
						row[pos2 + 2] = row[pos1 + 2];
						row[pos1 + 2] = temp;
					}
				}
			}

			void FlipHorizontal3(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				for (int32_t y = 0; y < src_height; ++y)
				{
					// 현재 행의 시작 주소 계산 및 캐싱
					const uint8_t* srcRow = src + y * src_width * 3;
					uint8_t* dstRow = dst + y * src_width * 3;

					// 좌우 반전: 각 행을 처리
					for (int32_t x = 0; x < src_width; ++x)
					{
						// 한쪽 픽셀 계산
						const auto pos1 = (src_width - 1 - x) * 3;
						const auto pos2 = x * 3;

						dstRow[pos1 + 0] = srcRow[pos2 + 0];
						dstRow[pos1 + 1] = srcRow[pos2 + 1];
						dstRow[pos1 + 2] = srcRow[pos2 + 2];
					}
				}
			}

			void FlipHorizontal4(uint8_t* src, int32_t src_width, int32_t src_height)
			{
				for (int32_t y = 0; y < src_height; ++y)
				{
					uint8_t* row = src + y * src_width * 4; // 행 시작 위치 캐싱
					for (int32_t x = 0; x < src_width / 2; ++x)
					{
						// 왼쪽 픽셀과 오른쪽 픽셀 교환
						const auto pos1 = (src_width - 1 - x) * 4;
						const auto pos2 = x * 4;

						uint8_t temp = row[pos2 + 0];
						row[pos2 + 0] = row[pos1 + 0];
						row[pos1 + 0] = temp;

						temp = row[pos2 + 1];
						row[pos2 + 1] = row[pos1 + 1];
						row[pos1 + 1] = temp;

						temp = row[pos2 + 2];
						row[pos2 + 2] = row[pos1 + 2];
						row[pos1 + 2] = temp;

						temp = row[pos2 + 3];
						row[pos2 + 3] = row[pos1 + 3];
						row[pos1 + 3] = temp;
					}
				}
			}

			void FlipHorizontal4(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* dst)
			{
				for (int32_t y = 0; y < src_height; ++y)
				{
					// 현재 행의 시작 주소 계산 및 캐싱
					const uint8_t* srcRow = src + y * src_width * 4;
					uint8_t* dstRow = dst + y * src_width * 4;

					// 좌우 반전: 각 행을 처리
					for (int32_t x = 0; x < src_width; ++x)
					{
						// 한쪽 픽셀 계산
						const auto pos1 = (src_width - 1 - x) * 4;
						const auto pos2 = x * 4;

						dstRow[pos1 + 0] = srcRow[pos2 + 0];
						dstRow[pos1 + 1] = srcRow[pos2 + 1];
						dstRow[pos1 + 2] = srcRow[pos2 + 2];
						dstRow[pos1 + 3] = srcRow[pos2 + 3];
					}
				}
			}

			void FlipVertical(uint8_t* src, int32_t src_width, int32_t src_height, int32_t nChannels)
			{
				const int32_t rowSize = src_width * nChannels; // 한 행의 크기 (바이트 단위)
				const auto tempRow = new uint8_t[rowSize]; // 임시 행 저장 버퍼

				for (int32_t y = 0; y < src_height / 2; ++y) {
					uint8_t* topRow = src + y * rowSize; // 상단 행
					uint8_t* bottomRow = src + (src_height - 1 - y) * rowSize; // 하단 행

					// 두 행을 교환 (top ↔ bottom)
					std::memcpy(tempRow, topRow, rowSize);
					std::memcpy(topRow, bottomRow, rowSize);
					std::memcpy(bottomRow, tempRow, rowSize);
				}

				delete[] tempRow; // 메모리 해제
			}

			void FlipVertical(uint8_t* src, int32_t src_width, int32_t src_height, int32_t nChannels, const uint8_t* dst)
			{
				const int32_t rowSize = src_width * nChannels; // 한 행의 크기 (바이트 단위)

				for (int32_t y = 0; y < src_height; ++y) {
					uint8_t* topRow = src + y * rowSize; // 상단 행
					const uint8_t* bottomRow = dst + (src_height - 1 - y) * rowSize; // 하단 행

					// 두 행을 교환 (top ↔ bottom)
					std::memcpy(topRow, bottomRow, rowSize);
				}
			}

			void SplitChannels(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* ch1, uint8_t* ch2)
			{
				const uint32_t max = src_height * src_width * 2;

				for (uint32_t pos = 0, dst_pos = 0; pos < max; pos += 2, dst_pos++)
				{
					ch1[dst_pos] = src[pos + 0];
					ch2[dst_pos] = src[pos + 1];
				}
			}

			void SplitChannels(const uint8_t* src, int32_t src_width, int32_t src_height, uint8_t* ch1, uint8_t* ch2, uint8_t* ch3)
			{
				const uint32_t max = src_height * src_width * 3;

				for (uint32_t pos = 0, dst_pos = 0; pos < max; pos += 3, dst_pos++)
				{
					ch1[dst_pos] = src[pos + 0];
					ch2[dst_pos] = src[pos + 1];
					ch3[dst_pos] = src[pos + 2];
				}
			}

			void MergeChannels(uint8_t* dst, int32_t width, int32_t height, const uint8_t* ch1, const uint8_t* ch2)
			{
				const uint32_t max = height * width;

				for (uint32_t pos = 0, dst_pos = 0; pos < max; pos++, dst_pos += 2)
				{
					dst[dst_pos + 0] = ch1[pos];
					dst[dst_pos + 1] = ch2[pos];
				}
			}

			void MergeChannels(uint8_t* dst, int32_t width, int32_t height, const uint8_t* ch1, const uint8_t* ch2, const uint8_t* ch3)
			{
				const uint32_t max = height * width;

				for (uint32_t pos = 0, dst_pos = 0; pos < max; pos++, dst_pos += 3)
				{
					dst[dst_pos + 0] = ch1[pos];
					dst[dst_pos + 1] = ch2[pos];
					dst[dst_pos + 2] = ch3[pos];
				}
			}

			void Histogram(const uint8_t* src, int32_t width, int32_t height, uint32_t histo[256])
			{
				const uint32_t max = width * height;

				// count histo
				for (uint32_t pos = 0; pos < max; pos++)
					histo[src[pos]]++;
			}

			void Histogram(const uint8_t* src, int32_t width, int32_t height, float_t histo[256])
			{
				const float_t max = static_cast<float_t>(width) * static_cast<float_t>(height);

				// histoCount
				uint32_t iHisto[256] = {
					0,
				};

				// GetHisto
				Histogram(src, width, height, iHisto);

				// count histo
				for (int32_t pos = 0; pos < 256; pos++)
					histo[pos] = static_cast<float_t>(iHisto[pos]) / max;
			}

			void HistoEqualizer(const uint8_t* src, int32_t width, int32_t height, float_t histo[256], uint8_t* dst)
			{
				uint32_t pos;
				const uint32_t max = width * height;

				float_t cdf[256] = {
					0.0,
				};
				cdf[0] = histo[0];
				for (pos = 1; pos < 256; pos++)
				{
					cdf[pos] = cdf[pos - 1] + histo[pos];
				}

				for (pos = 0; pos < max; pos++)
				{
					dst[pos] = static_cast<uint8_t>(LIMIT(cdf[src[pos]] * 255));
				}
			}

			void Gaussian(const uint8_t* src, int32_t width, int32_t heigt, uint8_t* dst, float_t sigma)
			{
				float_t b[4];

				float_t q;

				if (sigma >= 2.5F)
				{
					q = 0.98711F * sigma - 0.96330F;
				}
				else if ((sigma >= 0.5F) && (sigma < 2.5F))
				{
					q = 3.97156f - 4.14554f * sqrt(static_cast<float_t>(1) - 0.26891f * sigma);
				}
				else
				{
					q = 0.1147705018520355224609375F;
				}

				const float_t q2 = q * q;
				const float_t q3 = q * q2;
				b[0] = (1.57825f + (2.44413f * q) + (1.4281f * q2) + (0.422205f * q3));
				b[1] = ((2.44413f * q) + (2.85619f * q2) + (1.26661f * q3));
				b[2] = (-((1.4281f * q2) + (1.26661f * q3)));
				b[3] = ((0.422205f * q3));
				const float_t B = 1.0f - ((b[1] + b[2] + b[3]) / b[0]);

				/*
				 * Papers:  "Recursive Implementation of the gaussian filter.",
				 *          Ian T. Young , Lucas J. Van Vliet, Signal Processing 44, Elsevier 1995.
				 * formula: 9a        forward filter
				 *          9b        backward filter
				 *          fig7      algorithm
				 */
				int32_t i, n;

				const int32_t bufsize = width > heigt ? width : heigt;

				auto* w1 = new float_t[bufsize + 3];
				auto* w2 = new float_t[bufsize + 3];
				auto* floatImage = new float_t[width * heigt];

				int32_t size = width - 1;
				for (int32_t posY = 0; posY < heigt; posY++)
				{
					/* forward pass */
					w1[0] = static_cast<float_t>(src[posY * width]) + 1.0f;
					w1[1] = static_cast<float_t>(src[posY * width]) + 1.0f;
					w1[2] = static_cast<float_t>(src[posY * width]) + 1.0f;

					const float_t invB = 1.0f / b[0];
					for (i = 0, n = 3; i <= size; i++, n++)
					{
						w1[n] = B * (static_cast<float_t>(src[i + posY * width]) + 1.0f) + (b[1] * w1[n - 1] + b[2] * w1[n - 2] + b[3] * w1[n - 3]) * invB;
					}

					/* backward pass */
					w2[size + 1] = w1[size + 3];
					w2[size + 2] = w1[size + 3];
					w2[size + 3] = w1[size + 3];
					floatImage[size - 2 + posY * width] = w1[size + 3];
					floatImage[size - 1 + posY * width] = w1[size + 3];
					floatImage[size - 0 + posY * width] = w1[size + 3];

					for (i = size, n = i; i >= 3; i--, n--)
					{
						w2[n] = floatImage[i - 3 + posY * width] = B * w1[n] + (b[1] * w2[n + 1] + b[2] * w2[n + 2] + b[3] * w2[n + 3]) * invB;
					}
				}

				size = heigt - 1;
				for (int32_t posX = 0; posX < width; posX++)
				{
					/* forward pass */
					w1[0] = floatImage[posX];
					w1[1] = floatImage[posX];
					w1[2] = floatImage[posX];

					const float_t invB = 1.0f / b[0];
					for (i = 0, n = 3; i <= size; i++, n++)
					{
						w1[n] = B * floatImage[i * width + posX] + (b[1] * w1[n - 1] + b[2] * w1[n - 2] + b[3] * w1[n - 3]) * invB;
					}

					/* backward pass */
					w2[size + 1] = w1[size + 3];
					w2[size + 2] = w1[size + 3];
					w2[size + 3] = w1[size + 3];

					floatImage[(size - 2) * width + posX] = w1[size + 3];
					floatImage[(size - 1) * width + posX] = w1[size + 3];
					floatImage[(size - 0) * width + posX] = w1[size + 3];

					for (i = size, n = i; i >= 3; i--, n--)
					{
						w2[n] = floatImage[(i - 3) * width + posX] = B * w1[n] + (b[1] * w2[n + 1] + b[2] * w2[n + 2] + b[3] * w2[n + 3]) * invB;
					}
				}

				// float_t to char
				for (i = 0; i < heigt * width; i++)
				{
					dst[i] = static_cast<uint8_t>(LIMIT(floatImage[i] - 1.0f));
				}

				delete[] w1;
				delete[] w2;
				delete[] floatImage;
			}

			void Translate(const uint8_t* src, int32_t width, int32_t height, int32_t nChannels, uint8_t* dst, int32_t mx, int32_t my)
			{
				if (width <= abs(mx) || height <= abs(my))
					return;

				int32_t sx = 0, sy = 0;
				int32_t dx = 0, dy = 0;

				if (mx < 0)
					sx = abs(mx);
				else
					dx = mx;

				if (my < 0)
					sy = abs(my);
				else
					dy = my;

				const int32_t offset = (width - abs(mx)) * nChannels;
				const int32_t _height = (height - abs(my));
				const int32_t widthStep = width * nChannels;

				int32_t spos = sy * widthStep + sx * nChannels;
				int32_t dpos = dy * widthStep + dx * nChannels;

				for (int32_t y = 0; y < _height; y++)
				{
					memcpy(&dst[dpos], &src[spos], offset);
					spos += widthStep;
					dpos += widthStep;
				}
			}

			void CalcRotateResultSize(const int32_t width, const int32_t height, const float_t angle, int32_t* nwidth, int32_t* nheight)
			{
				if (angle == 90.0f)
				{
					*nwidth = height;
					*nheight = width;
				}
				else if (angle == 180.0f)
				{
					*nwidth = width;
					*nheight = height;
				}
				else if (angle == 270.0f)
				{
					*nwidth = height;
					*nheight = width;
				}
				else
				{
					const float_t radians = angle / 180.0f * 3.14159265358979323846f;
					const auto cosine = (float_t)cos(radians);
					const auto sine = (float_t)sin(radians);

					const auto x1 = static_cast<int>(-height * sine);
					const auto y1 = static_cast<int>(height * cosine);
					const auto x2 = static_cast<int>(width * cosine - height * sine);
					const auto y2 = static_cast<int>(height * cosine + width * sine);
					const auto x3 = static_cast<int>(width * cosine);
					const auto y3 = static_cast<int>(width * sine);

					const int32_t minx = MIN(0, MIN(x1, MIN(x2, x3)));
					const int32_t miny = MIN(0, MIN(y1, MIN(y2, y3)));
					const int32_t maxx = MAX(0, MAX(x1, MAX(x2, x3)));
					const int32_t maxy = MAX(0, MAX(y1, MAX(y2, y3)));

					*nwidth = maxx - minx;
					*nheight = maxy - miny;
				}
			}

			void Rotate(uint8_t* src, int32_t width, int32_t height, int32_t nChannels, uint8_t* dst, float_t angle)
			{
				if (angle == 90.0f)
				{
					int32_t x, y;

					if (nChannels == 1)
					{
						for (y = 0; y < height; y++)
						{
							for (x = 0; x < width; x++)
							{
								dst[x * height + y] = src[y * width + x];
							}
						}
					}
					else if (nChannels == 3)
					{
						int32_t srcWidthStep = width * nChannels;
						int32_t dstWidthStep = height * nChannels;
						int32_t dstPos = 0;
						int32_t srcPos = 0;
						int32_t h = height - 1;
						for (y = 0; y < height; y++)
						{
							for (x = 0; x < width; x++)
							{
								dstPos = x * dstWidthStep + (h - y) * nChannels;
								srcPos = y * srcWidthStep + x * nChannels;

								dst[dstPos + 0] = src[srcPos + 0];
								dst[dstPos + 1] = src[srcPos + 1];
								dst[dstPos + 2] = src[srcPos + 2];
							}
						}
					}
					else if (nChannels == 4)
					{
						int32_t srcWidthStep = width * nChannels;
						int32_t dstWidthStep = height * nChannels;
						int32_t dstPos = 0;
						int32_t srcPos = 0;
						int32_t h = height - 1;
						for (y = 0; y < height; y++)
						{
							for (x = 0; x < width; x++)
							{
								dstPos = x * dstWidthStep + (h - y) * nChannels;
								srcPos = y * srcWidthStep + x * nChannels;

								dst[dstPos + 0] = src[srcPos + 0];
								dst[dstPos + 1] = src[srcPos + 1];
								dst[dstPos + 2] = src[srcPos + 2];
								dst[dstPos + 3] = src[srcPos + 3];
							}
						}
					}
				}
				else if (angle == 180.0f)
				{
					int32_t x, y;

					if (nChannels == 1)
					{
						for (y = 0; y < height; y++)
						{
							for (x = 0; x < width; x++)
							{
								dst[(height - 1 - y) * width + (width - 1 - x)] = src[y * width + x];
							}
						}
					}
					else if (nChannels == 3)
					{
						int32_t widthStep = width * nChannels;
						int32_t w = width - 1;
						int32_t h = height - 1;
						int32_t dstPos = 0;
						int32_t srcPos = 0;
						for (y = 0; y < height; y++)
						{
							for (x = 0; x < width; x++)
							{
								dstPos = (h - y) * widthStep + (w - x) * nChannels;
								srcPos = y * widthStep + x * nChannels;

								dst[dstPos + 0] = src[srcPos + 0];
								dst[dstPos + 1] = src[srcPos + 1];
								dst[dstPos + 2] = src[srcPos + 2];
							}
						}
					}
					else if (nChannels == 4)
					{
						int32_t widthStep = width * nChannels;
						int32_t w = width - 1;
						int32_t h = height - 1;
						int32_t dstPos = 0;
						int32_t srcPos = 0;
						for (y = 0; y < height; y++)
						{
							for (x = 0; x < width; x++)
							{
								dstPos = (h - y) * widthStep + (w - x) * nChannels;
								srcPos = y * widthStep + x * nChannels;

								dst[dstPos + 0] = src[srcPos + 0];
								dst[dstPos + 1] = src[srcPos + 1];
								dst[dstPos + 2] = src[srcPos + 2];
								dst[dstPos + 3] = src[srcPos + 3];
							}
						}
					}
				}
				else if (angle == 270.0f)
				{
					int32_t x, y;

					if (nChannels == 1)
					{
						for (y = 0; y < height; y++)
						{
							for (x = 0; x < width; x++)
							{
								dst[x * height + y] = src[y * width + x];
							}
						}
					}
					else if (nChannels == 3)
					{
						int32_t srcWidthStep = width * nChannels;
						int32_t dstWidthStep = height * nChannels;
						int32_t w = width - 1;
						int32_t dstPos = 0;
						int32_t srcPos = 0;
						for (y = 0; y < height; y++)
						{
							for (x = 0; x < width; x++)
							{
								dstPos = (w - x) * dstWidthStep + y * nChannels;
								srcPos = y * srcWidthStep + x * nChannels;

								dst[dstPos + 0] = src[srcPos + 0];
								dst[dstPos + 1] = src[srcPos + 1];
								dst[dstPos + 2] = src[srcPos + 2];
							}
						}
					}
					else if (nChannels == 4)
					{
						int32_t srcWidthStep = width * nChannels;
						int32_t dstWidthStep = height * nChannels;
						int32_t w = width - 1;
						int32_t dstPos = 0;
						int32_t srcPos = 0;
						for (y = 0; y < height; y++)
						{
							for (x = 0; x < width; x++)
							{
								dstPos = (w - x) * dstWidthStep + y * nChannels;
								srcPos = y * srcWidthStep + x * nChannels;

								dst[dstPos + 0] = src[srcPos + 0];
								dst[dstPos + 1] = src[srcPos + 1];
								dst[dstPos + 2] = src[srcPos + 2];
								dst[dstPos + 3] = src[srcPos + 3];
							}
						}
					}
				}
				else
				{
					float_t radians = angle / 180.0f * 3.14159265358979323846f;
					float_t cosine = cos(radians);
					float_t sine = sin(radians);

					auto x1 = static_cast<int>(-height * sine);
					auto y1 = static_cast<int>(height * cosine);
					auto x2 = static_cast<int>(width * cosine - height * sine);
					auto y2 = static_cast<int>(height * cosine + width * sine);
					auto x3 = static_cast<int>(width * cosine);
					auto y3 = static_cast<int>(width * sine);

					int32_t minx = MIN(0, MIN(x1, MIN(x2, x3)));
					int32_t miny = MIN(0, MIN(y1, MIN(y2, y3)));
					int32_t maxx = MAX(0, MAX(x1, MAX(x2, x3)));
					int32_t maxy = MAX(0, MAX(y1, MAX(y2, y3)));

					int32_t nwidth = maxx - minx;

					int32_t x, y;
					int32_t srcWidthStep = width * nChannels;
					int32_t dstWidthStep = nwidth * nChannels;

					double_t rx, ry, p, q, tb, tg, tr, ta;

					if (nChannels == 1)
					{
						double_t temp;
						for (y = miny; y < maxy; y++)
						{
							for (x = minx; x < maxx; x++)
							{
								rx = x * cosine + y * sine;
								ry = -x * sine + y * cosine;

								if (rx < 0 || rx > width - 1 || ry < 0 || ry > height - 1)
									continue;

								x1 = static_cast<int>(rx);
								y1 = static_cast<int>(ry);

								x2 = x1 + 1;
								if (x2 == width)
									x2 = width - 1;
								y2 = y1 + 1;
								if (y2 == height)
									y2 = height - 1;

								p = rx - x1;
								q = ry - y1;

								temp = (1.0 - p) * (1.0 - q) * src[y1 * srcWidthStep + x1] + p * (1.0 - q) * src[y1 * srcWidthStep + x2] + (1.0 - p) * q * src[y2 * srcWidthStep + x1] + p * q * src[y2 * srcWidthStep + x2];

								dst[(y - miny) * dstWidthStep + x - minx] = static_cast<uint8_t>(LIMIT(temp));
							}
						}
					}
					else if (nChannels == 3)
					{
						int32_t dstPos = 0;
						int32_t srcPos1 = 0;
						int32_t srcPos2 = 0;
						int32_t srcPos3 = 0;
						int32_t srcPos4 = 0;

						for (y = miny; y < maxy; y++)
						{
							for (x = minx; x < maxx; x++)
							{
								rx = x * cosine + y * sine;
								ry = -x * sine + y * cosine;

								if (rx < 0 || rx > width - 1 || ry < 0 || ry > height - 1)
									continue;

								x1 = static_cast<int>(rx);
								y1 = static_cast<int>(ry);

								x2 = x1 + 1;
								if (x2 == width)
									x2 = width - 1;
								y2 = y1 + 1;
								if (y2 == height)
									y2 = height - 1;

								p = rx - x1;
								q = ry - y1;

								srcPos1 = y1 * srcWidthStep + x1 * nChannels;
								srcPos2 = y1 * srcWidthStep + x2 * nChannels;
								srcPos3 = y2 * srcWidthStep + x1 * nChannels;
								srcPos4 = y2 * srcWidthStep + x2 * nChannels;

								tb = (1.0 - p) * (1.0 - q) * src[srcPos1 + 0] + p * (1.0 - q) * src[srcPos2 + 0] + (1.0 - p) * q * src[srcPos3 + 0] + p * q * src[srcPos4 + 0];

								tg = (1.0 - p) * (1.0 - q) * src[srcPos1 + 1] + p * (1.0 - q) * src[srcPos2 + 1] + (1.0 - p) * q * src[srcPos3 + 1] + p * q * src[srcPos4 + 1];

								tr = (1.0 - p) * (1.0 - q) * src[srcPos1 + 2] + p * (1.0 - q) * src[srcPos2 + 2] + (1.0 - p) * q * src[srcPos3 + 2] + p * q * src[srcPos4 + 2];

								dstPos = (y - miny) * dstWidthStep + (x - minx) * nChannels;
								dst[dstPos + 0] = static_cast<uint8_t>(LIMIT(tb));
								dst[dstPos + 1] = static_cast<uint8_t>(LIMIT(tg));
								dst[dstPos + 2] = static_cast<uint8_t>(LIMIT(tr));
							}
						}
					}
					else if (nChannels == 4)
					{
						int32_t dstPos = 0;
						int32_t srcPos1 = 0;
						int32_t srcPos2 = 0;
						int32_t srcPos3 = 0;
						int32_t srcPos4 = 0;

						for (y = miny; y < maxy; y++)
						{
							for (x = minx; x < maxx; x++)
							{
								rx = x * cosine + y * sine;
								ry = -x * sine + y * cosine;

								if (rx < 0 || rx > width - 1 || ry < 0 || ry > height - 1)
									continue;

								x1 = static_cast<int>(rx);
								y1 = static_cast<int>(ry);

								x2 = x1 + 1;
								if (x2 == width)
									x2 = width - 1;
								y2 = y1 + 1;
								if (y2 == height)
									y2 = height - 1;

								p = rx - x1;
								q = ry - y1;

								srcPos1 = y1 * srcWidthStep + x1 * nChannels;
								srcPos2 = y1 * srcWidthStep + x2 * nChannels;
								srcPos3 = y2 * srcWidthStep + x1 * nChannels;
								srcPos4 = y2 * srcWidthStep + x2 * nChannels;

								tb = (1.0 - p) * (1.0 - q) * src[srcPos1 + 0] + p * (1.0 - q) * src[srcPos2 + 0] + (1.0 - p) * q * src[srcPos3 + 0] + p * q * src[srcPos4 + 0];

								tg = (1.0 - p) * (1.0 - q) * src[srcPos1 + 1] + p * (1.0 - q) * src[srcPos2 + 1] + (1.0 - p) * q * src[srcPos3 + 1] + p * q * src[srcPos4 + 1];

								tr = (1.0 - p) * (1.0 - q) * src[srcPos1 + 2] + p * (1.0 - q) * src[srcPos2 + 2] + (1.0 - p) * q * src[srcPos3 + 2] + p * q * src[srcPos4 + 2];

								ta = (1.0 - p) * (1.0 - q) * src[srcPos1 + 3] + p * (1.0 - q) * src[srcPos2 + 3] + (1.0 - p) * q * src[srcPos3 + 3] + p * q * src[srcPos4 + 3];

								dstPos = (y - miny) * dstWidthStep + (x - minx) * nChannels;
								dst[dstPos + 0] = static_cast<uint8_t>(LIMIT(tb));
								dst[dstPos + 1] = static_cast<uint8_t>(LIMIT(tg));
								dst[dstPos + 2] = static_cast<uint8_t>(LIMIT(tr));
								dst[dstPos + 3] = static_cast<uint8_t>(LIMIT(ta));
							}
						}
					}
				}
			}

			void ResizeNearest(const uint8_t* src, const int32_t width, const int32_t height, const int32_t nChannels, uint8_t* dst, const int32_t dstWidth, const int32_t dstHeight)
			{
				int32_t x, y;
				const int32_t srcWidthStep = width * nChannels;
				const int32_t dstWidthStep = dstWidth * nChannels;

				const float_t dstWidthScale = 1.0f / static_cast<float_t>(dstWidth);
				const float_t dstHeightScale = 1.0f / static_cast<float_t>(dstHeight);

				int32_t sx, sy;

				if (nChannels == 1)
				{
					for (y = 0; y < dstHeight; y++)
					{
						sy = static_cast<int>(height * y * dstHeightScale);

						for (x = 0; x < dstWidth; x++)
						{
							sx = static_cast<int>(width * x * dstWidthScale);
							dst[y * dstWidthStep + x] = src[sy * srcWidthStep + sx];
						}
					}
				}
				else if (nChannels == 3)
				{
					for (y = 0; y < dstHeight; y++)
					{
						sy = static_cast<int>(height * y * dstHeightScale);

						for (x = 0; x < dstWidth; x++)
						{
							sx = static_cast<int>(width * x * dstWidthScale);

							const int32_t dstPos = y * dstWidthStep + x * nChannels;
							const int32_t srcPos = sy * srcWidthStep + sx * nChannels;
							dst[dstPos + 0] = src[srcPos + 0];
							dst[dstPos + 1] = src[srcPos + 1];
							dst[dstPos + 2] = src[srcPos + 2];
						}
					}
				}
				else if (nChannels == 4)
				{
					for (y = 0; y < dstHeight; y++)
					{
						sy = static_cast<int>(height * y * dstHeightScale);

						for (x = 0; x < dstWidth; x++)
						{
							sx = static_cast<int>(width * x * dstWidthScale);

							const int32_t dstPos = y * dstWidthStep + x * nChannels;
							const int32_t srcPos = sy * srcWidthStep + sx * nChannels;
							dst[dstPos + 0] = src[srcPos + 0];
							dst[dstPos + 1] = src[srcPos + 1];
							dst[dstPos + 2] = src[srcPos + 2];
							dst[dstPos + 3] = src[srcPos + 3];
						}
					}
				}
			}

			void ResizeBilinear(const uint8_t* src, const int32_t width, const int32_t height, const int32_t nChannels, uint8_t* dst, const int32_t dstWidth, const int32_t dstHeight)
			{
				int32_t x, y;
				const int32_t srcWidthStep = width * nChannels;
				const int32_t dstWidthStep = dstWidth * nChannels;

				const float_t dstWidthScale = 1.0f / static_cast<float_t>(dstWidth);
				const float_t dstHeightScale = 1.0f / static_cast<float_t>(dstHeight);

				int32_t x1, y1, x2, y2;
				double_t rx, ry, p, q, tb, tg, tr;

				if (nChannels == 1)
				{
					for (y = 0; y < dstHeight; y++)
					{
						for (x = 0; x < dstWidth; x++)
						{
							rx = static_cast<double_t>(width) * x * dstWidthScale;
							ry = static_cast<double_t>(height) * y * dstHeightScale;

							x1 = static_cast<int>(rx);
							y1 = static_cast<int>(ry);

							x2 = x1 + 1;
							if (x2 == width)
								x2 = width - 1;
							y2 = y1 + 1;
							if (y2 == height)
								y2 = height - 1;

							p = rx - x1;
							q = ry - y1;

							const double_t temp = (1.0 - p) * (1.0 - q) * src[y1 * srcWidthStep + x1] + p * (1.0 - q) * src[y1 * srcWidthStep + x2] + (1.0 - p) * q * src[y2 * srcWidthStep + x1] + p * q * src[y2 * srcWidthStep + x2];

							dst[y * dstWidthStep + x] = static_cast<uint8_t>(LIMIT(temp));
						}
					}
				}
				else if (nChannels == 3)
				{
					for (y = 0; y < dstHeight; y++)
					{
						for (x = 0; x < dstWidth; x++)
						{
							rx = static_cast<double_t>(width) * x * dstWidthScale;
							ry = static_cast<double_t>(height) * y * dstHeightScale;

							x1 = static_cast<int>(rx);
							y1 = static_cast<int>(ry);

							x2 = x1 + 1;
							if (x2 == width)
								x2 = width - 1;
							y2 = y1 + 1;
							if (y2 == height)
								y2 = height - 1;

							p = rx - x1;
							q = ry - y1;

							const int32_t srcPos1 = y1 * srcWidthStep + x1 * nChannels;
							const int32_t srcPos2 = y1 * srcWidthStep + x2 * nChannels;
							const int32_t srcPos3 = y2 * srcWidthStep + x1 * nChannels;
							const int32_t srcPos4 = y2 * srcWidthStep + x2 * nChannels;

							tb = (1.0 - p) * (1.0 - q) * src[srcPos1 + 0] + p * (1.0 - q) * src[srcPos2 + 0] + (1.0 - p) * q * src[srcPos3 + 0] + p * q * src[srcPos4 + 0];

							tg = (1.0 - p) * (1.0 - q) * src[srcPos1 + 1] + p * (1.0 - q) * src[srcPos2 + 1] + (1.0 - p) * q * src[srcPos3 + 1] + p * q * src[srcPos4 + 1];

							tr = (1.0 - p) * (1.0 - q) * src[srcPos1 + 2] + p * (1.0 - q) * src[srcPos2 + 2] + (1.0 - p) * q * src[srcPos3 + 2] + p * q * src[srcPos4 + 2];

							const int32_t dstPos = y * dstWidthStep + x * nChannels;
							dst[dstPos + 0] = static_cast<uint8_t>(LIMIT(tb));
							dst[dstPos + 1] = static_cast<uint8_t>(LIMIT(tg));
							dst[dstPos + 2] = static_cast<uint8_t>(LIMIT(tr));
						}
					}
				}
				else if (nChannels == 4)
				{
					for (y = 0; y < dstHeight; y++)
					{
						for (x = 0; x < dstWidth; x++)
						{
							rx = static_cast<double_t>(width) * x * dstWidthScale;
							ry = static_cast<double_t>(height) * y * dstHeightScale;

							x1 = static_cast<int>(rx);
							y1 = static_cast<int>(ry);

							x2 = x1 + 1;
							if (x2 == width)
								x2 = width - 1;
							y2 = y1 + 1;
							if (y2 == height)
								y2 = height - 1;

							p = rx - x1;
							q = ry - y1;

							const int32_t srcPos1 = y1 * srcWidthStep + x1 * nChannels;
							const int32_t srcPos2 = y1 * srcWidthStep + x2 * nChannels;
							const int32_t srcPos3 = y2 * srcWidthStep + x1 * nChannels;
							const int32_t srcPos4 = y2 * srcWidthStep + x2 * nChannels;

							tb = (1.0 - p) * (1.0 - q) * src[srcPos1 + 0] + p * (1.0 - q) * src[srcPos2 + 0] + (1.0 - p) * q * src[srcPos3 + 0] + p * q * src[srcPos4 + 0];

							tg = (1.0 - p) * (1.0 - q) * src[srcPos1 + 1] + p * (1.0 - q) * src[srcPos2 + 1] + (1.0 - p) * q * src[srcPos3 + 1] + p * q * src[srcPos4 + 1];

							tr = (1.0 - p) * (1.0 - q) * src[srcPos1 + 2] + p * (1.0 - q) * src[srcPos2 + 2] + (1.0 - p) * q * src[srcPos3 + 2] + p * q * src[srcPos4 + 2];

							const double_t ta = (1.0 - p) * (1.0 - q) * src[srcPos1 + 3] + p * (1.0 - q) * src[srcPos2 + 3] + (1.0 - p) * q * src[
								srcPos3 + 3] + p * q * src[srcPos4 + 3];

							const int32_t dstPos = y * dstWidthStep + x * nChannels;
							dst[dstPos + 0] = static_cast<uint8_t>(LIMIT(tb));
							dst[dstPos + 1] = static_cast<uint8_t>(LIMIT(tg));
							dst[dstPos + 2] = static_cast<uint8_t>(LIMIT(tr));
							dst[dstPos + 3] = static_cast<uint8_t>(LIMIT(ta));
						}
					}
				}
			}

			inline double_t CubicInterpolation(const double_t v1, const double_t v2, const double_t v3, const double_t v4, const double_t d)
			{
				const double_t p1 = v2;
				const double_t p2 = -v1 + v3;
				const double_t p3 = 2 * (v1 - v2) + v3 - v4;
				const double_t p4 = -v1 + v2 - v3 + v4;

				const double_t v = p1 + d * (p2 + d * (p3 + d * p4));

				return v;
			}

			void ResizeCubic(const uint8_t* src, int32_t width, int32_t height, int32_t nChannels, uint8_t* dst, int32_t dstWidth, int32_t dstHeight)
			{
				int32_t x, y;
				int32_t srcWidthStep = width * nChannels;
				int32_t dstWidthStep = dstWidth * nChannels;

				float_t dstWidthScale = 1.0f / static_cast<float_t>(dstWidth);
				float_t dstHeightScale = 1.0f / static_cast<float_t>(dstHeight);

				int32_t x1 = 0, x2 = 0, x3 = 0, x4 = 0;
				int32_t y1 = 0, y2 = 0, y3 = 0, y4 = 0;

				double_t v1, v2, v3, v4, v, vb, vg, vr, va;
				double_t rx, ry, p, q;

				if (nChannels == 1)
				{
					for (y = 0; y < dstHeight; y++)
					{
						for (x = 0; x < dstWidth; x++)
						{
							rx = static_cast<double_t>(width) * x * dstWidthScale;
							ry = static_cast<double_t>(height) * y * dstHeightScale;

							x2 = static_cast<int>(rx);
							x1 = x2 - 1;
							if (x1 < 0)
								x1 = 0;
							x3 = x2 + 1;
							if (x3 >= width)
								x3 = width - 1;
							x4 = x2 + 2;
							if (x4 >= width)
								x4 = width - 1;
							p = rx - x2;

							y2 = static_cast<int>(ry);
							y1 = y2 - 1;
							if (y1 < 0)
								y1 = 0;
							y3 = y2 + 1;
							if (y3 >= height)
								y3 = height - 1;
							y4 = y2 + 2;
							if (y4 >= height)
								y4 = height - 1;
							q = ry - y2;

							v1 = CubicInterpolation(src[y1 * srcWidthStep + x1], src[y1 * srcWidthStep + x2], src[y1 * srcWidthStep + x3], src[y1 * srcWidthStep + x4], p);
							v2 = CubicInterpolation(src[y2 * srcWidthStep + x1], src[y2 * srcWidthStep + x2], src[y2 * srcWidthStep + x3], src[y2 * srcWidthStep + x4], p);
							v3 = CubicInterpolation(src[y3 * srcWidthStep + x1], src[y3 * srcWidthStep + x2], src[y3 * srcWidthStep + x3], src[y3 * srcWidthStep + x4], p);
							v4 = CubicInterpolation(src[y4 * srcWidthStep + x1], src[y4 * srcWidthStep + x2], src[y4 * srcWidthStep + x3], src[y4 * srcWidthStep + x4], p);
							v = CubicInterpolation(v1, v2, v3, v4, q);

							dst[y * dstWidth + x] = static_cast<uint8_t>(LIMIT(v));
						}
					}
				}
				else if (nChannels == 3)
				{
					int32_t dstPos = 0;
					int32_t srcPos11 = 0;
					int32_t srcPos12 = 0;
					int32_t srcPos13 = 0;
					int32_t srcPos14 = 0;
					int32_t srcPos21 = 0;
					int32_t srcPos22 = 0;
					int32_t srcPos23 = 0;
					int32_t srcPos24 = 0;
					int32_t srcPos31 = 0;
					int32_t srcPos32 = 0;
					int32_t srcPos33 = 0;
					int32_t srcPos34 = 0;
					int32_t srcPos41 = 0;
					int32_t srcPos42 = 0;
					int32_t srcPos43 = 0;
					int32_t srcPos44 = 0;

					for (y = 0; y < dstHeight; y++)
					{
						for (x = 0; x < dstWidth; x++)
						{
							rx = static_cast<double_t>(width) * x * dstWidthScale;
							ry = static_cast<double_t>(height) * y * dstHeightScale;

							x2 = static_cast<int>(rx);
							x1 = x2 - 1;
							if (x1 < 0)
								x1 = 0;
							x3 = x2 + 1;
							if (x3 >= width)
								x3 = width - 1;
							x4 = x2 + 2;
							if (x4 >= width)
								x4 = width - 1;
							p = rx - x2;

							y2 = static_cast<int>(ry);
							y1 = y2 - 1;
							if (y1 < 0)
								y1 = 0;
							y3 = y2 + 1;
							if (y3 >= height)
								y3 = height - 1;
							y4 = y2 + 2;
							if (y4 >= height)
								y4 = height - 1;
							q = ry - y2;

							srcPos11 = y1 * srcWidthStep + x1 * nChannels;
							srcPos12 = y1 * srcWidthStep + x2 * nChannels;
							srcPos13 = y1 * srcWidthStep + x3 * nChannels;
							srcPos14 = y1 * srcWidthStep + x4 * nChannels;

							srcPos21 = y2 * srcWidthStep + x1 * nChannels;
							srcPos22 = y2 * srcWidthStep + x2 * nChannels;
							srcPos23 = y2 * srcWidthStep + x3 * nChannels;
							srcPos24 = y2 * srcWidthStep + x4 * nChannels;

							srcPos31 = y3 * srcWidthStep + x1 * nChannels;
							srcPos32 = y3 * srcWidthStep + x2 * nChannels;
							srcPos33 = y3 * srcWidthStep + x3 * nChannels;
							srcPos34 = y3 * srcWidthStep + x4 * nChannels;

							srcPos41 = y4 * srcWidthStep + x1 * nChannels;
							srcPos42 = y4 * srcWidthStep + x2 * nChannels;
							srcPos43 = y4 * srcWidthStep + x3 * nChannels;
							srcPos44 = y4 * srcWidthStep + x4 * nChannels;

							v1 = CubicInterpolation(src[srcPos11 + 0], src[srcPos12 + 0], src[srcPos13 + 0], src[srcPos14 + 0], p);
							v2 = CubicInterpolation(src[srcPos21 + 0], src[srcPos22 + 0], src[srcPos23 + 0], src[srcPos24 + 0], p);
							v3 = CubicInterpolation(src[srcPos31 + 0], src[srcPos32 + 0], src[srcPos33 + 0], src[srcPos34 + 0], p);
							v4 = CubicInterpolation(src[srcPos41 + 0], src[srcPos42 + 0], src[srcPos43 + 0], src[srcPos44 + 0], p);
							vb = CubicInterpolation(v1, v2, v3, v4, q);

							v1 = CubicInterpolation(src[srcPos11 + 1], src[srcPos12 + 1], src[srcPos13 + 1], src[srcPos14 + 1], p);
							v2 = CubicInterpolation(src[srcPos21 + 1], src[srcPos22 + 1], src[srcPos23 + 1], src[srcPos24 + 1], p);
							v3 = CubicInterpolation(src[srcPos31 + 1], src[srcPos32 + 1], src[srcPos33 + 1], src[srcPos34 + 1], p);
							v4 = CubicInterpolation(src[srcPos41 + 1], src[srcPos42 + 1], src[srcPos43 + 1], src[srcPos44 + 1], p);
							vg = CubicInterpolation(v1, v2, v3, v4, q);

							v1 = CubicInterpolation(src[srcPos11 + 2], src[srcPos12 + 2], src[srcPos13 + 2], src[srcPos14 + 2], p);
							v2 = CubicInterpolation(src[srcPos21 + 2], src[srcPos22 + 2], src[srcPos23 + 2], src[srcPos24 + 2], p);
							v3 = CubicInterpolation(src[srcPos31 + 2], src[srcPos32 + 2], src[srcPos33 + 2], src[srcPos34 + 2], p);
							v4 = CubicInterpolation(src[srcPos41 + 2], src[srcPos42 + 2], src[srcPos43 + 2], src[srcPos44 + 2], p);
							vr = CubicInterpolation(v1, v2, v3, v4, q);

							dstPos = y * dstWidthStep + x * nChannels;
							dst[dstPos + 0] = static_cast<uint8_t>(LIMIT(vb));
							dst[dstPos + 1] = static_cast<uint8_t>(LIMIT(vg));
							dst[dstPos + 2] = static_cast<uint8_t>(LIMIT(vr));
						}
					}
				}
				else if (nChannels == 4)
				{
					int32_t dstPos = 0;
					int32_t srcPos11 = 0;
					int32_t srcPos12 = 0;
					int32_t srcPos13 = 0;
					int32_t srcPos14 = 0;
					int32_t srcPos21 = 0;
					int32_t srcPos22 = 0;
					int32_t srcPos23 = 0;
					int32_t srcPos24 = 0;
					int32_t srcPos31 = 0;
					int32_t srcPos32 = 0;
					int32_t srcPos33 = 0;
					int32_t srcPos34 = 0;
					int32_t srcPos41 = 0;
					int32_t srcPos42 = 0;
					int32_t srcPos43 = 0;
					int32_t srcPos44 = 0;

					for (y = 0; y < dstHeight; y++)
					{
						for (x = 0; x < dstWidth; x++)
						{
							rx = static_cast<double_t>(width) * x * dstWidthScale;
							ry = static_cast<double_t>(height) * y * dstHeightScale;

							x2 = static_cast<int>(rx);
							x1 = x2 - 1;
							if (x1 < 0)
								x1 = 0;
							x3 = x2 + 1;
							if (x3 >= width)
								x3 = width - 1;
							x4 = x2 + 2;
							if (x4 >= width)
								x4 = width - 1;
							p = rx - x2;

							y2 = static_cast<int>(ry);
							y1 = y2 - 1;
							if (y1 < 0)
								y1 = 0;
							y3 = y2 + 1;
							if (y3 >= height)
								y3 = height - 1;
							y4 = y2 + 2;
							if (y4 >= height)
								y4 = height - 1;
							q = ry - y2;

							srcPos11 = y1 * srcWidthStep + x1 * nChannels;
							srcPos12 = y1 * srcWidthStep + x2 * nChannels;
							srcPos13 = y1 * srcWidthStep + x3 * nChannels;
							srcPos14 = y1 * srcWidthStep + x4 * nChannels;

							srcPos21 = y2 * srcWidthStep + x1 * nChannels;
							srcPos22 = y2 * srcWidthStep + x2 * nChannels;
							srcPos23 = y2 * srcWidthStep + x3 * nChannels;
							srcPos24 = y2 * srcWidthStep + x4 * nChannels;

							srcPos31 = y3 * srcWidthStep + x1 * nChannels;
							srcPos32 = y3 * srcWidthStep + x2 * nChannels;
							srcPos33 = y3 * srcWidthStep + x3 * nChannels;
							srcPos34 = y3 * srcWidthStep + x4 * nChannels;

							srcPos41 = y4 * srcWidthStep + x1 * nChannels;
							srcPos42 = y4 * srcWidthStep + x2 * nChannels;
							srcPos43 = y4 * srcWidthStep + x3 * nChannels;
							srcPos44 = y4 * srcWidthStep + x4 * nChannels;

							v1 = CubicInterpolation(src[srcPos11 + 0], src[srcPos12 + 0], src[srcPos13 + 0], src[srcPos14 + 0], p);
							v2 = CubicInterpolation(src[srcPos21 + 0], src[srcPos22 + 0], src[srcPos23 + 0], src[srcPos24 + 0], p);
							v3 = CubicInterpolation(src[srcPos31 + 0], src[srcPos32 + 0], src[srcPos33 + 0], src[srcPos34 + 0], p);
							v4 = CubicInterpolation(src[srcPos41 + 0], src[srcPos42 + 0], src[srcPos43 + 0], src[srcPos44 + 0], p);
							vb = CubicInterpolation(v1, v2, v3, v4, q);

							v1 = CubicInterpolation(src[srcPos11 + 1], src[srcPos12 + 1], src[srcPos13 + 1], src[srcPos14 + 1], p);
							v2 = CubicInterpolation(src[srcPos21 + 1], src[srcPos22 + 1], src[srcPos23 + 1], src[srcPos24 + 1], p);
							v3 = CubicInterpolation(src[srcPos31 + 1], src[srcPos32 + 1], src[srcPos33 + 1], src[srcPos34 + 1], p);
							v4 = CubicInterpolation(src[srcPos41 + 1], src[srcPos42 + 1], src[srcPos43 + 1], src[srcPos44 + 1], p);
							vg = CubicInterpolation(v1, v2, v3, v4, q);

							v1 = CubicInterpolation(src[srcPos11 + 2], src[srcPos12 + 2], src[srcPos13 + 2], src[srcPos14 + 2], p);
							v2 = CubicInterpolation(src[srcPos21 + 2], src[srcPos22 + 2], src[srcPos23 + 2], src[srcPos24 + 2], p);
							v3 = CubicInterpolation(src[srcPos31 + 2], src[srcPos32 + 2], src[srcPos33 + 2], src[srcPos34 + 2], p);
							v4 = CubicInterpolation(src[srcPos41 + 2], src[srcPos42 + 2], src[srcPos43 + 2], src[srcPos44 + 2], p);
							vr = CubicInterpolation(v1, v2, v3, v4, q);

							v1 = CubicInterpolation(src[srcPos11 + 3], src[srcPos12 + 3], src[srcPos13 + 3], src[srcPos14 + 3], p);
							v2 = CubicInterpolation(src[srcPos21 + 3], src[srcPos22 + 3], src[srcPos23 + 3], src[srcPos24 + 3], p);
							v3 = CubicInterpolation(src[srcPos31 + 3], src[srcPos32 + 3], src[srcPos33 + 3], src[srcPos34 + 3], p);
							v4 = CubicInterpolation(src[srcPos41 + 3], src[srcPos42 + 3], src[srcPos43 + 3], src[srcPos44 + 3], p);
							va = CubicInterpolation(v1, v2, v3, v4, q);

							dstPos = y * dstWidthStep + x * nChannels;
							dst[dstPos + 0] = static_cast<uint8_t>(LIMIT(vb));
							dst[dstPos + 1] = static_cast<uint8_t>(LIMIT(vg));
							dst[dstPos + 2] = static_cast<uint8_t>(LIMIT(vr));
							dst[dstPos + 3] = static_cast<uint8_t>(LIMIT(va));
						}
					}
				}
			}

			void Extract(const uint8_t* src, const int32_t nChannels, const int32_t srcStepLine, int32_t extractX, int32_t extractY, int32_t extractWidth, int32_t extractHeight, uint8_t* dst, const int32_t dstStepLine, const int32_t dstX, const int32_t dstY)
			{
				const uint8_t* pSrc = &src[extractY * srcStepLine + extractX * nChannels];
				uint8_t* pDst = &dst[dstY * dstStepLine + dstX * nChannels];
				const int32_t nCopySize = extractWidth * nChannels;

				for (int32_t y = 0; y < extractHeight; y++)
				{
					memcpy(pDst, pSrc, nCopySize);
					pSrc += srcStepLine;
					pDst += dstStepLine;
				}
			}

			void Dilation(const uint8_t* src, const int32_t width, const int32_t height, uint8_t* dst, const int32_t mask_x, const int32_t mask_y)
			{
				int32_t cx, cy, loop_x, loop_y;

				const int32_t off_x = mask_x / 2;
				const int32_t off_y = mask_y / 2;

				for (int32_t y = off_y; y < height - off_y; y++)
				{
					for (int32_t x = off_x; x < width - off_x; x++)
					{
						uint8_t temp = src[y * width + x];

						for (cy = y - off_y, loop_y = 0; loop_y < mask_y; loop_y++, cy++)
						{
							for (cx = x - off_x, loop_x = 0; loop_x < mask_x; loop_x++, cx++)
							{
								if (temp < src[cy * width + cx])
									temp = src[cy * width + cx];
							}
						}

						dst[y * width + x] = temp;
					}
				}
			}

			void Erosion(const uint8_t* src, const int32_t width, const int32_t height, uint8_t* dst, const int32_t mask_x, const int32_t mask_y)
			{
				int32_t cx, cy, loop_x, loop_y;

				const int32_t off_x = mask_x / 2;
				const int32_t off_y = mask_y / 2;
				for (int32_t y = off_y; y < height - off_x; y++)
				{
					for (int32_t x = off_x; x < width - off_y; x++)
					{
						uint8_t temp = src[y * width + x];

						for (cy = y - off_y, loop_y = 0; loop_y < mask_y; loop_y++, cy++)
						{
							for (cx = x - off_x, loop_x = 0; loop_x < mask_x; loop_x++, cx++)
							{
								if (temp > src[cy * width + cx])
									temp = src[cy * width + cx];
							}
						}

						dst[y * width + x] = temp;
					}
				}
			}

		}
	}
}