#if defined(_MSC_VER)

#pragma warning(disable : 5105)
#endif

#include <utility>

#include "../../include/GL/CGlCore.hpp"

#if defined(_MSC_VER)
#include <GL/GLU.h>
#pragma warning(default : 5105)
#endif

#include <cmath>

namespace DevLib {
	namespace GL {

		constexpr float_t GL_PI = 3.14159265358979323846F;

		CGlCore::CGlCore(float_t zoom /*= 1.0*/, bool bShowCoord /*= false*/, bool bPerspective /*= false*/)
			: m_hrc(nullptr)
#if defined(_MSC_VER)
			, m_hdc(nullptr)
#elif defined(__linux__) // Linux
			, m_hdc({})
#endif
			, m_width(0), m_height(0), m_bEnableBlend(true), m_zNear(0), m_zFar(0)
		{
			m_camera.SetLength(zoom);
			m_camera.SetPerspective(bPerspective);

			RegisterCallbackInitializeGL(&CGlCore::OnInitializeGL, this);
			RegisterCallbackProjectionMatrix(&CGlCore::OnProjectionMatrix, this);
			RegisterCallbackModelMatrix(&CGlCore::OnModelMatrix, this);
			RegisterCallbackUpdateOption(&CGlCore::OnUpdateOption, this);
		}


		CGlCore::~CGlCore()
		{
			DestroyGlObject();
			Destroy();
		}

		bool CGlCore::Create(const GUI::WinDC& hdc)
		{
			bool bRet = false;

#if defined(_MSC_VER)
			if (!m_hdc)
			{
				if (WindowFromDC(hdc))
				{
					m_hdc = hdc;

					// Initialize view size
					RECT rect;
					GetClientRect(WindowFromDC(m_hdc), &rect);

					m_width = rect.right - rect.left;
					m_height = rect.bottom - rect.top;

					SetStretchBltMode(m_hdc, COLORONCOLOR);

					constexpr PIXELFORMATDESCRIPTOR pfd =
					{
						sizeof(PIXELFORMATDESCRIPTOR),					// Size of this structure
						1,												// Version of this structure
						PFD_DRAW_TO_WINDOW |							// Draw to window ( not Bitmap )
						PFD_SUPPORT_OPENGL |							// Support OpenGL calls in window
						PFD_DOUBLEBUFFER,								// Double buffered mode
						PFD_TYPE_RGBA,									// RGBA Color mode
						24,												// Want 24bit color
						0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0, 0,		// Not used to select mode
						32,												// Size of depth buffer
						0, 0,
						PFD_MAIN_PLANE,									// Draw in main plane
						0, 0, 0, 0	//Not used to select mode
					};

					const int32_t nPixelFormat = ChoosePixelFormat(m_hdc, &pfd);

					// Set the pixel format for the device context
					SetPixelFormat(m_hdc, nPixelFormat, &pfd);

					// Create the rendering context
					m_hrc = wglCreateContext(m_hdc);

					InitializeGL();
					bRet = true;
				}
			}

#else // Linux
			if (!m_hdc.display)
			{
				m_hrc = glXCreateContext(hdc.display, hdc.visualInfo, nullptr, GL_TRUE);
				m_hdc = hdc;

				InitializeGL();
				bRet = true;
			}
#endif

			return bRet;
		}

		void CGlCore::Destroy()
		{
			(void)EndContext();
			if (m_hrc)
			{
#if defined(_MSC_VER)
				wglDeleteContext(m_hrc);

				m_hdc = nullptr;
				m_hrc = nullptr;
#else // Linux
				glXDestroyContext(m_hdc.display, m_hrc);

				m_hdc.display = nullptr;
				m_hrc = nullptr;
#endif
			}

		}

		int CGlCore::GetWidth() const
		{
			return m_width;
		}

		bool CGlCore::IsCreated() const
		{
#if defined(_MSC_VER)
			return m_hdc && m_hrc;
#else // Linux
			return m_hdc.display && m_hrc;
#endif
		}

		int CGlCore::GetHeight() const
		{
			return m_height;
		}

