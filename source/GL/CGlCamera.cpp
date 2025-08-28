#include "../../include/GL/CGlCamera.hpp"
#include "../../include/GL/CQuaternion.hpp"

#include "../../include/3rd/Eigen3/include/eigen3/Eigen/Core"

namespace DevLib {

	namespace GL
	{
		CGlCamera::CGlCamera(std::string name)
			: m_cameraName(std::move(name))
			, m_fov(56.0f), m_bPerspective(false)
		{
			SetDefaultCameraInfo(0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F);
			Reset();
		}

		void CGlCamera::Reset()
		{
			m_posLookAt = m_defaultPosLookAt;
			m_posCam = m_defaultPosCam;
			m_vecUp = m_defaultVecUp;

			m_worldUp = m_vecUp;
			m_verticalAngle = 0;
		}

		void CGlCamera::Move(const float_t left, const float_t up, const float_t front)
		{
			// 현재 바라보는 방향과 위 방향을 기준으로 이동한다.
			const CPoint3Df vLook{ m_posLookAt - m_posCam };
			const CPoint3Df vLeft = vLook.cross(m_vecUp);

			const CPoint3Df move{ (vLook.normalized() * front) + (vLeft.normalized() * left) + (m_vecUp.normalized() * up) };
			m_posCam = m_posCam + move;
			m_posLookAt = m_posLookAt + move;
		}

		void CGlCamera::RotateLookAt(const float_t dx, float_t dy)
		{
			// 가로 방향 회전
			CPoint3Df vLook{ m_posLookAt - m_posCam };
			CQuaternion::GetRotateAxis(vLook, vLook, m_worldUp, dx);
			CQuaternion::GetRotateAxis(m_vecUp, m_vecUp, m_worldUp, dx);

			// 세로 방향 회전
			const float_t nextAngle = m_verticalAngle + dy;

			if (-50 < nextAngle && nextAngle < 50)
			{
				const CPoint3Df vLeft = vLook.cross(m_vecUp);
				CQuaternion::GetRotateAxis(vLook, vLook, vLeft, dy);
				CQuaternion::GetRotateAxis(m_vecUp, m_vecUp, vLeft, dy);
				m_verticalAngle = nextAngle;
			}


			m_posLookAt = m_posCam + vLook;
		}

		void CGlCamera::RotateCamera(const float_t dx, const float_t dy)
		{
			// 가로 방향 회전
			CPoint3Df vLook{ m_posLookAt - m_posCam };
			CQuaternion::GetRotateAxis(vLook, vLook, m_vecUp, dx);
			CQuaternion::GetRotateAxis(m_worldUp, m_worldUp, m_vecUp, dx);

			// 세로 방향 회전
			const CPoint3Df vLeft = vLook.cross(m_vecUp).normalized();
			CQuaternion::GetRotateAxis(vLook, vLook, vLeft, dy);
			CQuaternion::GetRotateAxis(m_vecUp, m_vecUp, vLeft, dy); // Up 벡터 회전
			CQuaternion::GetRotateAxis(m_worldUp, m_worldUp, vLeft, dy); // Up 벡터 회전

			m_posCam = m_posLookAt - vLook;
		}

		CPoint3Df& CGlCamera::PosCamera()
		{
			return m_posCam;
		}

		CPoint3Df& CGlCamera::PosLookAt()
		{
			return m_posLookAt;
		}

		CPoint3Df& CGlCamera::VecUP()
		{
			return m_vecUp;
		}

		bool CGlCamera::IsPerspective() const
		{
			return m_bPerspective;
		}

		void CGlCamera::SetPerspective(bool bPerspective)
		{
			m_bPerspective = bPerspective;
		}

		void CGlCamera::SetLength(float_t len)
		{
			CPoint3Df vLook{ m_posLookAt - m_posCam };
			vLook = vLook.normalized() * len;
			m_posCam = m_posLookAt - vLook;
		}

		float_t CGlCamera::GetLength() const
		{
			const CPoint3Df vLook{ m_posCam - m_posLookAt };
			return  vLook.norm();
		}

		void CGlCamera::SetFieldOfView(float_t angle)
		{
			m_fov = angle;
		}

		float_t CGlCamera::GetFieldOfView() const
		{
			return m_fov;
		}

		void CGlCamera::SetDefaultCameraInfo(const float_t posX, const float_t posY, const float_t posZ,
			const float_t lookAtX, const float_t lookAtY, const float_t lookAtZ,
			const float_t upX, const float_t upY, const float_t upZ)
		{
			m_defaultPosCam = CPoint3Df(posX, posY, posZ);
			m_defaultPosLookAt = CPoint3Df(lookAtX, lookAtY, lookAtZ);
			m_defaultVecUp = CPoint3Df(upX, upY, upZ);
		}

		void CGlCamera::SetCameraInfo(const float_t posX, const float_t posY, const float_t posZ,
			const float_t lookAtX, const float_t lookAtY, const float_t lookAtZ,
			const float_t upX, const float_t upY, const float_t upZ)
		{
			m_posCam = CPoint3Df(posX, posY, posZ);
			m_posLookAt = CPoint3Df(lookAtX, lookAtY, lookAtZ);
			m_vecUp = CPoint3Df(upX, upY, upZ);
		}
	}
}
