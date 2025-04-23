#pragma once

#include "DevLibGLTypes.hpp"

namespace DevLib
{
	namespace GL
	{
		class CGlCore;

		class CGlMaterial
		{
		public:
			CGlMaterial(std::string name = "GlMaterial");
			CGlMaterial(const CGlMaterial& rhs);

			void SetName(const std::string& name = "GlMaterial");
			std::string GetName();

			GlTexture& GetTexture();

			GlMaterialProp& Ambient();
			GlMaterialProp& Diffuse();
			GlMaterialProp& Specular();
			GlMaterialProp& Emission();
			float_t& Shininess();

			void SetTexture(const Graphics::CImage& texture);

			void SetAmbient(const GlMaterialProp& ambient);
			void SetDiffuse(const GlMaterialProp& diffuse);
			void SetSpecular(const GlMaterialProp& specular);
			void SetEmission(const GlMaterialProp& emission);

			void SetAmbient(float_t r = 0.2f, float_t g = 0.2f, float_t b = 0.2f, float_t a = 1.0F);
			void SetDiffuse(float_t r = 0.8f, float_t g = 0.8f, float_t b = 0.8f, float_t a = 1.0F);
			void SetSpecular(float_t r = 0.0f, float_t g = 0.0f, float_t b = 0.0f, float_t a = 1.0F);
			void SetEmission(float_t r = 0.0f, float_t g = 0.0f, float_t b = 0.0f, float_t a = 1.0F);
			void SetShininess(float_t shininess = 0.0f);
			void SetTransparent(float_t transparent = 1.0f);

			CGlMaterial& operator=(const CGlMaterial& rhs);

		private:
			std::string m_materialName;

			GlTexture		m_textureImage;

			GlMaterialProp	m_materialAmbient;
			GlMaterialProp	m_materialDiffuse;
			GlMaterialProp	m_materialSpecular;
			GlMaterialProp	m_materialEmission;
			float_t			m_materialShininess;

			friend CGlCore;
			std::unique_ptr<uint32_t> m_glTextureID;	// Only Use CGlCOre
		};

		using GlMaterials = std::vector<CGlMaterial>;

	}
}