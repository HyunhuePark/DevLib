#pragma once

#if defined(_MSC_VER)
#include <Windows.h>
#include <gl/GL.h>

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "OPENGL32.lib")
#pragma comment(lib, "GLU32.lib")

#elif defined(__linux__) // Linux
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#endif

#include "../Base/DevLibTypes.hpp"
#include "../Base/CPointCloud.hpp"
#include "../Graphics/CImage.hpp"

namespace DevLib
{
	namespace GL
	{
		enum class GlObjectType
		{
			GlPoints,
			GlLines,
			GlLineLoop,
			GlLineStrip,
			GlTriangles,
			GlTriangleStrip,
			GlTriangleFan,
			GlQuads,
			GlQuadStrip,
			GlPolygon
		};

		enum class GlShadeModel
		{
			FLAT,
			SMOOTH
		};

		enum class GlCullingFace
		{
			CULL_BACK,
			CULL_FRONT,
			CULL_BACK_AND_FRONT
		};

		enum class GlCullingFrontFace
		{
			CULL_CW,
			CULL_CCW
		};

		// Defines
		using GlVertices = std::vector<CPoint3Df>;
		using GlIndices = std::vector<uint32_t>;
		using GlColors = std::vector<CPoint4Df>;
		using GlNormals = std::vector<CPoint3Df>;
		using GlTexture = Graphics::CImage;
		using GlTextureCoords = std::vector<CPointf>;
		using GlMaterialProp = CPoint4Df;

		// [X][Y]
		class GlDepthBuffer : public std::vector<std::vector<float_t>>
		{
		public:
			GlDepthBuffer() = default;

			int32_t Width() const
			{
				return static_cast<int32_t>(size());
			}

			int32_t Height() const
			{
				return static_cast<int32_t>((*this)[0].size());
			}
		};

#if defined(_MSC_VER)
		using GLContext = HGLRC;
#elif defined(__linux__) // Linux
		using GLContext = GLXContext;
#endif 

		inline uint32_t ObjectTypeToGlType(GlObjectType type)
		{
			uint32_t glType = GL_POINTS;
			switch (type)
			{
			case GlObjectType::GlPoints:
				glType = GL_POINTS;
				break;
			case GlObjectType::GlLines:
				glType = GL_LINES;
				break;
			case GlObjectType::GlLineLoop:
				glType = GL_LINE_LOOP;
				break;
			case GlObjectType::GlLineStrip:
				glType = GL_LINE_STRIP;
				break;
			case GlObjectType::GlTriangles:
				glType = GL_TRIANGLES;
				break;
			case GlObjectType::GlTriangleStrip:
				glType = GL_TRIANGLE_STRIP;
				break;
			case GlObjectType::GlTriangleFan:
				glType = GL_TRIANGLE_FAN;
				break;
			case GlObjectType::GlQuads:
				glType = GL_QUADS;
				break;
			case GlObjectType::GlQuadStrip:
				glType = GL_QUAD_STRIP;
				break;
			case GlObjectType::GlPolygon:
				glType = GL_POLYGON;
				break;
			}

			return glType;
		}

		class CGlFog
		{
		public:
			CGlFog(uint32_t _mode = GL_LINEAR, float_t _density = 0.5f, float_t _near = 0.0f, float_t _far = 1.0f, float_t r = 1.0f, float_t g = 1.0f, float_t b = 1.0f)
				: fogMode(_mode), fogDensity(_density), fogNear(_near), fogFar(_far), fogColor(r, g, b)
			{

			}

			uint32_t GetMode() const { return  fogMode;  }
			float_t GetDensity() const { return  fogDensity; }
			float_t GetNear() const { return  fogNear; }
			float_t GetFar() const { return  fogFar; }
			CPoint3Df GetColor() { return  fogColor; }

			inline uint32_t SetMode(uint32_t _mode = GL_EXP2);
			inline float_t SetDensity(float_t _density = 0.5f);
			inline float_t SetNear(float_t _near = 0.0f);
			inline float_t SetFar(float_t _far = 1.0f);
			inline CPoint3Df SetColor(float_t r = 1.0f, float_t g = 1.0f, float_t b = 1.0f);

		private:
			uint32_t fogMode;
			float_t fogDensity;
			float_t fogNear;
			float_t fogFar;
			CPoint3Df fogColor;
		};

		inline uint32_t CGlFog::SetMode(uint32_t _mode)
		{
			const auto curFogMode = fogMode;
			fogMode = _mode;
			return curFogMode;
		}

		inline float_t CGlFog::SetDensity(float_t _density)
		{
			const auto curDensity = fogDensity;
			fogDensity = _density;
			return curDensity;
		}

		inline float_t CGlFog::SetNear(float_t _near)
		{
			const auto curNear = fogNear;
			fogNear = _near;
			return curNear;
		}

		inline float_t CGlFog::SetFar(float_t _far)
		{
			const auto curFar = fogFar;
			fogFar = _far;
			return curFar;
		}

		inline CPoint3Df CGlFog::SetColor(float_t r, float_t g, float_t b)
		{
			auto curColor = fogColor;
			fogColor = CPoint3Df(r, g, b);
			return curColor;
		}
	}
}