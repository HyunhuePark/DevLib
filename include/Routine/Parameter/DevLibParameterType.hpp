#pragma once

#include "../../Base/DevLibTypes.hpp"
#include "../Serialization/CSerializer.hpp"
#include "../Serialization/CDeSerializer.hpp"
#include "../Network/RoutineData.hpp"
#include <string>

namespace DevLib {
	namespace Routine
	{
		class CParamType
		{
		public:
			virtual ~CParamType() = default;

			virtual std::string ToStringValue() { return {}; }
			virtual std::string SetValue(std::string v) { return  {}; }

			std::string ToStringType()
			{
				return m_paramType;
			}

		protected:
			std::string m_paramType;
			EnableSmartPointer(CParamType)
		};

		template <typename Type,
			typename = std::enable_if_t
			<
			std::is_same_v<bool_t, std::remove_reference_t<Type>> ||
			std::is_same_v<int8_t, std::remove_reference_t<Type>> ||
			std::is_same_v<int16_t, std::remove_reference_t<Type>> ||
			std::is_same_v<int32_t, std::remove_reference_t<Type>> ||
			std::is_same_v<int64_t, std::remove_reference_t<Type>> ||
			std::is_same_v<uint8_t, std::remove_reference_t<Type>> ||
			std::is_same_v<uint16_t, std::remove_reference_t<Type>> ||
			std::is_same_v<uint32_t, std::remove_reference_t<Type>> ||
			std::is_same_v<uint64_t, std::remove_reference_t<Type>> ||
			std::is_same_v<size_t, std::remove_reference_t<Type>> ||
			std::is_same_v<float_t, std::remove_reference_t<Type>> ||
			std::is_same_v<double_t, std::remove_reference_t<Type>> ||
			std::is_same_v<string_t, std::remove_reference_t<Type>> , std::remove_reference_t<Type> >
		>
		class CParamValue : public  CParamType
		{
		public:
			CParamValue(Type& refValue)
				: m_value(std::forward<Type&>(refValue))
			{
				if (std::is_same_v<std::remove_reference_t<Type>, bool_t>) m_paramType = "bool_t";
				else if (std::is_same_v<std::remove_reference_t<Type>, int8_t>) m_paramType = "int8_t";
				else if (std::is_same_v<std::remove_reference_t<Type>, int16_t>) m_paramType = "int16_t";
				else if (std::is_same_v<std::remove_reference_t<Type>, int32_t>) m_paramType = "int32_t";
				else if (std::is_same_v<std::remove_reference_t<Type>, int64_t>) m_paramType = "int64_t";
				else if (std::is_same_v<std::remove_reference_t<Type>, uint8_t>) m_paramType = "uint8_t";
				else if (std::is_same_v<std::remove_reference_t<Type>, uint16_t>) m_paramType = "uint16_t";
				else if (std::is_same_v<std::remove_reference_t<Type>, uint32_t>) m_paramType = "uint32_t";
				else if (std::is_same_v<std::remove_reference_t<Type>, uint64_t>) m_paramType = "uint64_t";
				else if (std::is_same_v<std::remove_reference_t<Type>, size_t>) m_paramType = "size_t";
				else if (std::is_same_v<std::remove_reference_t<Type>, float_t>) m_paramType = "float_t";
				else if (std::is_same_v<std::remove_reference_t<Type>, double_t>) m_paramType = "double_t";
				else if (std::is_same_v<std::remove_reference_t<Type>, string_t>) m_paramType = "string_t";
				else m_paramType = "unknown";
			}


			std::string ToStringValue() override
			{
				std::string str{};

				if constexpr (std::is_same_v<std::remove_reference_t<Type>, bool_t>)
				{
					if (m_value) str = "true";
					else str = "false";
				}
				else if constexpr (std::is_same_v<std::remove_reference_t<Type>, string_t>)
				{
					str = m_value;
				}
				else 
				{
					str = std::to_string(m_value);
				}

				return str;
			}

			std::string SetValue(std::string value) override
			{
				if constexpr (std::is_same_v<std::remove_reference_t<Type>, bool_t>)
				{
					std::transform(value.begin(), value.end(), value.begin(), ::tolower);
					m_value = value == "true" ? true : false;
				}
				else if constexpr (std::is_same_v<std::remove_reference_t<Type>, float_t> || std::is_same_v<std::remove_reference_t<Type>, double_t>)
				{
					m_value = static_cast<std::remove_reference_t<Type>>(std::atof(value.c_str()));
				}
				else if constexpr (std::is_same_v<std::remove_reference_t<Type>, string_t>)
				{
					m_value = value;
				}
				else // if (m_paramType == "int8_t" || m_paramType == "int16_t" || m_paramType == "int32_t" || m_paramType == "int64_t" || m_paramType == "size_t")
				{
					m_value = static_cast<std::remove_reference_t<Type>>(std::atoll(value.c_str()));
				}

				return ToStringValue();
			}

			Type& data()
			{
				return m_value;
			}

			Type& operator*()
			{
				return m_value;
			}

		private:
			Type& m_value;
		};


		///
		// Network Packet
		///
		BeginRoutineData(ParameterObject)
		std::string GroupName;
		std::string Name;
		std::string Type;
		std::string Value;

		void Serialization(CSerializer& serializer)override
		{
			serializer << GroupName;
			serializer << Name;
			serializer << Type;
			serializer << Value;
		}

		void DeSerialization(CDeSerializer& deserializer)override
		{
			deserializer >> GroupName;
			deserializer >> Name;
			deserializer >> Type;
			deserializer >> Value;
		}
		EndRoutineData(ParameterObject)

			/// <summary>
			/// 
			/// </summary>
		BeginRoutineData(ParameterPacket)

		std::vector<ParameterObject> parameters;

		void Serialization(CSerializer& serializer)override
		{
			serializer << parameters;
		}

		void DeSerialization(CDeSerializer& deserializer)override
		{
			deserializer >> parameters;
		}

		EndRoutineData(ParameterPacket)
	}
}