#include "../../include/Graphics/CImageColor.hpp"
#include "pseudoLUT.hpp"

#ifdef _MSC_VER
#else // Linux
#include <cstring>
#endif

namespace DevLib
{
	namespace Graphics
	{
		CImageColor::CImageColor() = default;
		CImageColor::CImageColor(uint8_t blue, uint8_t green, uint8_t red, uint8_t alpha)
		{
			b() = blue;
			g() = green;
			r() = red;
			a() = alpha;
		}

		CImageColor::CImageColor(uint8_t GrayLevel)
		{
			b() = GrayLevel;
			g() = GrayLevel;
			r() = GrayLevel;
			a() = 255;
		}
		CImageColor::~CImageColor() = default;

		CImageColor CImageColor::GetLUTColor(uint32_t data, int nMax /*= 0xFFFF*/)
		{
			const float PseudoStep = 512.0f / nMax;
			const auto pos = static_cast<uint32_t>(PseudoStep * data);
			return { __table_pseudo[pos][0], __table_pseudo[pos][1], __table_pseudo[pos][2] };
		}

		CImageColor CImageColor::operator+(const uint8_t& color)
		{
			return { static_cast<uint8_t>(b() + color), static_cast<uint8_t>(g() + color), static_cast<uint8_t>(r() + color) };
		}

		CImageColor CImageColor::operator*(const double_t& alpha)
		{
			return { static_cast<uint8_t>(b() * alpha), static_cast<uint8_t>(g() * alpha), static_cast<uint8_t>(r() * alpha) };
		}

		bool CImageColor::operator!=(const CImageColor& color) const
		{
			if (m_data[0] != color.m_data[0] ||
				m_data[1] != color.m_data[1] ||
				m_data[2] != color.m_data[2] ||
				m_data[3] != color.m_data[3]) return true;
			return false;
		}

		bool CImageColor::operator==(const CImageColor& color) const
		{
			if (m_data[0] == color.m_data[0] &&
				m_data[1] == color.m_data[1] &&
				m_data[2] == color.m_data[2] &&
				m_data[3] == color.m_data[3]) return true;
			return false;
		}

		CImageColor& CImageColor::operator=(const uint8_t& color)
		{
			m_data[0] = m_data[1] = m_data[2] = color;

			return *this;
		}

	}
}