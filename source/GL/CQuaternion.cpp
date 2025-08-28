#include "../../include/GL/CQuaternion.hpp"

namespace DevLib {
	namespace GL {

		// 축 벡터
		static const CPoint3Df g_vAxisX(1, 0, 0);
		static const CPoint3Df g_vAxisY(0, 1, 0);
		static const CPoint3Df g_vAxisZ(0, 0, 1);

		CQuaternion::CQuaternion()
			:m_fs(0), m_fx(0), m_fy(0), m_fz(0)
		{
		}

		CQuaternion::CQuaternion(float s, float x, float y, float z)
			: m_fs(s), m_fx(x), m_fy(y), m_fz(z)
		{
		}

		CQuaternion::~CQuaternion()
			= default;

		inline float CQuaternion::GetS() const
		{
			return m_fs;
		}

		inline float CQuaternion::GetX() const
		{
			return m_fx;
		}

		inline float CQuaternion::GetY() const
		{
			return m_fy;
		}

		inline float CQuaternion::GetZ() const
		{
			return m_fz;
		}

		float* CQuaternion::GetSXYZ()
		{
			return m_sxyz;
		}

		inline void CQuaternion::SetS(float s)
		{
			m_fs = s;
		}

		inline void CQuaternion::SetX(float x)
		{
			m_fx = x;
		}

		inline void CQuaternion::SetY(float y)
		{
			m_fy = y;
		}

		inline void CQuaternion::SetZ(float z)
		{
			m_fz = z;
		}

		inline void CQuaternion::SetSXYZ(float s, float x, float y, float z)
		{
			m_fs = x;	m_fx = x;	m_fy = y;	m_fz = z;
		}

		CQuaternion& CQuaternion::operator=(const CQuaternion& Q)
		{
			if (this != &Q)
			{
				m_fs = Q.m_fs;
				m_fx = Q.m_fx;
				m_fy = Q.m_fy;
				m_fz = Q.m_fz;
			}
			return *this;
		}

		CQuaternion CQuaternion::Pure() const
		{
			CQuaternion quat(0, m_fx, m_fy, m_fz);
			return quat;
		}

		CQuaternion CQuaternion::Conjugation() const
		{
			return { m_fs, -m_fx, -m_fy, -m_fz };
		}

		CQuaternion CQuaternion::Inverse() const
		{
			const float lenSq = LengthSquare();
			if (lenSq == 0)
			{
				return Conjugation();
			}

			return Conjugation() / sqrt(lenSq);
		}

		CQuaternion CQuaternion::Normal()
		{
			float leg = LengthSquare();
			if (leg == 0) return { m_fs, m_fx, m_fy, m_fz };

			leg = 1 / sqrt(leg);
			return { m_fs * leg, m_fx * leg, m_fy * leg, m_fz * leg };
		}

		float CQuaternion::Length() const
		{
			return sqrt(m_fs * m_fs + m_fx * m_fx + m_fy * m_fy + m_fz * m_fz);
		}

		float CQuaternion::LengthSquare() const
		{
			return m_fs * m_fs + m_fx * m_fx + m_fy * m_fy + m_fz * m_fz;
		}

		CQuaternion CQuaternion::operator+(const CQuaternion& Q) const
		{
			CQuaternion quaternion(m_fs + Q.m_fs, m_fx + Q.m_fx, m_fy + Q.m_fy, m_fz + Q.m_fz);
			return quaternion;
		}

		CQuaternion CQuaternion::operator+(const float& F) const
		{
			CQuaternion quaternion(m_fs + F, m_fx, m_fy, m_fz);
			return quaternion;
		}

		CQuaternion CQuaternion::operator-(const CQuaternion& Q) const
		{
			CQuaternion quaternion(m_fs - Q.m_fs, m_fx - Q.m_fx, m_fy - Q.m_fy, m_fz - Q.m_fz);
			return quaternion;
		}

		CQuaternion CQuaternion::operator-(const float& F) const
		{
			CQuaternion quaternion(m_fs - F, m_fx, m_fy, m_fz);
			return quaternion;
		}

		CQuaternion CQuaternion::operator*(const float& f) const
		{
			CQuaternion quaternion(m_fs * f, m_fx * f, m_fy * f, m_fz * f);
			return quaternion;
		}

		CQuaternion CQuaternion::operator*(const CQuaternion& Q) const
		{
			CQuaternion quaternion;
			quaternion.m_fs = (m_fs * Q.m_fs) - (m_fx * Q.m_fx) - (m_fy * Q.m_fy) - (m_fz * Q.m_fz);
			quaternion.m_fx = (m_fs * Q.m_fx) + (m_fx * Q.m_fs) + (m_fy * Q.m_fz) - (m_fz * Q.m_fy);
			quaternion.m_fy = (m_fs * Q.m_fy) + (m_fy * Q.m_fs) + (m_fz * Q.m_fx) - (m_fx * Q.m_fz);
			quaternion.m_fz = (m_fs * Q.m_fz) + (m_fz * Q.m_fs) + (m_fx * Q.m_fy) - (m_fy * Q.m_fx);
			return quaternion;
		}

		CQuaternion CQuaternion::operator/(const float& f) const
		{
			CQuaternion quaternion(m_fs / f, m_fx / f, m_fy / f, m_fz / f);
			return quaternion;
		}

		void CQuaternion::GetRotateAxisX(CPoint3Df& vDst, const CPoint3Df& pSrc, const float angle)
		{
			GetRotateAxis(vDst, pSrc, g_vAxisX, angle);
		}

		void CQuaternion::GetRotateAxisY(CPoint3Df& vDst, const CPoint3Df& pSrc, const float angle)
		{
			GetRotateAxis(vDst, pSrc, g_vAxisY, angle);
		}

		void CQuaternion::GetRotateAxisZ(CPoint3Df& vDst, const CPoint3Df& pSrc, const float angle)
		{
			GetRotateAxis(vDst, pSrc, g_vAxisZ, angle);
		}

		void CQuaternion::GetRotateAxis(CPoint3Df& vDst, const CPoint3Df& pSrc, const CPoint3Df& vAxis, const float angle)
		{
			const CQuaternion p1(0, pSrc.x(), pSrc.y(), pSrc.z());

			// 축 쿼터니언
			CQuaternion axis(0, vAxis.x(), vAxis.y(), vAxis.z());

			// 회전 쿼터니언
			const CQuaternion qa = (axis.Normal() * sinf(angle * 0.5F) + cosf(angle * 0.5F));

			// 회전 결과
			const CQuaternion p2 = qa * p1 * qa.Inverse();

			vDst = CPoint3Df(p2.GetX(), p2.GetY(), p2.GetZ());
		}

		void CQuaternion::NormalizeQuaternion(CQuaternion& Q)
		{
			float length = Q.LengthSquare();
			if (length == 0) return;

			length = 1.0f / sqrt(length);
			Q.SetSXYZ(Q.GetS() * length, Q.GetX() * length, Q.GetY() * length, Q.GetZ() * length);
		}

		void CQuaternion::InverseQuaternion(CQuaternion& Q)
		{
			float length = Q.LengthSquare();

			if (length)
			{
				Q.SetSXYZ(Q.GetS(), -Q.GetX(), -Q.GetY(), -Q.GetZ());
			}
			else
			{
				length = -1.0f / sqrt(length);
				Q.SetSXYZ(Q.GetS(), Q.GetX() * length, Q.GetY() * length, Q.GetZ() * length);
			}
		}


	}
}