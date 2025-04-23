#pragma once

#include "DevLibGLTypes.hpp"

namespace DevLib
{
	namespace GL
	{
		class CGlCore;

		class CGlObject
		{
		public:
			CGlObject(GlObjectType type = GlObjectType::GlPoints, std::string name = "GlObject");
			CGlObject(const CGlObject& rhs);

			void SetName(const std::string& name = "GlObject");
			std::string GetName();

			void SetType(GlObjectType type);
			GlObjectType GetType() const;

			void SetDrawPointSize(float_t size);
			float_t GetDrawPointSize() const;

			void SetDrawColor(const CPoint4Df& color);
			void SetDrawColor(float_t r, float_t g, float_t b, float_t a = 1.0F);
			CPoint4Df GetDrawColor() const;

			void SetCompile(bool bCompile);

			// Object Pose
			inline CPoint3Df& Scale();
			inline CPoint3Df& Position();
			inline CPoint3Df& Rotation();

			void SetScale(const CPoint3Df& scale);
			void SetPosition(const CPoint3Df& pos);
			void SetRotation(const CPoint3Df& ori);

			inline void SetScale(float_t x, float_t y, float_t z);
			inline void SetPosition(float_t x, float_t y, float_t z);
			inline void SetRotation(float_t x_deg, float_t y_deg, float_t z_deg);

			inline GlVertices& Points();
			inline GlIndices& Indices();
			inline GlColors& Colors();
			inline GlNormals& Normals();
			inline GlTextureCoords& TextureCoords();

			void AddPoint(CPointCloud<>& points);

			void AddPoint(const CPoint3Df& point);
			void AddIndex(const uint32_t& index);
			void AddColor(const CPoint4Df& color);
			void AddNormal(const CPoint3Df& normal);
			void AddTextureCoord(const CPointf& coord);

			inline void AddPoint(float_t x, float_t y, float_t z);
			inline void AddIndex(uint32_t point1, uint32_t point2, uint32_t point3);
			inline void AddColor(float_t r, float_t g, float_t b, float_t a = 1.0F);
			inline void AddNormal(float_t vx, float_t vy, float_t vz);
			inline void AddTextureCoord(float_t x, float_t y);

			GlShadeModel GetShadeModel() const;
			void SetShadeModel(GlShadeModel model);

			void EnableCulling();
			void DisableCulling();
			bool GetEnableCulling() const;

			GlCullingFace& CullingFace();
			GlCullingFrontFace& CullingFrontFace();
			
			void SetCullingFace(GlCullingFace face);
			void SetCullingFrontFace(GlCullingFrontFace frontFace);

			void SetMaterialName(const std::string& matrialName);
			std::string GetMaterialName() const;

			void Clear();

			CGlObject& operator=(const CGlObject& rhs);

			inline bool Blend() const;
			void EnableBlend();
			void DisableBlend();

			inline bool Lightning() const;
			void EnableLightning();
			void DisableLightning();
		private:
			GlObjectType	m_objType;
			std::string		m_modelName;
			float_t			m_drawPointSize;
			CPoint4Df		m_drawColor;

			CPoint3Df m_scale;
			CPoint3Df m_position;
			CPoint3Df m_rotation;

			GlVertices		m_vertices;
			GlIndices		m_indices;
			GlColors		m_colors;
			GlNormals		m_normals;
			GlTextureCoords m_textureCoords;

			// Shade Model
			GlShadeModel m_shadeModel;

			// Culling Option
			bool m_bCulling;
			GlCullingFace m_cullFace;
			GlCullingFrontFace m_cullFrontFace;

			bool m_bEnableBlend;
			bool m_bLightning;

		protected:
			friend CGlCore;
			bool m_bCompile;
			std::unique_ptr<uint32_t> m_glListID;		// Only Use CGlCorey
			std::string m_materialName;
		};

		using GlObjects = std::vector<CGlObject>;


		inline CPoint3Df& CGlObject::Scale()
		{
			return m_scale;
		}

		inline CPoint3Df& CGlObject::Position()
		{
			return m_position;
		}

		inline CPoint3Df& CGlObject::Rotation()
		{
			return m_rotation;
		}

		inline void CGlObject::SetScale(float_t x, float_t y, float_t z)
		{
			SetScale(CPoint3Df(x, y, z));
		}

		inline void CGlObject::SetPosition(float_t x, float_t y, float_t z)
		{
			SetPosition(CPoint3Df(x, y, z));
		}

		inline void CGlObject::SetRotation(float_t x_deg, float_t y_deg, float_t z_deg)
		{
			SetRotation(CPoint3Df(x_deg, y_deg, z_deg));
		}

		inline GlVertices& CGlObject::Points()
		{
			return m_vertices;
		}

		inline GlIndices& CGlObject::Indices()
		{
			return m_indices;
		}

		inline GlColors& CGlObject::Colors()
		{
			return m_colors;
		}

		inline GlNormals& CGlObject::Normals()
		{
			return m_normals;
		}

		inline GlTextureCoords& CGlObject::TextureCoords()
		{
			return m_textureCoords;
		}

		inline void CGlObject::AddPoint(float_t x, float_t y, float_t z)
		{
			AddPoint(CPoint3Df(x, y, z));
		}

		inline void CGlObject::AddIndex(uint32_t point1, uint32_t point2, uint32_t point3)
		{
			AddIndex(point1); AddIndex(point2); AddIndex(point3);
		}

		inline void CGlObject::AddColor(float_t r, float_t g, float_t b, float_t a)
		{
			AddColor(CPoint4Df(r, g, b, a));
		}

		inline void CGlObject::AddNormal(float_t vx, float_t vy, float_t vz)
		{
			AddNormal(CPoint3Df(vx, vy, vz));
		}

		inline void CGlObject::AddTextureCoord(float_t x, float_t y)
		{
			AddTextureCoord(CPointf(x, y));
		}

		inline bool CGlObject::Blend() const
		{
			return m_bEnableBlend;
		}

		inline bool CGlObject::Lightning() const
		{
			return  m_bLightning;
		}

	}
}
