#pragma once

#include <vector>

#include "CAbstractSerialization.hpp"
#include "../../Base/CPointCloud.hpp"
#include "../../Graphics/CImageObject.hpp"
#include "../../Graphics/CPng.hpp"
#include "../../Graphics/CJpeg.hpp"

namespace DevLib {
	namespace Routine
	{
		class CSerializer : public std::vector<uint8_t>
		{
		public:
			CSerializer();

			// Operation
			void operator<<(const char& _data);
			void operator<<(const int8_t& _data);
			void operator<<(const uint8_t& _data);
			void operator<<(const int16_t& _data);
			void operator<<(const uint16_t& _data);
			void operator<<(const int32_t& _data);
			void operator<<(const uint32_t& _data);
			void operator<<(const int64_t& _data);
			void operator<<(const uint64_t& _data);
#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
			void operator<<(const size_t& data);
#endif
			void operator<<(const float_t& _data);
			void operator<<(const double_t& _data);
			void operator<<(StBitField8_t& _data);
			void operator<<(StBitField16_t& _data);
			void operator<<(StBitField32_t& _data);
			void operator<<(StBitField64_t& _data);
			void operator<<(const std::string& _data);
			void operator<<(const Graphics::CImageObject& _data);
			void operator<<(Graphics::CJpeg& _data);
			void operator<<(Graphics::CPng& _data);
			void operator<<(CAbstractSerialization& _data);

			template <typename Type> void operator<<(const Eigen::Matrix<Type, 2, 1, 0>& _data);
			template <typename Type> void operator<<(const Eigen::Matrix<Type, 3, 1, 0>& _data);
			template <typename Type> void operator<<(const Eigen::Matrix<Type, 4, 1, 0>& _data);
			template <typename Type> void operator<<(CPointCloud<Type>& _data);
			template <typename Type> void operator<<(std::vector<Type>& _data);

		protected:
			std::vector<uint8_t> memory;
		};

		inline CSerializer::CSerializer()
		{
			clear();
		}

		inline void CSerializer::operator<<(const char& _data)
		{
			const uint8_t* pData = reinterpret_cast<uint8_t*>(const_cast<char*>(&_data));
			push_back(*pData);
		}

		inline void CSerializer::operator<<(const int8_t& _data)
		{
			const uint8_t* pData = reinterpret_cast<uint8_t*>(const_cast<int8_t*>(&_data));
			push_back(*pData);
		}

		inline void CSerializer::operator<<(const uint8_t& _data)
		{
			const uint8_t* pData = &_data;
			push_back(*pData);
		}

		inline void CSerializer::operator<<(const int16_t& _data)
		{
			const uint8_t* pData = reinterpret_cast<uint8_t*>(const_cast<int16_t*>(&_data));
			for (size_t i = 0; i < sizeof _data; i++)
			{
				push_back(pData[i]);
			}
		}

		inline void CSerializer::operator<<(const uint16_t& _data)
		{
			const uint8_t* pData = reinterpret_cast<uint8_t*>(const_cast<uint16_t*>(&_data));
			for (size_t i = 0; i < sizeof _data; i++)
			{
				push_back(pData[i]);
			}
		}

		inline void CSerializer::operator<<(const int32_t& _data)
		{
			const uint8_t* pData = reinterpret_cast<uint8_t*>(const_cast<int32_t*>(&_data));
			for (size_t i = 0; i < sizeof _data; i++)
			{
				push_back(pData[i]);
			}
		}

		inline void CSerializer::operator<<(const uint32_t& _data)
		{
			const uint8_t* pData = reinterpret_cast<uint8_t*>(const_cast<uint32_t*>(&_data));
			for (size_t i = 0; i < sizeof _data; i++)
			{
				push_back(pData[i]);
			}
		}

		inline void CSerializer::operator<<(const int64_t& _data)
		{
			const uint8_t* pData = reinterpret_cast<uint8_t*>(const_cast<int64_t*>(&_data));
			for (size_t i = 0; i < sizeof _data; i++)
			{
				push_back(pData[i]);
			}
		}