		bool CGlCore::BeginContext() const
		{
			m_lcokGL.Lock();
			bool bRet = false;

			if (IsCreated())
			{
#if defined(_MSC_VER)
				bRet = wglMakeCurrent(m_hdc, m_hrc) == TRUE ? true : false;
#else // Linux
				bRet = glXMakeCurrent(m_hdc.display, m_hdc.window, m_hrc);
#endif
			}

			if (bRet == false)
			{
				m_lcokGL.UnLock();
			}

			return bRet;
		}

		bool CGlCore::EndContext() const
		{
#if defined(_MSC_VER)
			const bool bRet = wglMakeCurrent(nullptr, nullptr) ? true : false;
#else // Linux
			const bool bRet = glXMakeCurrent(m_hdc.display, None, nullptr);
#endif
			m_lcokGL.UnLock();

			return bRet;
		}

		void CGlCore::AddModel(const CGlModel& model)
		{
			m_lcokGL.Lock();
			m_models.emplace_back(model);
			m_lcokGL.UnLock();
		}


		void CGlCore::InitializeGL() const
		{
			// Make Current Context
			if (BeginContext())
			{
				// LoadIdentity
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();

				// Gl SetupRC
				// Black background
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

				// Set drawing color
				glColor3f(1.0f, 1.0f, 1.0f);

				// Depth test Enable
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);

				// Set ViewPort
				glViewport(0, 0, m_width, m_height);

				// Callback 
				m_callbackInitializeGL(*this);

				// Release Current Context
				(void)EndContext();
			}
		}

		void CGlCore::DestroyGlObject()
		{
			for (auto& model : m_models)
			{
				for (auto& mat : model.Materials())
				{
					if (*mat.m_glTextureID)
					{
						glDeleteTextures(1, &*mat.m_glTextureID);
						*mat.m_glTextureID = 0;
					}
				}

				for (auto& obj : model.Objects())
				{
					if (*obj.m_glListID)
					{
						glDeleteLists(*obj.m_glListID, 1);
						*obj.m_glListID = 0;
					}
				}
			}
		}

		void CGlCore::ReSize()
		{
			// BeginContext
			if (BeginContext())
			{
#if defined(_MSC_VER)
				RECT rect;
				GetClientRect(WindowFromDC(m_hdc), &rect);

				m_width = rect.right - rect.left;
				m_height = rect.bottom - rect.top;
#else // Linux
				XWindowAttributes gwa;
				XGetWindowAttributes(m_hdc.display, m_hdc.window, &gwa);

				m_width = gwa.width;
				m_height = gwa.height;
#endif

				if (m_height && m_width)
				{
					// Set viewport to window dimensions
					glViewport(0, 0, m_width, m_height);
				}

				(void)EndContext();
			}
		}

		void CGlCore::ClearBuffer() const
		{
			// Clear the window with current clearing color
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glDrawBuffer(GL_BACK);
		}

		void CGlCore::ProjectionMatrix()
		{
			//Reset coordinate system
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			const double_t aspectRate = static_cast<double_t>(GetWidth()) / static_cast<double_t>(GetHeight());

			if (m_camera.IsPerspective())
			{
				const double_t verticalFOV = 56.0 / aspectRate;

				m_zNear = 0.01 * static_cast<double_t>(m_camera.GetLength());
				m_zFar = 1000.0 * static_cast<double_t>(m_camera.GetLength());
				gluPerspective(verticalFOV, aspectRate, m_zNear, m_zFar);
			}
			else
			{
				if (GetWidth() > GetHeight())
				{
					const double_t left = -(0.5 * static_cast<double_t>(m_camera.GetLength()));
					const double_t right = -left;
					const double_t top = -(0.5 / aspectRate * static_cast<double_t>(m_camera.GetLength()));
					const double_t bottom = -top;

					m_zNear = 0.01 * static_cast<double_t>(m_camera.GetLength());
					m_zFar = 1000.0 * static_cast<double_t>(m_camera.GetLength());

					glOrtho(left, right, top, bottom, m_zNear, m_zFar);
				}
				else
				{
					const double_t left = -(0.5 * aspectRate * static_cast<double_t>(m_camera.GetLength()));
					const double_t right = -left;
					const double_t top = -(0.5 * static_cast<double_t>(m_camera.GetLength()));
					const double_t bottom = -top;
					
					m_zNear = 0.01 * static_cast<double_t>(m_camera.GetLength());
					m_zFar = 1000.0 * static_cast<double_t>(m_camera.GetLength());

					glOrtho(left, right, top, bottom, m_zNear, m_zFar);
				}
			}

			m_callbackProjectionMatrix(*this);
		}

