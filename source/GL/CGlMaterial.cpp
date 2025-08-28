#include "../../include/GL/CGlMaterial.hpp"

namespace DevLib {

	namespace GL
	{
		CGlMaterial::CGlMaterial(std::string name)
			: m_materialName(std::move(name))
			, m_glTextureID(std::make_unique<uint32_t>(0))
		{
			m_materialAmbient = CPoint4Df(0.2f, 0.2f, 0.2f, 1.0f);
			m_materialDiffuse = CPoint4Df(0.8f, 0.8f, 0.8f, 1.0f);
			m_materialSpecular = CPoint4Df(0.0f, 0.0f, 0.0f, 1.0f);
			m_materialEmission = CPoint4Df(0.0f, 0.0f, 0.0f, 1.0f);
			m_materialShininess = 0.f;
		}

		CGlMaterial::CGlMaterial(const CGlMaterial& rhs)
			: m_materialName(rhs.m_materialName)
			, m_textureImage(rhs.m_textureImage)
			, m_materialAmbient(rhs.m_materialAmbient)
			, m_materialDiffuse(rhs.m_materialDiffuse)
			, m_materialSpecular(rhs.m_materialSpecular)
			, m_materialEmission(rhs.m_materialEmission)
			, m_materialShininess(rhs.m_materialShininess)
			, m_glTextureID(std::make_unique<uint32_t>(0))
		{
		}

		void CGlMaterial::SetName(const std::string& name)
		{
			m_materialName = name;
		}

		std::string CGlMaterial::GetName()
		{
			return m_materialName;
		}

		void CGlMaterial::SetTexture(const Graphics::CImage& texture)
		{
			m_textureImage = texture;
		}

		GlTexture& CGlMaterial::GetTexture()
		{
			return m_textureImage;
		}

		GlMaterialProp& CGlMaterial::Ambient()
		{
			return m_materialAmbient;
		}

		GlMaterialProp& CGlMaterial::Diffuse()
		{
			return m_materialDiffuse;
		}

		GlMaterialProp& CGlMaterial::Specular()
		{
			return m_materialSpecular;
		}

		GlMaterialProp& CGlMaterial::Emission()
		{
			return m_materialEmission;
		}

		void CGlMaterial::SetEmission(const GlMaterialProp& emission)
		{
			m_materialEmission = emission;
		}

		void CGlMaterial::SetEmission(float_t r, float_t g, float_t b, float_t a)
		{
			SetEmission(GlMaterialProp(r, g, b, a));
		}

		void CGlMaterial::SetShininess(float_t shininess)
		{
			m_materialShininess = shininess;
		}

		float_t& CGlMaterial::Shininess()
		{
			return m_materialShininess;
		}

		bool CGlMaterial::LoadImageFile(const std::string& imgPath)
		{
			return m_textureImage.LoadImageFile(imgPath);
		}

		void CGlMaterial::SetTransparent(float_t transparent)
		{
			m_materialAmbient.w() = transparent;
			m_materialDiffuse.w() = transparent;
			m_materialSpecular.w() = transparent;
			m_materialEmission.w() = transparent;
		}

		void CGlMaterial::SetAmbient(const GlMaterialProp& ambient)
		{
			m_materialAmbient = ambient;
		}

		void CGlMaterial::SetDiffuse(const GlMaterialProp& diffuse)
		{
			m_materialDiffuse = diffuse;
		}

		void CGlMaterial::SetSpecular(const GlMaterialProp& specular)
		{
			m_materialSpecular = specular;
		}

		void CGlMaterial::SetAmbient(float_t r, float_t g, float_t b, float_t a)
		{
			SetAmbient(GlMaterialProp(r, g, b, a));
		}

		void CGlMaterial::SetDiffuse(float_t r, float_t g, float_t b, float_t a)
		{
			SetDiffuse(GlMaterialProp(r, g, b, a));
		}

		void CGlMaterial::SetSpecular(float_t r, float_t g, float_t b, float_t a)
		{
			SetSpecular(GlMaterialProp(r, g, b, a));
		}

		CGlMaterial& CGlMaterial::operator=(const CGlMaterial& rhs)
		{
			m_materialName = rhs.m_materialName;
			m_textureImage = rhs.m_textureImage;
			m_materialAmbient = rhs.m_materialAmbient;
			m_materialDiffuse = rhs.m_materialDiffuse;
			m_materialSpecular = rhs.m_materialSpecular;
			m_materialShininess = rhs.m_materialShininess;
			m_materialEmission = rhs.m_materialEmission;
			m_glTextureID = std::make_unique<uint32_t>(0);

			return *this;
		}
		
	}
}
