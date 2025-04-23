#pragma once

#include "DevLibGLTypes.hpp"

namespace DevLib
{
	namespace GL
	{

		class CGlLight
		{
		public:
			CGlLight(std::string name = "GlLight");

			void SetEnable(bool bEnable = true);
			bool IsEnable() const;

			CPoint4Df& Ambient();
			CPoint4Df& Diffuse();
			CPoint4Df& Specular();
			CPoint4Df& Position();
			CPoint4Df& SpotLightDirection();
			float_t& SpotLightSpotCutOff();
			float_t& SpotLightExponent();

			void SetAmbient(CPoint4Df ambient);
			void SetDiffuse(CPoint4Df diffuse);
			void SetSpecular(CPoint4Df specular);
			void SetPosition(CPoint4Df position);

			void SetAmbient(float_t r, float_t g, float_t b, float_t a = 1.0F);
			void SetDiffuse(float_t r, float_t g, float_t b, float_t a = 1.0F);
			void SetSpecular(float_t r, float_t g, float_t b, float_t a = 1.0F);
			void SetPosition(float_t x, float_t y, float_t z);

			void UseSpotLight(bool bUse = true);
			bool IsUsedSpotLight() const;
			void SetSpotLight(float_t directionX, float_t directionY, float_t directionZ, float_t SpotCutOff = 90.0F, float_t Exponent = 80.0F);

		private:
			std::string m_lightName;
			bool m_bEnable;
			bool m_bSpotLight;
			CPoint4Df m_Ambient;
			CPoint4Df m_Diffuse;
			CPoint4Df m_Specular;
			CPoint4Df m_Position;
			CPoint4Df m_SpotLightDirection;

			float_t	m_SpotLightSpotCutOff;
			float_t	m_SpotLightExponent;

		};

	}
}