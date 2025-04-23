#pragma once

#include "DevLibGLTypes.hpp"

namespace DevLib
{
	namespace GL
	{

		class CGlCamera
		{
		public:
			CGlCamera(std::string name = "GlCamera");

			void Reset();

			void Move(float_t left, float_t up, float_t front);
			void RotateLookAt(float_t dx, float_t dy);
			void RotateCamera(float_t dx, float_t dy);

			CPoint3Df& PosCamera();
			CPoint3Df& PosLookAt();
			CPoint3Df& VecUP();

			bool IsPerspective() const;

			void SetPerspective(bool bPerspective);

			void SetLength(float_t len);
			float_t GetLength() const;

			void SetFieldOfView(float_t angle);
			float_t GetFieldOfView() const;

			void SetDefaultCameraInfo(float_t posX, float_t posY, float_t posZ, float_t lookAtX, float_t lookAtY, float_t lookAtZ, float_t upX, float_t upY, float_t upZ);

			void SetCameraInfo(float_t posX, float_t posY, float_t posZ, float_t lookAtX, float_t lookAtY, float_t lookAtZ, float_t upX, float_t upY, float_t upZ);

		private:
			std::string m_cameraName;

			// 카메라 파라미터
			CPoint3Df m_posCam;
			CPoint3Df m_posLookAt;
			CPoint3Df m_vecUp;

			// 카메라 내부의 월드 up 벡터
			float_t m_verticalAngle{};
			CPoint3Df m_worldUp;

			// 초기 위치
			CPoint3Df m_defaultPosCam;
			CPoint3Df m_defaultPosLookAt;
			CPoint3Df m_defaultVecUp;

			float_t m_fov;

			// 투영 모드
			bool m_bPerspective;

		};

	}
}