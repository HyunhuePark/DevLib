#pragma once

#include "CGlMaterial.hpp"
#include "CGlObject.hpp"

namespace DevLib
{
	namespace GL
	{
		class CGlModel
		{
		public:
			CGlModel(const std::string& name = "GlModel");
			CGlModel(const CGlModel& rhs);

			void SetName(const std::string& name = "GlObject");
			std::string GetName();

			inline CPoint3Df& Scale();
			inline CPoint3Df& Position();
			inline CPoint3Df& Rotation();

			void SetScale(const CPoint3Df& scale);
			void SetPosition(const CPoint3Df& pos);
			void SetRotation(const CPoint3Df& ori);

			void SetScale(float_t x, float_t y, float_t z);
			void SetPosition(float_t x, float_t y, float_t z);
			void SetRotation(float_t x_deg, float_t y_deg, float_t z_deg);

			inline GlObjects& Objects();
			inline GlMaterials& Materials();

			size_t AddObject(const CGlObject& object);
			size_t AddMaterial(const CGlMaterial& material);

			void Clear();
		private:
			std::string m_modelName;
			GlObjects m_objects;
			GlMaterials m_materials;

			CPoint3Df m_scale;
			CPoint3Df m_position;
			CPoint3Df m_rotation;
		};

		using GlModels = std::vector<CGlModel>;

		inline CPoint3Df& CGlModel::Scale()
		{
			return m_scale;
		}


		inline CPoint3Df& CGlModel::Position()
		{
			return m_position;
		}

		inline CPoint3Df& CGlModel::Rotation()
		{
			return m_rotation;
		}

		inline GlObjects& CGlModel::Objects()
		{
			return  m_objects;
		}

		inline GlMaterials& CGlModel::Materials()
		{
			return  m_materials;
		}


	}
}