		void CGlCore::ModelMatrix()
		{
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			CPoint3Df posCam = m_camera.PosCamera();
			CPoint3Df posLookAt = m_camera.PosLookAt();
			CPoint3Df VecUP = m_camera.VecUP();

			gluLookAt(posCam.x(), posCam.y(), posCam.z(),
				posLookAt.x(), posLookAt.y(), posLookAt.z(),
				VecUP.x(), VecUP.y(), VecUP.z());


			m_callbackModelMatrix(*this);
		}

		void CGlCore::UpdateOption()
		{
			/////////////////////
			/// Set Default Material
			glMaterialfv(GL_FRONT, GL_AMBIENT, CPoint4Df(0.2f, 0.2f, 0.2f, 1.0f).data());
			glMaterialfv(GL_FRONT, GL_DIFFUSE, CPoint4Df(0.8f, 0.8f, 0.8f, 1.0f).data());
			glMaterialfv(GL_FRONT, GL_SPECULAR, CPoint4Df(0.0f, 0.0f, 0.0f, 1.0f).data());
			glMaterialfv(GL_FRONT, GL_EMISSION, CPoint4Df(0.0f, 0.0f, 0.0f, 1.0f).data());

			glMaterialf(GL_FRONT, GL_SHININESS, 0);

			for (uint32_t i = 0; i < GL_MAX_LIGHT; i++)
			{
				CGlLight& light = m_vLight[i];

				if (light.IsEnable())
				{
					glEnable(GL_LIGHT0 + i);

					glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light.Ambient().data());
					glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light.Diffuse().data());
					glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light.Specular().data());
					glLightfv(GL_LIGHT0 + i, GL_POSITION, light.Position().data());

					//glLightf(GL_LIGHT0+ i, GL_CONSTANT_ATTENUATION, 0.0f);
					//glLightf(GL_LIGHT0+ i, GL_LINEAR_ATTENUATION, 1.0f);
					//glLightf(GL_LIGHT0+ i, GL_QUADRATIC_ATTENUATION, 1.0f);

