#pragma once

#include "CAbstractSerialization.hpp"
#include "../../Base/CPointCloud.hpp"
#include "../../Graphics/CImageObject.hpp"
#include "../../Graphics/CPng.hpp"
#include "../../Graphics/CJpeg.hpp"

namespace DevLib {
	namespace Routine
	{
		class CDeSerializer
		{
		public:
			CDeSerializer(void_ptr pData);

			size_t size() const;
			void reset(void_ptr pData);
			void_ptr data() const;
			void_ptr cur() const;
			void_ptr at(size_t pos) const;

			void operator>>(char& data);
			void operator>>(int8_t& data);
			void operator>>(uint8_t& data);
			void operator>>(int16_t& data);
			void operator>>(uint16_t& data);
			void operator>>(int32_t& data);
			void operator>>(uint32_t& data);
			void operator>>(int64_t& data);
			void operator>>(uint64_t& data);

#if defined( _MSC_VER )
#elif defined(__linux__) // Linux
			void operator>>(size_t& data);
#endif
			void operator>>(float_t& data);
			void operator>>(double_t& data);
			void operator>>(StBitField8_t& data);
			void operator>>(StBitField16_t& data);
			void operator>>(StBitField32_t& data);
			void operator>>(StBitField64_t& data);
			void operator>>(std::string& data);
			void operator>>(Graphics::CImageObject& data);
			void operator>>(Graphics::CJpeg& data);
			void operator>>(Graphics::CPng& data);
			void operator>>(CAbstractSerialization& data);

			template <typename Type> void operator>>(Eigen::Matrix<Type, 2, 1, 0>& data);
			template <typename Type> void operator>>(Eigen::Matrix<Type, 3, 1, 0>& data);
			template <typename Type> void operator>>(Eigen::Matrix<Type, 4, 1, 0>& data);
			template <typename Type> void operator>>(CPointCloud<Type>& data);
			template <typename Type> void operator>>(std::vector<Type>& data);

		private:
			uint8_t* memory;
			size_t nPos;
		};

		inline CDeSerializer::CDeSerializer(const void_ptr pData)
			: memory(static_cast<uint8_t*>(pData)), nPos(0)
		{
		}

		inline size_t CDeSerializer::size() const
		{
			return nPos;
		}

		inline void CDeSerializer::reset(const void_ptr pData)
		{
			memory = static_cast<uint8_t*>(pData);
			nPos = 0;
		}

		inline void_ptr CDeSerializer::data() const
		{
			return memory;
		}

		inline void_ptr CDeSerializer::cur() const
		{
			return &memory[nPos];
		}

		inline void_ptr CDeSerializer::at(const size_t pos) const
		{
			return &memory[pos];
		}

		inline void CDeSerializer::operator>>(char& data)
		{
			const auto pData = reinterpret_cast<uint8_t*>(&data);
			pData[0] = memory[nPos++];
		}

		inline void CDeSerializer::operator>>(int8_t& data)
		{
			const auto pData = reinterpret_cast<uint8_t*>(&data);
			pData[0] = memory[nPos++];
		}

		inline void CDeSerializer::operator>>(uint8_t& data)
		{
			data = memory[nPos++];
		}

		inline void CDeSerializer::operator>>(int16_t& data)
		{
			const auto pData = reinterpret_cast<uint8_t*>(&data);
			pData[0] = memory[nPos++];
			pData[1] = memory[nPos++];
		}

		inline void CDeSerializer::operator>>(uint16_t& data)
		{
			const auto pData = reinterpret_cast<uint8_t*>(&data);
			pData[0] = memory[nPos++];
			pData[1] = memory[nPos++];
		}

		inline void CDeSerializer::operator>>(int32_t& data)
		{
			const auto pData = reinterpret_cast<uint8_t*>(&data);
			pData[0] = memory[nPos++];
			pData[1] = memory[nPos++];
			pData[2] = memory[nPos++];
			pData[3] = memory[nPos++];
		}

		inline void CDeSerializer::operator>>(uint32_t& data)
		{
			const auto pData = reinterpret_cast<uint8_t*>(&data);
			pData[0] = memory[nPos++];
			pData[1] = memory[nPos++];
			pData[2] = memory[nPos++];
			pData[3] = memory[nPos++];
		}

		inline void CDeSerializer::operator>>(int64_t& data)
		{
			const auto pData = reinterpret_cast<uint8_t*>(&data);
			pData[0] = memory[nPos++];
			pData[1] = memory[nPos++];
			pData[2] = memory[nPos++];
			pData[3] = memory[nPos++];
			pData[4] = memory[nPos++];
			pData[5] = memory[nPos++];
			pData[6] = memory[nPos++];
			pData[7] = memory[nPos++];
		}

