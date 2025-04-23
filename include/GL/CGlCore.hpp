#pragma once


#include "../Base/CCriticalSectionEx.hpp"
#include "../Base/DevLibCallback.hpp"
#include "../GUI/DevLibGUITypes.hpp"

#include "CGlModel.hpp"
#include "CGlCamera.hpp"
#include "CGlLight.hpp"

#define GL_MAX_LIGHT  (8)
#define BLEND_Z_ODER	// Blend Z-ordering Test

namespace DevLib {
	namespace GL {

		class CGlCore
		{
		public:
			CGlCore(float_t zoom = 1.0, bool bShowCoord = false, bool bPerspective = false);
			virtual  ~CGlCore();

			bool Create(const GUI::WinDC& hdc);
			bool IsCreated() const;
			void Destroy();

			int GetWidth() const;
			int GetHeight() const;

			// GL Draw
			bool BeginContext() const;
			void ClearBuffer() const;
			void DrawModels();
			void SwapBuffer() const;
			bool EndContext() const;

			void AddModel(const CGlModel& model);
			inline GlModels& Models();
			void ClearModels();

			inline CGlCamera& Camera();
			CGlCamera SetCamera(const CGlCamera& camera);

			void RotationLookAt(float_t dx, float_t dy);
			void RotationCamera(float_t dx, float_t dy);
			void TranslateCamera(float_t dx, float_t dy);

			float_t Zoom(float_t fZoom);
			float_t SetZoom(float_t fZoom);
			float_t GetZoomScale() const;
			float_t GetFieldOfView() const;

			void SetPerspective(bool bPerspective);
			float_t SetFieldOfView(float_t angle);


			void SetBackgroundColor(float b, float g, float r, float alpha = 1.0f) const;

			void EnableBlend();
			void DisableBlend();

			CGlLight SetLight0(CGlLight light);
			CGlLight SetLight1(CGlLight light);
			CGlLight SetLight2(CGlLight light);
			CGlLight SetLight3(CGlLight light);
			CGlLight SetLight4(CGlLight light);
			CGlLight SetLight5(CGlLight light);
			CGlLight SetLight6(CGlLight light);
			CGlLight SetLight7(CGlLight light);

			inline CGlLight& Light0();
			inline CGlLight& Light1();
			inline CGlLight& Light2();
			inline CGlLight& Light3();
			inline CGlLight& Light4();
			inline CGlLight& Light5();
			inline CGlLight& Light6();
			inline CGlLight& Light7();

			inline double_t zNear() const;
			inline double_t zFar() const;

			void EnableFog();
			void DisableFog();
			inline CGlFog& Fog();
			void SetFog(const CGlFog& fog);
			inline void SetFog(uint32_t _mode = GL_LINEAR, float_t _density = 0.5f, float_t _near = 0.0f, float_t _far = 1.0f);

			void ReSize();

			void CaptureScreen(Graphics::CImage& capture, Graphics::ImageType type = Graphics::ImageType::IMG_BGR) const;
			void CaptureDepth(GlDepthBuffer& depth_buffer) const;

			EnableCallback(InitializeGL, const DevLib::GL::CGlCore& Core)
			EnableCallback(ProjectionMatrix, const DevLib::GL::CGlCore& Core)
			EnableCallback(ModelMatrix, const DevLib::GL::CGlCore& Core)
			EnableCallback(UpdateOption, const DevLib::GL::CGlCore& Core)

		private:
			CCriticalSectionEx m_lcokGL;

			GLContext m_hrc;
			GUI::WinDC m_hdc;

			int32_t m_width;
			int32_t m_height;

			CGlCamera m_camera;

			// LookAt Point
			CGlModel m_lookAtModel;

			// Vertex 
			GlModels m_models;

			// Light
			CGlLight m_vLight[GL_MAX_LIGHT];

			// Blending
			bool m_bEnableBlend;

			// 
			double_t m_zNear;
			double_t m_zFar;

			// Fog
			bool m_bEnableFog;
			CGlFog m_fog;

			// OpenGL Process
			void InitializeGL() const;
			void ProjectionMatrix();
			void ModelMatrix();
			void UpdateOption();

			// Object Draw
			void ObjectDrawGL(DevLib::GL::CGlObject& object);
			void ObjectMaterial(DevLib::GL::GlMaterials& materials, const std::string& materialName);
			void ObjectListCall(DevLib::GL::CGlObject& object);

			// Compile
			void ModelCompile();
			void TextureCompile(DevLib::GL::CGlMaterial& mat);

			// Gl LIst ID 
			void DestroyGlObject();

		};


		inline std::vector<CGlModel>& CGlCore::Models()
		{
			return m_models;
		}

		inline CGlCamera& CGlCore::Camera()
		{
			return m_camera;
		}

		inline CGlLight& CGlCore::Light0()
		{
			return m_vLight[0];
		}

		inline CGlLight& CGlCore::Light1()
		{
			return m_vLight[1];
		}

		inline CGlLight& CGlCore::Light2()
		{
			return m_vLight[2];
		}

		inline CGlLight& CGlCore::Light3()
		{
			return m_vLight[3];
		}

		inline CGlLight& CGlCore::Light4()
		{
			return m_vLight[4];
		}

		inline CGlLight& CGlCore::Light5()
		{
			return m_vLight[5];
		}

		inline CGlLight& CGlCore::Light6()
		{
			return m_vLight[6];
		}

		inline CGlLight& CGlCore::Light7()
		{
			return m_vLight[7];
		}

		inline double_t CGlCore::zNear() const
		{
			return m_zNear;
		}

		inline double_t CGlCore::zFar() const
		{
			return m_zFar;
		}

		inline CGlFog& CGlCore::Fog()
		{
			return m_fog;
		}

		inline void CGlCore::SetFog(uint32_t _mode, float_t _density, float_t _near, float_t _far)
		{
			SetFog(CGlFog(_mode, _density, _near, _far));
		}
	}
}
