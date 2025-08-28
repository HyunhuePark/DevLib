#include <utility>

#include "../../include/GL/CGlLight.hpp"

namespace DevLib {
	namespace GL {
		CGlLight::CGlLight(std::string name)
			: m_lightName(std::move(name))
			, m_bEnable(false)
			, m_bSpotLight(false)
			, m_SpotLightSpotCutOff(90.0F)
			, m_SpotLightExponent(80.0F)
		{
			// GL_AMBIENT default (0.0, 0.0, 0.0, 1.0), ambient RGBA intensity of light
			// GL_DIFFUSE default (1.0, 1.0, 1.0, 1.0), diffuse RGBA intensity of light
			// GL_SPECULAR default (1.0, 1.0, 1.0, 1.0), specular RGBA intensity of light
			// GL_POSITION default (0.0, 0.0, 1.0, 0.0), (x, y, z, w) position of light
			// GL_SPOT_DIRECTION default (0.0, 0.0, -1.0), (x, y, z) direction of spotlight

			//m_Ambient = CPoint4Df(0.0f, 0.0f, 0.0f, 1.0f);
			//m_Diffuse = CPoint4Df(1.0f, 1.0f, 1.0f, 1.0f);
			//m_Specular = CPoint4Df(1.0f, 1.0f, 1.0f, 1.0f);
			//m_Position = CPoint4Df(0.0f, 0.0f, 1.0f, 0.0f);

			m_SpotLightDirection = CPoint4Df(0.0F, 0.0F, -1.0F, 0.0F);

			// Old
			m_Ambient = CPoint4Df(0.1F, 0.1F, 0.1F, 1.0F);
			m_Diffuse = CPoint4Df(0.7F, 0.7F, 0.7F, 1.0F);
			m_Specular = CPoint4Df(0.5F, 0.5F, 0.5F, 1.0F);
			m_Position = CPoint4Df(1.0F, 1.0F, 1.0F, 1.0F);

			//m_SpotLightDirection = CPoint4Df(0.0F, 0.0F, 0.0F, 1.0F);
		}

		void CGlLight::SetEnable(bool bEnable)
		{
			m_bEnable = bEnable;
		}

		bool CGlLight::IsEnable() const
		{
			return m_bEnable;
		}

		CPoint4Df& CGlLight::Ambient()
		{
			return m_Ambient;
		}

		CPoint4Df& CGlLight::Diffuse()
		{
			return m_Diffuse;
		}

		CPoint4Df& CGlLight::Specular()
		{
			return m_Specular;
		}

		CPoint4Df& CGlLight::Position()
		{
			return m_Position;
		}

		CPoint4Df& CGlLight::SpotLightDirection()
		{
			return m_SpotLightDirection;
		}

		float_t& CGlLight::SpotLightSpotCutOff()
		{
			return m_SpotLightSpotCutOff;
		}

		float_t& CGlLight::SpotLightExponent()
		{
			return m_SpotLightExponent;
		}

		bool CGlLight::IsUsedSpotLight() const
		{
			return m_bSpotLight;
		}

		void CGlLight::SetAmbient(CPoint4Df ambient)
		{
			m_Ambient = std::move(ambient);
		}

		void CGlLight::SetDiffuse(CPoint4Df diffuse)
		{
			m_Diffuse = std::move(diffuse);
		}

		void CGlLight::SetSpecular(CPoint4Df specular)
		{
			m_Specular = std::move(specular);
		}

		void CGlLight::SetPosition(CPoint4Df position)
		{
			m_Position = std::move(position);
		}

		void CGlLight::SetAmbient(float_t r, float_t g, float_t b, float_t a)
		{
			SetAmbient(CPoint4Df(r, b, g, a));
		}

		void CGlLight::SetDiffuse(float_t r, float_t g, float_t b, float_t a)
		{
			SetDiffuse(CPoint4Df(r, b, g, a));
		}

		void CGlLight::SetSpecular(float_t r, float_t g, float_t b, float_t a)
		{
			SetSpecular(CPoint4Df(r, b, g, a));
		}

		void CGlLight::SetPosition(float_t x, float_t y, float_t z)
		{
			SetPosition(CPoint4Df(x, y, z, m_bSpotLight ? 0.0F : 1.0F));
		}

		void CGlLight::UseSpotLight(bool bUse)
		{
			m_bSpotLight = bUse;
		}

		void CGlLight::SetSpotLight(float_t directionX, float_t directionY, float_t directionZ,
			float_t SpotCutOff, float_t Exponent)
		{
			m_SpotLightDirection = CPoint4Df(directionX, directionY, directionZ, 1.0F);
			m_SpotLightSpotCutOff = SpotCutOff;
			m_SpotLightExponent = Exponent;
		}
	}
}