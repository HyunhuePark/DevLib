#include "../../include/GL/CGlObject.hpp"

namespace DevLib {
	namespace GL {
		CGlObject::CGlObject(const GlObjectType type, std::string name)
			: m_objType(type)
			, m_modelName(std::move(name))
			, m_drawPointSize(1.0F)
			, m_drawColor(CPoint4Df(1.0F, 1.0F, 1.0F, 1.0F))
			, m_scale(1, 1, 1)
			, m_position(0, 0, 0)
			, m_rotation(0, 0, 0)
			, m_shadeModel(GlShadeModel::SMOOTH)
			, m_bCulling(false)
			, m_cullFace(GlCullingFace::CULL_BACK)
			, m_cullFrontFace(GlCullingFrontFace::CULL_CCW)
			, m_bEnableBlend(false)
			, m_bLightning(false)
			, m_bCompile(true)
			, m_glListID(std::make_unique<uint32_t>(0))
			, m_materialName(std::string())
		{

		}

		CGlObject::CGlObject(const CGlObject& rhs)
			: m_objType(rhs.m_objType)
			, m_modelName(rhs.m_modelName)
			, m_drawPointSize(rhs.m_drawPointSize)
			, m_drawColor(rhs.m_drawColor)
			, m_scale(rhs.m_scale)
			, m_position(rhs.m_position)
			, m_rotation(rhs.m_rotation)
			, m_vertices(rhs.m_vertices)
			, m_indices(rhs.m_indices)
			, m_colors(rhs.m_colors)
			, m_normals(rhs.m_normals)
			, m_textureCoords(rhs.m_textureCoords)
			, m_shadeModel(rhs.m_shadeModel)
			, m_bCulling(rhs.m_bCulling)
			, m_cullFace(rhs.m_cullFace)
			, m_cullFrontFace(rhs.m_cullFrontFace)
			, m_bEnableBlend(rhs.m_bEnableBlend)
			, m_bLightning(rhs.m_bLightning)
			, m_bCompile(rhs.m_bCompile)
			, m_glListID(std::make_unique<uint32_t>(0))
			, m_materialName(rhs.m_materialName)
		{
		}

		void CGlObject::SetName(const std::string& name)
		{
			m_modelName = name;
		}

		std::string CGlObject::GetName()
		{
			return m_modelName;
		}

		void CGlObject::SetType(GlObjectType type)
		{
			m_objType = type;
		}

		GlObjectType CGlObject::GetType() const
		{
			return m_objType;
		}

		void CGlObject::SetDrawPointSize(float_t size)
		{
			m_drawPointSize = size;
		}

		float_t CGlObject::GetDrawPointSize() const
		{
			return m_drawPointSize;
		}

		void CGlObject::SetDrawColor(const CPoint4Df& color)
		{
			m_drawColor = color;
		}

		void CGlObject::SetDrawColor(float_t r, float_t g, float_t b, float_t a)
		{
			SetDrawColor(CPoint4Df(r, g, b, a));
		}

		CPoint4Df CGlObject::GetDrawColor() const
		{
			return m_drawColor;
		}

		void CGlObject::SetCompile(bool bCompile)
		{
			m_bCompile = bCompile;
		}
		

		void CGlObject::SetScale(const CPoint3Df& scale)
		{
			m_scale = scale;
		}

		void CGlObject::SetPosition(const CPoint3Df& pos)
		{
			m_position = pos;
		}

		void CGlObject::SetRotation(const CPoint3Df& ori)
		{
			m_rotation = ori;
		}
		
		void CGlObject::AddPoint(CPointCloud<>& points)
		{
			m_vertices.insert(m_vertices.end(), points.GetPoints().begin(), points.GetPoints().end());
		}

		void CGlObject::AddTextureCoord(const CPointf& coord)
		{
			m_textureCoords.emplace_back(coord);
		}

		void CGlObject::AddPoint(const CPoint3Df& point)
		{
			m_vertices.emplace_back(point);
		}

		void CGlObject::AddIndex(const uint32_t& index)
		{
			m_indices.emplace_back(index);
		}

		void CGlObject::AddColor(const CPoint4Df& color)
		{
			m_colors.emplace_back(color);
		}

		void CGlObject::AddNormal(const CPoint3Df& normal)
		{
			m_normals.emplace_back(normal);
		}

		GlShadeModel CGlObject::GetShadeModel() const
		{
			return m_shadeModel;
		}

		void CGlObject::SetShadeModel(GlShadeModel model)
		{
			m_shadeModel = model;
		}

		bool CGlObject::GetEnableCulling() const
		{
			return m_bCulling;
		}

		GlCullingFace& CGlObject::CullingFace()
		{
			return m_cullFace;
		}

		GlCullingFrontFace& CGlObject::CullingFrontFace()
		{
			return m_cullFrontFace;
		}

		void CGlObject::EnableCulling()
		{
			m_bCulling = true;
		}

		void CGlObject::DisableCulling()
		{
			m_bCulling = false;
		}

		void CGlObject::SetCullingFace(GlCullingFace face)
		{
			m_cullFace = face;
		}

		void CGlObject::SetCullingFrontFace(GlCullingFrontFace frontFace)
		{
			m_cullFrontFace = frontFace;
		}

		void CGlObject::SetMaterialName(const std::string& matrialName)
		{
			m_materialName = matrialName;
		}

		std::string CGlObject::GetMaterialName() const
		{
			return m_materialName;
		}

		void CGlObject::Clear()
		{
			m_vertices.clear();
			m_indices.clear();
			m_colors.clear();
			m_normals.clear();
			m_textureCoords.clear();
		}

		CGlObject& CGlObject::operator=(const CGlObject& rhs)
		{
			m_objType = rhs.m_objType;
			m_modelName = rhs.m_modelName;
			m_drawPointSize = rhs.m_drawPointSize;
			m_drawColor = rhs.m_drawColor;
			m_scale = rhs.m_scale;
			m_position = rhs.m_position;
			m_rotation = rhs.m_rotation;
			m_vertices = rhs.m_vertices;
			m_indices = rhs.m_indices;
			m_colors = rhs.m_colors;
			m_normals = rhs.m_normals;
			m_textureCoords = rhs.m_textureCoords;
			m_shadeModel = rhs.m_shadeModel;
			m_bCulling = rhs.m_bCulling;
			m_cullFace = rhs.m_cullFace;
			m_cullFrontFace = rhs.m_cullFrontFace;
			m_bEnableBlend = rhs.m_bEnableBlend;
			m_bLightning = rhs.m_bLightning;
			m_bCompile = rhs.m_bCompile;
			m_glListID = std::make_unique<uint32_t>(0);
			m_materialName = rhs.m_materialName;

			return *this;
		}

		void CGlObject::EnableBlend()
		{
			m_bEnableBlend = true;
		}

		void CGlObject::DisableBlend()
		{
			m_bEnableBlend = false;
		}
		
		void CGlObject::EnableLightning()
		{
			m_bLightning = true;
		}

		void CGlObject::DisableLightning()
		{
			m_bLightning = false;
		}
	}
}
