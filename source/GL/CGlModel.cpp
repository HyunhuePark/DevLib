#include "../../include/GL/CGlModel.hpp"

namespace DevLib {

	namespace GL
	{
		CGlModel::CGlModel(const std::string& name)
			: m_modelName(name)
			, m_scale(1, 1, 1)
			, m_position(0, 0, 0)
			, m_rotation(0, 0, 0)
		{

		}

		CGlModel::CGlModel(const CGlModel& rhs)
			: m_modelName(rhs.m_modelName)
			, m_objects(rhs.m_objects)
			, m_materials(rhs.m_materials)
			, m_scale(rhs.m_scale)
			, m_position(rhs.m_position)
			, m_rotation(rhs.m_rotation)
		{
		}

		void CGlModel::SetName(const std::string& name)
		{
			m_modelName = name;
		}

		std::string CGlModel::GetName()
		{
			return m_modelName;
		}

		void CGlModel::SetScale(const CPoint3Df& scale)
		{
			m_scale = scale;
		}

		void CGlModel::SetScale(float_t x, float_t y, float_t z)
		{
			m_scale = CPoint3Df(x, y, z);
		}

		inline void CGlModel::SetPosition(const CPoint3Df& pos)
		{
			m_position = pos;
		}

		inline void CGlModel::SetRotation(const CPoint3Df& ori)
		{
			m_rotation = ori;
		}

		void CGlModel::SetPosition(float_t x, float_t y, float_t z)
		{
			SetPosition(CPoint3Df(x, y, z));
		}

		void CGlModel::SetRotation(float_t x_deg, float_t y_deg, float_t z_deg)
		{
			SetRotation(CPoint3Df(x_deg, y_deg, z_deg));
		}

		size_t CGlModel::AddObject(const CGlObject& object)
		{
			m_objects.emplace_back(object);
			//m_objects.push_back(object);
			return m_objects.size();
		}

		size_t CGlModel::AddMaterial(const CGlMaterial& material)
		{
			m_materials.emplace_back(material);
			//m_objects.push_back(object);
			return m_materials.size();
		}

		void CGlModel::Clear()
		{
			m_objects.clear();
		}
	}
}
