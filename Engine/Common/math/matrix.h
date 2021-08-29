#pragma once
#include "vector_helper.h"
#include "vector.h"
#include "matrix_helper.h"

namespace redtea {
namespace math {

template <typename T>
class Matrix33 : public VecAddOperators<Matrix33, T>,
	VecComparisonOperators<Matrix33, T>,
	MatProductOperators<Matrix33, T, Vector3>, 
	MatSquareFunctions<Matrix33, T>,
	MatHelpers<Matrix33, T>
{
	typedef Vector3<T> col_type;
	typedef Vector3<T> row_type;
	static constexpr size_t COL_SIZE = col_type::SIZE;  // size of a column (i.e.: number of rows)
	static constexpr size_t ROW_SIZE = row_type::SIZE;  // size of a row (i.e.: number of columns)
	static constexpr size_t NUM_ROWS = COL_SIZE;
	static constexpr size_t NUM_COLS = ROW_SIZE;
public:
	col_type data[COL_SIZE];

	constexpr Matrix33() noexcept;

	template<typename U>
	constexpr explicit Matrix33(U v) noexcept;

	template<typename U>
	constexpr explicit Matrix33(const Vector3<U>& v) noexcept;

	template<typename A, typename B, typename C>
	constexpr Matrix33(const Vector3<A>& v0, const Vector3<B>& v1, const Vector3<C>& v2) noexcept;

	template<typename U>
	constexpr explicit Matrix33(const Matrix33<U>& rhs) noexcept;

	template<
		typename A, typename B, typename C,
		typename D, typename E, typename F,
		typename G, typename H, typename I>
	constexpr explicit Matrix33(A m00, B m01, C m02,
			D m10, E m11, F m12,
			G m20, H m21, I m22) noexcept;


	struct row_major_init
	{
		template<
			typename A, typename B, typename C,
			typename D, typename E, typename F,
			typename G, typename H, typename I>
			constexpr explicit row_major_init(A m00, B m01, C m02,
				D m10, E m11, F m12,
				G m20, H m21, I m22) noexcept
			: m(m00, m10, m20,
				m01, m11, m21,
				m02, m12, m22) {}
	private:
		friend Matrix33;
		Matrix33 m;
	};

	constexpr explicit Matrix33(row_major_init c) noexcept : Matrix33(std::move(c.m)) {}

	inline constexpr Vector3<T> const& operator[](size_t column) const noexcept
	{
		assert(column < NUM_COLS);
		return data[column];
	}

	inline constexpr col_type& operator[](size_t column) noexcept
	{
		assert(column < NUM_COLS);
		return data[column];
	}
};


template<typename T>
constexpr Matrix33<T>::Matrix33() noexcept
	: data{
	col_type(1, 0, 0),
	col_type(0, 1, 0),
	col_type(0, 0, 1) }
{
}

template<typename T>
template<typename U>
constexpr Matrix33<T>::Matrix33(U v) noexcept
	: data{
	col_type(v, 0, 0),
	col_type(0, v, 0),
	col_type(0, 0, v) }
{
}

template<typename T>
template<typename U>
constexpr Matrix33<T>::Matrix33(const Vector3<U>& v) noexcept
	: m_value{
	col_type(v[0], 0, 0),
	col_type(0, v[1], 0),
	col_type(0, 0, v[2]) }
{
}

// construct from 16 scalars. Note that the arrangement
// of values in the constructor is the transpose of the matrix
// notation.
template<typename T>
template<
	typename A, typename B, typename C,
	typename D, typename E, typename F,
	typename G, typename H, typename I>
constexpr Matrix33<T>::Matrix33(A m00, B m01, C m02,
		D m10, E m11, F m12,
		G m20, H m21, I m22) noexcept
	: m_value{
	col_type(m00, m01, m02),
	col_type(m10, m11, m12),
	col_type(m20, m21, m22) }
{
}

template<typename T>
template<typename U>
constexpr Matrix33<T>::Matrix33(const Matrix33<U>& rhs) noexcept
{
	for (size_t col = 0; col < NUM_COLS; ++col)
	{
		data[col] = col_type(rhs[col]);
	}
}

// Construct from 3 column vectors.
template<typename T>
template<typename A, typename B, typename C>
constexpr Matrix33<T>::Matrix33(const Vector3<A>& v0, const Vector3<B>& v1, const Vector3<C>& v2) noexcept
	: data{ v0, v1, v2 }
{
}


template <typename T>
class Matrix44 : public VecAddOperators<Matrix44, T>,
	VecComparisonOperators<Matrix44, T>,
	MatProductOperators<Matrix44, T, Vector4>,
	MatSquareFunctions<Matrix44, T>,
	MatHelpers<Matrix44, T>
{
	typedef Vector4<T> col_type;
	typedef Vector4<T> row_type;
	static constexpr size_t COL_SIZE = col_type::SIZE;  // size of a column (i.e.: number of rows)
	static constexpr size_t ROW_SIZE = row_type::SIZE;  // size of a row (i.e.: number of columns)
	static constexpr size_t NUM_ROWS = COL_SIZE;
	static constexpr size_t NUM_COLS = ROW_SIZE;
public:
	col_type data[COL_SIZE];

	constexpr Matrix44() noexcept;

	template<typename U>
	constexpr explicit Matrix44(U v) noexcept;

	template<typename U>
	constexpr explicit Matrix44(const Vector4<U>& v) noexcept;

	template<typename A, typename B, typename C, typename D>
	constexpr Matrix44(const Vector4<A>& v0, const Vector4<B>& v1, const Vector4<C>& v2, const Vector4<D> & v3) noexcept;

	template<typename U>
	constexpr explicit Matrix44(const Matrix44<U>& rhs) noexcept;

	template<
		typename A, typename B, typename C, typename D,
		typename E, typename F, typename G, typename H,
		typename I, typename J, typename K, typename L,
		typename M, typename N, typename O, typename P>
		constexpr explicit Matrix44(A m00, B m01, C m02, D m03,
			E m10, F m11, G m12, H m13,
			I m20, J m21, K m22, L m23,
			M m30, N m31, O m32, P m33) noexcept;

	/**
	 * construct from a 3x3 matrix
	 */
	template<typename U>
	constexpr explicit Matrix44(const Matrix33<U>& matrix) noexcept;

	/**
	 * construct from a 3x3 matrix and 3d translation
	 */
	template<typename U, typename V>
	constexpr Matrix44(const Matrix33<U>& matrix, const Vector3<V>& translation) noexcept;

	/**
	 * construct from a 3x3 matrix and 4d last column.
	 */
	template<typename U, typename V>
	constexpr Matrix44(const Matrix33<U>& matrix, const Vector4<V>& column3) noexcept;

	struct row_major_init
	{
		template<
			typename A, typename B, typename C, typename D,
			typename E, typename F, typename G, typename H,
			typename I, typename J, typename K, typename L,
			typename M, typename N, typename O, typename P>
			constexpr explicit row_major_init(A m00, B m01, C m02, D m03,
				E m10, F m11, G m12, H m13,
				I m20, J m21, K m22, L m23,
				M m30, N m31, O m32, P m33) noexcept
			: m(m00, m10, m20, m30,
				m01, m11, m21, m31,
				m02, m12, m22, m32,
				m03, m13, m23, m33) {}
	private:
		friend Matrix44;
		Matrix44 m;
	};

	constexpr explicit Matrix44(row_major_init c) noexcept : Matrix44(std::move(c.m)) {}

	inline constexpr Vector4<T> const& operator[](size_t column) const noexcept
	{
		assert(column < NUM_COLS);
		return data[column];
	}

	inline constexpr col_type& operator[](size_t column) noexcept
	{
		assert(column < NUM_COLS);
		return data[column];
	}
};

template<typename T>
constexpr Matrix44<T>::Matrix44() noexcept
	: data{
	col_type(1, 0, 0, 0),
	col_type(0, 1, 0, 0),
	col_type(0, 0, 1, 0),
	col_type(0, 0, 0, 1) } {
}

template<typename T>
template<typename U>
constexpr Matrix44<T>::Matrix44(U v) noexcept
	: data{
	col_type(v, 0, 0, 0),
	col_type(0, v, 0, 0),
	col_type(0, 0, v, 0),
	col_type(0, 0, 0, v) } {
}

template<typename T>
template<typename U>
constexpr Matrix44<T>::Matrix44(const Vector4<U>& v) noexcept
	: data{
	col_type(v[0], 0, 0, 0),
	col_type(0, v[1], 0, 0),
	col_type(0, 0, v[2], 0),
	col_type(0, 0, 0, v[3]) } {
}


// construct from 16 scalars
template<typename T>
template<
	typename A, typename B, typename C, typename D,
	typename E, typename F, typename G, typename H,
	typename I, typename J, typename K, typename L,
	typename M, typename N, typename O, typename P>
	constexpr Matrix44<T>::Matrix44(A m00, B m01, C m02, D m03,
		E m10, F m11, G m12, H m13,
		I m20, J m21, K m22, L m23,
		M m30, N m31, O m32, P m33) noexcept
	: data{
	col_type(m00, m01, m02, m03),
	col_type(m10, m11, m12, m13),
	col_type(m20, m21, m22, m23),
	col_type(m30, m31, m32, m33) } {
}

template<typename T>
template<typename U>
constexpr Matrix44<T>::Matrix44(const Matrix44<U>& rhs) noexcept {
	for (size_t col = 0; col < NUM_COLS; ++col) {
		data[col] = col_type(rhs[col]);
	}
}

// Construct from 4 column vectors.
template<typename T>
template<typename A, typename B, typename C, typename D>
constexpr Matrix44<T>::Matrix44(const Vector4<A>& v0, const Vector4<B>& v1,
	const Vector4<C>& v2, const Vector4<D>& v3) noexcept
	: data{ v0, v1, v2, v3 } {
}

template<typename T>
template<typename U>
constexpr Matrix44<T>::Matrix44(const Matrix33<U>& m) noexcept
	: data{
	col_type(m[0][0], m[0][1], m[0][2], 0),
	col_type(m[1][0], m[1][1], m[1][2], 0),
	col_type(m[2][0], m[2][1], m[2][2], 0),
	col_type(0, 0, 0, 1) }  // NOLINT
{
}

template<typename T>
template<typename U, typename V>
constexpr Matrix44<T>::Matrix44(const Matrix33<U>& m, const Vector3<V>& v) noexcept
	: data{
	col_type(m[0][0], m[0][1], m[0][2], 0),
	col_type(m[1][0], m[1][1], m[1][2], 0),
	col_type(m[2][0], m[2][1], m[2][2], 0),
	col_type(v[0], v[1], v[2], 1) }  // NOLINT
{
}

template<typename T>
template<typename U, typename V>
constexpr Matrix44<T>::Matrix44(const Matrix33<U>& m, const Vector4<V>& v) noexcept
	: data{
	col_type(m[0][0], m[0][1], m[0][2], 0),
	col_type(m[1][0], m[1][1], m[1][2], 0),
	col_type(m[2][0], m[2][1], m[2][2], 0),
	col_type(v[0], v[1], v[2], v[3]) }  // NOLINT
{
}


}
}