					// SpotLight
					if (light.IsUsedSpotLight())
					{
						glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, light.SpotLightSpotCutOff());
						glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, light.SpotLightExponent());
						glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, light.SpotLightDirection().data());
					}
				}
				else
				{
					glDisable(GL_LIGHT0 + i);
				}
			}


			// Fog
			if( m_bEnableFog )
			{
				glEnable(GL_FOG);
				glHint(GL_FOG_HINT, GL_NICEST);

				glFogi(GL_FOG_MODE, m_fog.GetMode()); // GL_LINEAR, GL_EXP, GL_EXP2
				glFogfv(GL_FOG_COLOR, m_fog.GetColor().data());
				glFogf(GL_FOG_DENSITY, m_fog.GetDensity()); // for GL_EXP, GL_EXP2
				glFogf(GL_FOG_START, m_fog.GetNear());   // for GL_LINEAR
				glFogf(GL_FOG_END, m_fog.GetFar());     // for GL_LINEAR
			}
			else
			{
				glDisable(GL_FOG);
			}

			m_callbackUpdateOption(*this);
		}

		void CGlCore::ObjectDrawGL(DevLib::GL::CGlObject& object)
		{
			/////////////////////////
			/// 1. Draw option
			// Shade Model
			switch (object.GetShadeModel())
			{
			case GlShadeModel::FLAT:
				glShadeModel(GL_FLAT);
				break;
			case GlShadeModel::SMOOTH:
				glShadeModel(GL_SMOOTH);
				break;
			}

			// Lightning
			if (object.Lightning())
			{
				glEnable(GL_LIGHTING);

				glEnable(GL_AUTO_NORMAL);
				glEnable(GL_NORMALIZE);
				glEnable(GL_COLOR_MATERIAL);
			}
			else
			{
				glDisable(GL_LIGHTING);

				glDisable(GL_AUTO_NORMAL);
				glDisable(GL_NORMALIZE);
				glDisable(GL_COLOR_MATERIAL);
			}

			// Culling Option
			if (object.GetEnableCulling())
			{
				glEnable(GL_CULL_FACE);
			}
			else
			{
				glDisable(GL_CULL_FACE);
			}

			// Culling Option
			switch (object.m_cullFace)
			{
			case GL::GlCullingFace::CULL_BACK:
				glCullFace(GL_BACK);
				break;
			case GL::GlCullingFace::CULL_FRONT:
				glCullFace(GL_FRONT);
				break;
			case GL::GlCullingFace::CULL_BACK_AND_FRONT:
				glCullFace(GL_FRONT_AND_BACK);
				break;
			}

			switch (object.m_cullFrontFace)
			{
			case GL::GlCullingFrontFace::CULL_CW:
				glFrontFace(GL_CW);
				break;
			case GL::GlCullingFrontFace::CULL_CCW:
				glFrontFace(GL_CCW);
				break;
			}

			// DrawPointSize
			if (object.GetType() == GlObjectType::GlPoints)
			{
				glPointSize(object.GetDrawPointSize());
			}
			else // Line Width
			{
				glLineWidth(object.GetDrawPointSize());
			}

			// GL Enable
			// VertexArry
			glEnableClientState(GL_VERTEX_ARRAY);

			// Vertex 
			if (!object.Points().empty())
			{
				glVertexPointer(3, GL_FLOAT, 0, object.Points().data());
			}

			// Color
			if (!object.Colors().empty())
			{
				glEnableClientState(GL_COLOR_ARRAY);

				glColorPointer(4, GL_FLOAT, 0, object.Colors().data());
			}
			else
			{
				glColor4f(
					object.GetDrawColor().x(),
					object.GetDrawColor().y(),
					object.GetDrawColor().z(),
					object.GetDrawColor().w());
			}
			

			// GlTexture 
			if (!object.TextureCoords().empty())
			{
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);

				glTexCoordPointer(2, GL_FLOAT, 0, object.TextureCoords().data());
			}

			// Normal 
			if (!object.Normals().empty())
			{
				glEnableClientState(GL_NORMAL_ARRAY);

				glNormalPointer(GL_FLOAT, 0, object.Normals().data());
			}
			else // Default GlNormals 
			{

			}

			if (!object.Indices().empty())	// 
			{
				glDrawElements(
					ObjectTypeToGlType(object.GetType()),
					static_cast<GLsizei>(object.Indices().size()),
					GL_UNSIGNED_INT,
					object.Indices().data());
			}
			else if (!object.Points().empty())
			{
				// Draw Call
				glDrawArrays(ObjectTypeToGlType(object.GetType()), 0, static_cast<GLsizei>(object.Points().size()));
			}

			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
		}

		void CGlCore::ObjectMaterial(DevLib::GL::GlMaterials& materials, const std::string &materialName)
		{
			for (auto& mat : materials)
			{
				if (mat.GetName() == materialName)
				{
					if (*mat.m_glTextureID)
					{
						glEnable(GL_TEXTURE_2D);

						// Bind
						glBindTexture(GL_TEXTURE_2D, *mat.m_glTextureID);
					}

					const CPoint4Df frontDiffuse = mat.Diffuse();
					const CPoint4Df backDiffuse = mat.Diffuse();

					// Material
					glMaterialfv(GL_FRONT, GL_DIFFUSE, frontDiffuse.data());
					glMaterialfv(GL_BACK, GL_DIFFUSE, backDiffuse.data());

					glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat.Ambient().data());
					glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat.Specular().data());
					glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat.Emission().data());

					const auto frontShiness  = mat.Shininess();
					const auto backShiness = mat.Shininess() * 0.8f;
					glMaterialf(GL_FRONT, GL_SHININESS, frontShiness);
					glMaterialf(GL_BACK, GL_SHININESS, backShiness);
				}
			}
		}

		void CGlCore::ObjectListCall(DevLib::GL::CGlObject& object)
		{
			//////////////////////////////////////////////////////////////////////////
			// Matrix Push : Object
			glPushMatrix();
					

			// glMultMatrixf(static_cast<GLfloat*>(m_models.Models()->GetDataItr()->GetMatrix()));
			glTranslatef(object.Position().x(), object.Position().y(), object.Position().z());
			glRotatef(object.Rotation().y(), 0.0F, 1.0F, 0.0F);
			glRotatef(object.Rotation().z(), 0.0F, 0.0F, 1.0F);
			glRotatef(object.Rotation().x(), 1.0F, 0.0F, 0.0F);

			glScalef(object.Scale().x(), object.Scale().y(), object.Scale().z());

			if (*object.m_glListID)
			{
				// GL List Call
				glCallList(*object.m_glListID);
			}
			else
			{
				// GL Direct Draw
				ObjectDrawGL(object);
			}

			//////////////////////////////////////////////////////////////////////////
			// Matrix Pop : Object
			glPopMatrix();

		}

		void CGlCore::TextureCompile(DevLib::GL::CGlMaterial& mat)
		{
			// GlTexture Object 
			glGenTextures(1, &*mat.m_glTextureID);

			// Bind
			glBindTexture(GL_TEXTURE_2D, *mat.m_glTextureID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			// Default RGB ( 3 channels )
			GLint inter = GL_RGB;
			GLint input = GL_BGR_EXT;

			if (mat.GetTexture().GetChannels() == 4)
			{
				inter = GL_RGBA;
				input = GL_BGRA_EXT;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, inter,
				mat.GetTexture().GetWidth(),
				mat.GetTexture().GetHeight(),
				0, input, GL_UNSIGNED_BYTE,
				mat.GetTexture().GetImage());

			glBindTexture(GL_TEXTURE_2D, 0);

			// Compile 완료 후 메모리 제거 ( Object )
			mat.GetTexture().Destroy();
		}

		void CGlCore::ModelCompile()
		{
			for (auto& model : m_models)
			{
				// 1. Material Compile
				for (auto& mat : model.Materials())
				{
					if (*mat.m_glTextureID == 0)
					{
						if (!mat.GetTexture().empty()) // GlTexture 
						{
							TextureCompile(mat);
						}
					}
					else
					{
						if (!mat.GetTexture().empty()) // Update GlTexture 
						{
							// Bind
							glBindTexture(GL_TEXTURE_2D, *mat.m_glTextureID);

							// Default RGB ( 3 channels )
							GLint inter = GL_RGB;
							GLint input = GL_BGR_EXT;

							if (mat.GetTexture().GetChannels() == 4)
							{
								inter = GL_RGBA;
								input = GL_BGRA_EXT;
							}

							glTexImage2D(GL_TEXTURE_2D, 0, inter,
								mat.GetTexture().GetWidth(),
								mat.GetTexture().GetHeight(),
								0, input, GL_UNSIGNED_BYTE,
								mat.GetTexture().GetImage());

							glBindTexture(GL_TEXTURE_2D, 0);
						}
					}
				}

				// 2. Object Compile
				for (auto& object : model.Objects())
				{
					if (object.m_bCompile)
					{
						if (*object.m_glListID == 0)
						{
							//////////////////////////////////////////////////////////////////////////
							// Gl List
  							*object.m_glListID = glGenLists(1);
							glNewList(*object.m_glListID, GL_COMPILE_AND_EXECUTE);

							ObjectDrawGL(object);

							glEndList();

							// Compile 완료 후 메모리 제거 ( Object )
							object.Clear();
						}
					}
				}
			}
		}



#if defined(BLEND_Z_ODER)
		class CGlNoneBlendObject
		{
		public:
			CGlNoneBlendObject(CGlModel* pModel, CGlObject* pObject) : parentModel(pModel), object(pObject)
			{
			}

			CGlModel& Model() const { return *parentModel; }
			CGlObject& Object() const { return *object; }

		private:
			CGlModel* parentModel;
			CGlObject* object;
		};

		class CGlBlendObject : public CGlNoneBlendObject
		{
		public:
			CGlBlendObject(CGlModel* pModel, CGlObject* pObject, const CPoint3Df& point) : CGlNoneBlendObject(pModel, pObject)
			{
				auto& position = pModel->Position();
				distance = sqrt(pow(position.x() - point.x(), 2.0f) + pow(position.y() - point.y(), 2.0f) + pow(position.z() - point.z(), 2.0f));
			}

			float_t Distance() const
			{
				return distance;
			}

		private:
			float_t distance;
		};

		bool SortZOrder(const CGlBlendObject& a, const CGlBlendObject& b)
		{
			return a.Distance() > b.Distance();
		}

#endif

		void CGlCore::DrawModels()
		{
			ProjectionMatrix();
			ModelMatrix();
			UpdateOption();

			ModelCompile();

#if defined(BLEND_Z_ODER)
			std::vector<CGlNoneBlendObject> NoneBlendObjects;
			std::vector<CGlBlendObject> BlendObjects;
			for (auto& model : m_models)
			{
				for (auto& object : model.Objects())
				{
					if (object.Blend())
					{
						BlendObjects.emplace_back(&model, &object, m_camera.PosCamera());
					}
					else
					{
						NoneBlendObjects.emplace_back(&model, &object);
					}
				}
			}

			// Blend z-ordering 
			std::sort(BlendObjects.begin(), BlendObjects.end(), SortZOrder);

			////////////////////////////////////
			/// 1. Draw None-Alpha Blend Object
			/// Blend Off
			glDisable(GL_BLEND);

			for (auto& noneBlend : NoneBlendObjects)
			{
				auto& model = noneBlend.Model();
				auto& object = noneBlend.Object();

				//////////////////////////////////////////////////////////////////////////
				// Matrix Push : Model 
				glPushMatrix();

				glTranslatef(model.Position().x(), model.Position().y(), model.Position().z());

				glRotatef(model.Rotation().y(), 0.0F, 1.0F, 0.0F);
				glRotatef(model.Rotation().z(), 0.0F, 0.0F, 1.0F);
				glRotatef(model.Rotation().x(), 1.0F, 0.0F, 0.0F);

				glScalef(model.Scale().x(), model.Scale().y(), model.Scale().z());

				/////////////////////////
				///  Find Material
				ObjectMaterial(model.Materials(), object.GetMaterialName());


				// List Call
				ObjectListCall(object);


				/////////////////////////////
				// DeBind GlTexture
				// Default Bind
				glBindTexture(GL_TEXTURE_2D, 0);
				glDisable(GL_TEXTURE_2D);

				//////////////////////////////////////////////////////////////////////////
				// Matrix Pop : Model
				glPopMatrix();
			}


			////////////////////////////////////
			/// 2. Draw Alpha Blend Object
			// Blend On
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			for (auto& Blend : BlendObjects)
			{
				auto& model = Blend.Model();
				auto& object = Blend.Object();

				//////////////////////////////////////////////////////////////////////////
				// Matrix Push : Model 
				glPushMatrix();

				//glMultMatrixf(model.GetTransform().data());

				glTranslatef(model.Position().x(), model.Position().y(), model.Position().z());

				glRotatef(model.Rotation().y(), 0.0F, 1.0F, 0.0F);
				glRotatef(model.Rotation().z(), 0.0F, 0.0F, 1.0F);
				glRotatef(model.Rotation().x(), 1.0F, 0.0F, 0.0F);

				glScalef(model.Scale().x(), model.Scale().y(), model.Scale().z());

				/////////////////////////
				///  Find Material
				ObjectMaterial(model.Materials(), object.GetMaterialName());

				if (object.Lightning())
				{
					glDisable(GL_LIGHTING);
				}

				// List Call
				ObjectListCall(object);

				if (object.Lightning())
				{
					glEnable(GL_LIGHTING);
				}


				/////////////////////////////
				// DeBind GlTexture
				// Default Bind
				glBindTexture(GL_TEXTURE_2D, 0);
				glDisable(GL_TEXTURE_2D);

				//////////////////////////////////////////////////////////////////////////
				// Matrix Pop : Model
				glPopMatrix();
			}

			
#else

			if( m_bEnableBlend )
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				glDisable(GL_BLEND);
			}
			
			for (auto& model : m_models)
			{
				//////////////////////////////////////////////////////////////////////////
				// Matrix Push : Model 
				glPushMatrix();

				 //glMultMatrixf(model.GetTransform().data());

				glTranslatef(model.Position().x(), model.Position().y(), model.Position().z());
				glRotatef(model.Rotation().x(), 1.0F, 0.0F, 0.0F);
				glRotatef(model.Rotation().y(), 0.0F, 1.0F, 0.0F);
				glRotatef(model.Rotation().z(), 0.0F, 0.0F, 1.0F);

				glScalef(model.Scale().x(), model.Scale().y(), model.Scale().z());

				// Object Draw
				for (auto& object : model.Objects())
				{
					//////////////////////////////////////////////////////////////////////////
					// Matrix Push : Object
					glPushMatrix();

					///  Find Material
					ObjectMaterial(model.Materials(), object.GetMaterialName());

					// List Call
					ObjectListCall(object);

					/////////////////////////////
					// DeBind GlTexture
						// Default Bind
					glBindTexture(GL_TEXTURE_2D, 0);
					glDisable(GL_TEXTURE_2D);

					//////////////////////////////////////////////////////////////////////////
					// Matrix Pop : Object
					glPopMatrix();
				}

				//////////////////////////////////////////////////////////////////////////
				// Matrix Pop : Model
				glPopMatrix();
			}

			////////////////////////////////////
			/// 2. Draw LookAt Object
			if (m_bDrawLookAt)
			{
				m_objLookAt.SetPosition(m_camera.PosLookAt());
				ObjectListCall(m_objLookAt);
			}
#endif
		}

		void CGlCore::ClearModels()
		{
			if (BeginContext())
			{
				DestroyGlObject();

				m_models.clear();

				(void)EndContext();
			}
		}

		void CGlCore::SetPerspective(bool bPerspective)
		{
			m_camera.SetPerspective(bPerspective);
		}

		void CGlCore::SwapBuffer() const
		{
#if defined(_MSC_VER)
			SwapBuffers(m_hdc);
#else // Linux
			glXSwapBuffers(m_hdc.display, m_hdc.window);
#endif
		}

		CGlCamera CGlCore::SetCamera(const CGlCamera& camera)
		{
			CGlCamera oldCamera(m_camera);
			m_camera = camera;

			return oldCamera;
		}

		void CGlCore::RotationLookAt(float_t dx, float_t dy)
		{
			const float_t w = GL_PI / static_cast<float_t>(GetWidth());

			Camera().RotateLookAt(dx * w, dy * w);
		}

		void CGlCore::RotationCamera(float_t dx, float_t dy)
		{
			const float_t w = GL_PI / static_cast<float_t>(GetWidth());
			Camera().RotateCamera(dx * w, dy * w);
		}

		void CGlCore::TranslateCamera(float_t dx, float_t dy)
		{
			Camera().Move(dx * m_camera.GetLength() * 0.0005f, -dy * m_camera.GetLength() * 0.0005f, 0);
		}

		void CGlCore::EnableFog()
		{
			m_bEnableFog = true;
		}

		void CGlCore::DisableFog()
		{
			m_bEnableFog = false;
		}

		void CGlCore::SetFog(const CGlFog& fog)
		{
			m_fog = fog;
		}

		float_t CGlCore::Zoom(float_t fZoom)
		{
			const float_t old = m_camera.GetLength();
			m_camera.SetLength(old * fZoom);

			if (m_camera.IsPerspective())
			{
				m_camera.SetLength(m_camera.GetFieldOfView() * fZoom);
			}

			return old;
		}

		float_t CGlCore::SetZoom(float_t fZoom)
		{
			const float_t old = m_camera.GetLength();

			m_camera.SetLength(static_cast<float_t>(fZoom));

			if (m_camera.IsPerspective())
			{
				m_camera.SetLength(m_camera.GetFieldOfView() * fZoom);
			}

			return old;
		}

		void CGlCore::CaptureScreen(Graphics::CImage& capture, Graphics::ImageType type) const
		{
			if (BeginContext())
			{
				std::vector<uint8_t> buff;
				if (type == Graphics::ImageType::IMG_RGB)
				{
					buff.resize(static_cast<size_t>(GetWidth()) * static_cast<size_t>(GetHeight()) * 3);

					glReadPixels(0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, buff.data());
				}
				else
				{
					buff.resize(static_cast<size_t>(GetWidth()) * static_cast<size_t>(GetHeight()) * 4);

					glReadPixels(0, 0, m_width, m_height, GL_BGR_EXT, GL_UNSIGNED_BYTE, buff.data());
				}

				(void)EndContext();


				if (capture.GetWidth() != m_width || capture.GetHeight() != m_height || capture.GetImageType() != Graphics::ImageType::IMG_RGB)
				{
					capture.Destroy();
					capture.Create(GetWidth(), GetHeight(), type);
				}

				const size_t WidthStep = static_cast<size_t>(m_width) * 3;
				uint8_t* pDst = capture.GetImage();
				const uint8_t* pSrc = buff.data();
				const auto height = static_cast<size_t>(GetHeight());

				for (size_t y = 0; y < height; y++)
				{
					memcpy(&pDst[(height - 1 - y) * WidthStep], &pSrc[y * WidthStep], WidthStep);
				}
			}
		}

		void CGlCore::CaptureDepth(GlDepthBuffer& depth_buffer) const
		{
			if (BeginContext())
			{
				std::vector<float> buff;
				buff.resize(static_cast<size_t>(GetWidth()) * static_cast<size_t>(GetHeight()));

				glReadPixels(0, 0, m_width, m_height, GL_DEPTH_COMPONENT, GL_FLOAT, buff.data());

				(void)EndContext();

				depth_buffer.resize(m_width);

				for( auto& y : depth_buffer)
				{
					y.resize(m_height);
				}

				// relocation
				if (m_camera.IsPerspective())
				{
					for (int32_t y = 0; y < m_height; y++)
					{
						// Y-axis Flip
						for (int32_t x = 0; x < m_width; x++)
						{
							const auto z = static_cast<double_t>(buff[(m_height - 1 - y) * m_width + x]);
							depth_buffer[x][y] = static_cast<float_t>(((2.0 * m_zNear * m_zFar) / (m_zFar + m_zNear - z * (m_zFar- m_zNear))*0.5));
						}
					}
				}
				else
				{
					for (int32_t y = 0; y < m_height; y++)
					{
						// Y-axis Flip
						for (int32_t x = 0; x < m_width; x++)
						{
							const auto z = (2 * buff[(m_height - 1 - y) * m_width + x]) - 1;
							depth_buffer[x][y] = static_cast<float_t>(((z + (m_zFar + m_zNear) / (m_zFar - m_zNear)) * (m_zFar - m_zNear)) *0.5);
						}
					}
				}
			}
		}

		float_t CGlCore::GetZoomScale() const
		{
			return m_camera.GetLength();
		}

		float_t CGlCore::GetFieldOfView() const
		{
			return m_camera.GetFieldOfView();
		}

		void CGlCore::EnableBlend()
		{
			m_bEnableBlend = true;
		}

		void CGlCore::DisableBlend()
		{
			m_bEnableBlend = false;
		}

		CGlLight CGlCore::SetLight0(CGlLight light)
		{
			CGlLight old = m_vLight[0];
			m_vLight[0] = std::move(light);
			return old;
		}

		CGlLight CGlCore::SetLight1(CGlLight light)
		{
			CGlLight old = m_vLight[1];
			m_vLight[1] = std::move(light);
			return old;
		}

		CGlLight CGlCore::SetLight2(CGlLight light)
		{
			CGlLight old = m_vLight[2];
			m_vLight[2] = std::move(light);
			return old;
		}

		CGlLight CGlCore::SetLight3(CGlLight light)
		{
			CGlLight old = m_vLight[3];
			m_vLight[3] = std::move(light);
			return old;
		}

		CGlLight CGlCore::SetLight4(CGlLight light)
		{
			CGlLight old = m_vLight[4];
			m_vLight[4] = std::move(light);
			return old;
		}

		CGlLight CGlCore::SetLight5(CGlLight light)
		{
			CGlLight old = m_vLight[5];
			m_vLight[5] = std::move(light);
			return old;
		}

		CGlLight CGlCore::SetLight6(CGlLight light)
		{
			CGlLight old = m_vLight[6];
			m_vLight[6] = std::move(light);
			return old;
		}

		CGlLight CGlCore::SetLight7(CGlLight light)
		{
			CGlLight old = m_vLight[7];
			m_vLight[7] = std::move(light);
			return old;
		}
		
		float_t CGlCore::SetFieldOfView(float_t angle)
		{
			const float_t oldFOV = m_camera.GetFieldOfView();
			m_camera.SetFieldOfView(angle);

			ReSize();

			return oldFOV;
		}

		void CGlCore::SetBackgroundColor(float b, float g, float r, float alpha /*= 1.0f*/) const
		{
			if (BeginContext())
			{
				glClearColor(r, g, b, alpha);
				(void)EndContext();
			}
		}

	}
}