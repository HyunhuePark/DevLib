#pragma once
#include <vector>
#include "CMatrix.hpp"
#include "CPoint.hpp"

namespace DevLib
{
	template < class Type = CPoint3Df >
	class CPointCloud
	{
	public:
		CPointCloud()
			= default;

		using AlignedVector = std::vector<Type, Eigen::aligned_allocator<Type>>;

		typename Type::Scalar* GetRawPtr() { return m_vPoints.data()->data(); }

		size_t size()
		{
			return m_vPoints.size();
		}

		void resize(const size_t size)
		{
			m_vPoints.resize(size);
		}

		void reserve(const size_t size)
		{
			m_vPoints.reserve(size);
		}

		void push_back(const Type& point)
		{
			m_vPoints.push_back(point);
		}

		using iterator = typename AlignedVector::iterator;

		template <class InputIterator>
		void
			insert(iterator position, InputIterator first, InputIterator last)
		{
			m_vPoints.insert(position, first, last);
		}

		iterator begin() noexcept { return m_vPoints.begin(); }

		iterator end() noexcept { return m_vPoints.end(); }

		Type& operator[](const int _idx)
		{
			return m_vPoints[_idx];
		}

		AlignedVector& GetPoints()
		{
			return m_vPoints;
		}

		AlignedVector* operator->()
		{
			return &m_vPoints;
		}

		// Matrix Operator
		CPointCloud<CPoint> operator*(const CMatrix22& mat) const
		{
			CPointCloud<CPoint> ret;
			ret.GetPoints().resize(m_vPoints.size());
			size_t nPos = 0;

			std::for_each(m_vPoints.begin(), m_vPoints.end(), [&](const CPoint& point) { ret.GetPoints()[nPos] = mat * point; nPos++; });

			return ret;
		}

		CPointCloud<CPoint3D> operator*(const CMatrix33& mat) const
		{
			CPointCloud<CPoint3D> ret;
			ret.GetPoints().resize(m_vPoints.size());
			size_t nPos = 0;

			std::for_each(m_vPoints.begin(), m_vPoints.end(), [&](const CPoint3D& point) { ret.GetPoints()[nPos] = mat * point; nPos++; });
			return ret;
		}

		CPointCloud<CPoint4D> operator*(const CMatrix44& mat) const
		{
			CPointCloud<CPoint4D> ret;
			ret.GetPoints().resize(m_vPoints.size());
			size_t nPos = 0;

			std::for_each(m_vPoints.begin(), m_vPoints.end(), [&](const CPoint4D& point) { ret.GetPoints()[nPos] = mat * point; nPos++; });

			return ret;
		}

		CPointCloud<CPointf> operator*(const CMatrix22f& mat) const
		{
			CPointCloud<CPointf> ret;
			ret.GetPoints().resize(m_vPoints.size());
			size_t nPos = 0;

			std::for_each(m_vPoints.begin(), m_vPoints.end(), [&](const CPointf& point) { ret.GetPoints()[nPos] = mat * point; nPos++; });

			return ret;
		}

		CPointCloud<CPoint3Df> operator*(const CMatrix33f& mat) const
		{
			CPointCloud<CPoint3Df> ret;
			ret.GetPoints().resize(m_vPoints.size());
			size_t nPos = 0;

			std::for_each(m_vPoints.begin(), m_vPoints.end(), [&](const CPoint3Df& point) { ret.GetPoints()[nPos] = mat * point; nPos++; });

			return ret;
		}

		CPointCloud<CPoint3Df> operator+(const CPoint3Df& vec) const
		{
			CPointCloud<CPoint3Df> ret;
			ret.GetPoints().resize(m_vPoints.size());
			size_t nPos = 0;

			std::for_each(m_vPoints.begin(), m_vPoints.end(), [&](const CPoint3Df& point) { ret.GetPoints()[nPos] = vec + point; nPos++; });

			return ret;
		}

		CPointCloud<CPoint4Df> operator*(const CMatrix44f& mat) const
		{
			CPointCloud<CPoint4Df> ret;
			ret.GetPoints().resize(m_vPoints.size());
			size_t nPos = 0;

			std::for_each(m_vPoints.begin(), m_vPoints.end(), [&](const CPoint4Df& point) { ret.GetPoints()[nPos] = mat * point; nPos++; });

			return ret;
		}

		CPointCloud<CPointd> operator*(const CMatrix22d& mat) const
		{
			CPointCloud<CPointd> ret;
			ret.GetPoints().resize(m_vPoints.size());
			size_t nPos = 0;

			std::for_each(m_vPoints.begin(), m_vPoints.end(), [&](const CPointd& point) { ret.GetPoints()[nPos] = mat * point; nPos++; });

			return ret;
		}

		CPointCloud<CPoint3Dd> operator*(const CMatrix33d& mat) const
		{
			CPointCloud<CPoint3Dd> ret;
			ret.GetPoints().resize(m_vPoints.size());
			size_t nPos = 0;

			std::for_each(m_vPoints.begin(), m_vPoints.end(), [&](const CPoint3Dd& point) { ret.GetPoints()[nPos] = mat * point; nPos++; });

			return ret;
		}

		CPointCloud<CPoint4Dd> operator*(const CMatrix44d& mat) const
		{
			CPointCloud<CPoint4Dd> ret;
			ret.GetPoints().resize(m_vPoints.size());
			size_t nPos = 0;

			std::for_each(m_vPoints.begin(), m_vPoints.end(), [&](const CPoint4Dd& point) { ret.GetPoints()[nPos] = mat * point; nPos++; });

			return ret;
		}

	private:
		AlignedVector m_vPoints;
		EnableSmartPointer(CPointCloud<Type>)
	};
}