		inline void CDeSerializer::operator>>(uint64_t& data)
		{
			const auto pData = reinterpret_cast<uint8_t*>(&data);
			pData[0] = memory[nPos++];
			pData[1] = memory[nPos++];
			pData[2] = memory[nPos++];
			pData[3] = memory[nPos++];
			pData[4] = memory[nPos++];
			pData[5] = memory[nPos++];
			pData[6] = memory[nPos++];
			pData[7] = memory[nPos++];
		}

#if defined( _MSC_VER )
#elif defined(__linux__) // Linux
		inline void CDeSerializer::operator>>(size_t& data)
		{
			const auto pData = reinterpret_cast<uint8_t*>(&data);
			pData[0] = memory[nPos++];
			pData[1] = memory[nPos++];
			pData[2] = memory[nPos++];
			pData[3] = memory[nPos++];
			pData[4] = memory[nPos++];
			pData[5] = memory[nPos++];
			pData[6] = memory[nPos++];
			pData[7] = memory[nPos++];
		}
#endif

		inline void CDeSerializer::operator>>(float_t& data)
		{
			const auto pData = reinterpret_cast<uint8_t*>(&data);
			pData[0] = memory[nPos++];
			pData[1] = memory[nPos++];
			pData[2] = memory[nPos++];
			pData[3] = memory[nPos++];
		}

		inline void CDeSerializer::operator>>(double_t& data)
		{
			const auto pData = reinterpret_cast<uint8_t*>(&data);
			pData[0] = memory[nPos++];
			pData[1] = memory[nPos++];
			pData[2] = memory[nPos++];
			pData[3] = memory[nPos++];
			pData[4] = memory[nPos++];
			pData[5] = memory[nPos++];
			pData[6] = memory[nPos++];
			pData[7] = memory[nPos++];
		}

		inline void CDeSerializer::operator>>(StBitField8_t& data)
		{
			*this >> data.data();
		}

		inline void CDeSerializer::operator>>(StBitField16_t& data)
		{
			*this >> data.data();
		}

		inline void CDeSerializer::operator>>(StBitField32_t& data)
		{
			*this >> data.data();
		}

		inline void CDeSerializer::operator>>(StBitField64_t& data)
		{
			*this >> data.data();
		}

		inline void CDeSerializer::operator>>(std::string& data)
		{
			// insert Size
			std::string str;
			decltype(data.length()) size;

			*this >> size;
			for (size_t i = 0; i < size; i++)
			{
				char c;
				*this >> c;
				str.push_back(c);
			}

			data = std::move(str);
		}

		inline void CDeSerializer::operator>>(Graphics::CImageObject& data)
		{
			uint32_t w = 0;
			uint32_t h = 0;
			uint32_t type = 0;
			*this >> w;
			*this >> h;
			*this >> type;

			data.Create(w, h, static_cast<Graphics::ImageType>(type));
			auto* pImg = data.GetImage();
			for (size_t i = 0; i < data.GetSize(); i++) *this >> pImg[i];
		}

		inline void CDeSerializer::operator>>(Graphics::CJpeg& data)
		{
			uint32_t w = 0;
			uint32_t h = 0;
			uint32_t type = 0;
			*this >> w;
			*this >> h;
			*this >> type;

			*this >> data.JpegData();
		}

		inline void CDeSerializer::operator>>(Graphics::CPng& data)
		{
			uint32_t w = 0;
			uint32_t h = 0;
			uint32_t type = 0;
			*this >> w;
			*this >> h;
			*this >> type;

			*this >> data.PngData();
		}

		inline void CDeSerializer::operator>>(CAbstractSerialization& data)
		{
			data.DeSerialization(*this);
		}

		template <typename Type>
		void CDeSerializer::operator>>(Eigen::Matrix<Type, 2, 1, 0>& data)
		{
			*this >> data[0];
			*this >> data[1];
		}

		template <typename Type>
		void CDeSerializer::operator>>(Eigen::Matrix<Type, 3, 1, 0>& data)
		{
			*this >> data[0];
			*this >> data[1];
			*this >> data[2];
		}

		template <typename Type>
		void CDeSerializer::operator>>(Eigen::Matrix<Type, 4, 1, 0>& data)
		{
			*this >> data[0];
			*this >> data[1];
			*this >> data[3];
			*this >> data[4];
		}

		template <typename Type>
		void CDeSerializer::operator>>(CPointCloud<Type>& data)
		{
			decltype(data.size()) size;

			*this >> size;
			data.resize(size);
			memcpy(&data[0], &memory[nPos], size * data[0].rows() * sizeof(std::remove_reference_t<decltype(data[0].x())>));
			nPos += size * data[0].rows() * sizeof(std::remove_reference_t<decltype(data[0].x())>);
		}

		template <typename Type>
		void CDeSerializer::operator>>(std::vector<Type>& data)
		{
			decltype(data.size()) size;

			*this >> size;

			for (size_t i = 0; i < size; i++)
			{
				Type value;
				*this >> value;
				data.emplace_back(value);
			}
		}
	}
}