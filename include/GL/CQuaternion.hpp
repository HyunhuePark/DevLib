#pragma once

#include "../Base/CPointCloud.hpp"

namespace DevLib {
	namespace GL {

		class CQuaternion
		{
		public:
			CQuaternion();
			CQuaternion(float s, float x, float y, float z);
			~CQuaternion();

			float GetS() const;
			float GetX() const;
			float GetY() const;
			float GetZ() const;

			float* GetSXYZ();

			void SetS(float s);
			void SetX(float x);
			void SetY(float y);
			void SetZ(float z);
			void SetSXYZ(float s, float x, float y, float z);

			CQuaternion Pure() const;
			CQuaternion Conjugation() const;
			CQuaternion Inverse() const;
			CQuaternion Normal();
			float Length() const;
			float LengthSquare() const;

			static void GetRotateAxisX(CPoint3Df& vDst, const CPoint3Df& pSrc, float angle);
			static void GetRotateAxisY(CPoint3Df& vDst, const CPoint3Df& pSrc, float angle);
			static void GetRotateAxisZ(CPoint3Df& vDst, const CPoint3Df& pSrc, float angle);
			static void GetRotateAxis(CPoint3Df& vDst, const CPoint3Df& pSrc, const CPoint3Df& vAxis, float angle);

			static void NormalizeQuaternion(CQuaternion& Q);
			static void InverseQuaternion(CQuaternion& Q);

			CQuaternion& operator=(const CQuaternion& Q);
			CQuaternion operator+(const CQuaternion& Q) const;
			CQuaternion operator+(const float& F) const;
			CQuaternion operator-(const CQuaternion& Q) const;
			CQuaternion operator-(const float& F) const;
			CQuaternion operator*(const CQuaternion& Q) const;
			CQuaternion operator*(const float& F) const;
			CQuaternion operator/(const float& F) const;

		protected:
			union
			{
				struct
				{
					float m_fs;
					float m_fx;
					float m_fy;
					float m_fz;
				};
				float m_sxyz[4]{};
			};
		};

	}
}