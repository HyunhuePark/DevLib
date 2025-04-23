#pragma once

#include "../Base/DevLibTypes.hpp"

namespace DevLib
{
	namespace Graphics
	{
		class CImageColor
		{
		public:

			CImageColor();
			CImageColor(uint8_t blue, uint8_t green, uint8_t red, uint8_t alpha = 255);
			CImageColor(uint8_t GrayLevel);
			~CImageColor();

			static CImageColor GetLUTColor(uint32_t data, int nMax = 0xFFFF);

			uint8_t& b() { return m_data[0]; }
			uint8_t& g() { return m_data[1]; }
			uint8_t& r() { return m_data[2]; }
			uint8_t& a() { return m_data[3]; }

			uint8_t* data() { return m_data; }

			// Operator 연사자 정의
			CImageColor& operator=(const uint8_t& color);
			bool operator==(const CImageColor& color) const;
			bool operator!=(const CImageColor& color) const;
			CImageColor operator+(const uint8_t& color);
			CImageColor operator*(const double_t& alpha);

		private:
			uint8_t m_data[4]{};	// BGRA
		};

		// Simple Color
		const CImageColor Black = CImageColor(0, 0, 0);
		const CImageColor Blue = CImageColor(255, 0, 0);
		const CImageColor Green = CImageColor(0, 255, 0);
		const CImageColor Cyan = CImageColor(255, 255, 0);
		const CImageColor Red = CImageColor(0, 0, 255);
		const CImageColor Magenta = CImageColor(255, 0, 255);
		const CImageColor Yellow = CImageColor(0, 255, 255);
		const CImageColor White = CImageColor(255, 255, 255);


	}
}