		inline void CSerializer::operator<<(const uint64_t& _data)
		{
			const uint8_t* pData = reinterpret_cast<uint8_t*>(const_cast<uint64_t*>(&_data));
			for (size_t i = 0; i < sizeof _data; i++)
			{
				push_back(pData[i]);
			}
		}

#if defined(_MSC_VER)
#elif defined(__linux__) // Linux
		inline void CSerializer::operator<<(const size_t& data)
		{
			const uint8_t* pData = reinterpret_cast<uint8_t*>(const_cast<size_t*>(&data));
			for (size_t i = 0; i < sizeof data; i++)
			{
				push_back(pData[i]);
			}
		}
#endif

		inline void CSerializer::operator<<(const float_t& _data)
		{
			const uint8_t* pData = reinterpret_cast<uint8_t*>(const_cast<float_t*>(&_data));
			for (size_t i = 0; i < sizeof _data; i++)
			{
				push_back(pData[i]);
			}
		}

		inline void CSerializer::operator<<(const double_t& _data)
		{
			const uint8_t* pData = reinterpret_cast<uint8_t*>(const_cast<double_t*>(&_data));
			for (size_t i = 0; i < sizeof _data; i++)
			{
				push_back(pData[i]);
			}
		}

		inline void CSerializer::operator<<(StBitField8_t& _data)
		{
			*this << _data.data();
		}

		inline void CSerializer::operator<<(StBitField16_t& _data)
		{
			*this << _data.data();
		}

		inline void CSerializer::operator<<(StBitField32_t& _data)
		{
			*this << _data.data();
		}

		inline void CSerializer::operator<<(StBitField64_t& _data)
		{
			*this << _data.data();
		}

		inline void CSerializer::operator<<(const std::string& _data)
		{
			// insert Size
			*this << _data.length();

			for (const char c : _data) push_back(c);
		}

		inline void CSerializer::operator<<(const Graphics::CImageObject& _data)
		{
			*this << _data.GetWidth();
			*this << _data.GetHeight();
			*this << static_cast<uint32_t>(_data.GetImageType());
			const auto* pImg = _data.GetImageConst();
			for (size_t i = 0; i < _data.GetSize(); i++) *this << pImg[i];
		}

		inline void CSerializer::operator<<(Graphics::CJpeg& _data)
		{
			*this << _data.GetWidth();
			*this << _data.GetHeight();
			*this << static_cast<uint32_t>(_data.GetChannels());
			*this << _data.JpegData();
		}

		inline void CSerializer::operator<<(Graphics::CPng& _data)
		{
			*this << _data.GetWidth();
			*this << _data.GetHeight();
			*this << static_cast<uint32_t>(_data.GetChannels());
			*this << _data.PngData();
		}

		inline void CSerializer::operator<<(CAbstractSerialization& _data)
		{
			_data.Serialization(*this);
		}

		template <typename Type>
		void CSerializer::operator<<(const Eigen::Matrix<Type, 2, 1, 0>& _data)
		{
			*this << _data.x();
			*this << _data.y();
		}

		template <typename Type>
		void CSerializer::operator<<(const Eigen::Matrix<Type, 3, 1, 0>& _data)
		{
			*this << _data.x();
			*this << _data.y();
			*this << _data.z();
		}

		template <typename Type>
		void CSerializer::operator<<(const Eigen::Matrix<Type, 4, 1, 0>& _data)
		{
			*this << _data.x();
			*this << _data.y();
			*this << _data.z();
			*this << _data.w();
		}

		template <typename Type>
		void CSerializer::operator<<(CPointCloud<Type>& _data)
		{
			// insert Size
			*this << _data.size();
			const size_t pos = size();
			const size_t memSize = _data.size() * _data[0].rows() * sizeof(std::remove_reference_t<decltype(_data[0].x())>);
			resize(pos + memSize);
			memcpy(&data()[pos], &_data[0], memSize);
		}

		template <typename Type>
		void CSerializer::operator<<(std::vector<Type>& _data)
		{
			// insert Size
			*this << _data.size();

			for (auto& element : _data)
			{
				*this << element;
			}
		}

	}
}