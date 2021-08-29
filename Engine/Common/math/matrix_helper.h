#pragma once
#include <type_traits>

namespace redtea {
namespace math {
namespace matrix {
/*
	* Matrix inversion
	*/
template<typename MATRIX>
constexpr MATRIX gaussJordanInverse(MATRIX src) 
{
	typedef typename MATRIX::value_type T;
	constexpr unsigned int N = MATRIX::NUM_ROWS;
	MATRIX inverted;

	for (size_t i = 0; i < N; ++i) {
		// look for largest element in i'th column
		size_t swap = i;
		T t = src[i][i] < 0 ? -src[i][i] : src[i][i];
		for (size_t j = i + 1; j < N; ++j) {
			const T t2 = src[j][i] < 0 ? -src[j][i] : src[j][i];
			if (t2 > t) {
				swap = j;
				t = t2;
			}
		}

		if (swap != i) {
			// swap columns.
			std::swap(src[i], src[swap]);
			std::swap(inverted[i], inverted[swap]);
		}

		const T denom(src[i][i]);
		for (size_t k = 0; k < N; ++k) {
			src[i][k] /= denom;
			inverted[i][k] /= denom;
		}

		// Factor out the lower triangle
		for (size_t j = 0; j < N; ++j) {
			if (j != i) {
				const T t = src[j][i];
				for (size_t k = 0; k < N; ++k) {
					src[j][k] -= src[i][k] * t;
					inverted[j][k] -= inverted[i][k] * t;
				}
			}
		}
	}

	return inverted;
}

//------------------------------------------------------------------------------
// 2x2 matrix inverse is easy.
template<typename MATRIX>
constexpr MATRIX  fastInverse2(const MATRIX& x)
{
	typedef typename MATRIX::value_type T;

	// Assuming the input matrix is:
	// | a b |
	// | c d |
	//
	// The analytic inverse is
	// | d -b |
	// | -c a | / (a d - b c)
	//
	// Importantly, our matrices are column-major!

	MATRIX inverted{};

	const T a = x[0][0];
	const T c = x[0][1];
	const T b = x[1][0];
	const T d = x[1][1];

	const T det((a * d) - (b * c));
	inverted[0][0] = d / det;
	inverted[0][1] = -c / det;
	inverted[1][0] = -b / det;
	inverted[1][1] = a / det;
	return inverted;
}

//------------------------------------------------------------------------------
// From the Wikipedia article on matrix inversion's section on fast 3x3
// matrix inversion:
// http://en.wikipedia.org/wiki/Invertible_matrix#Inversion_of_3.C3.973_matrices
template<typename MATRIX>
constexpr MATRIX fastInverse3(const MATRIX& x)
{
	typedef typename MATRIX::value_type T;

	// Assuming the input matrix is:
	// | a b c |
	// | d e f |
	// | g h i |
	//
	// The analytic inverse is
	// | A B C |^T
	// | D E F |
	// | G H I | / determinant
	//
	// Which is
	// | A D G |
	// | B E H |
	// | C F I | / determinant
	//
	// Where:
	// A = (ei - fh), B = (fg - di), C = (dh - eg)
	// D = (ch - bi), E = (ai - cg), F = (bg - ah)
	// G = (bf - ce), H = (cd - af), I = (ae - bd)
	//
	// and the determinant is a*A + b*B + c*C (The rule of Sarrus)
	//
	// Importantly, our matrices are column-major!

	MATRIX inverted{};

	const T a = x[0][0];
	const T b = x[1][0];
	const T c = x[2][0];
	const T d = x[0][1];
	const T e = x[1][1];
	const T f = x[2][1];
	const T g = x[0][2];
	const T h = x[1][2];
	const T i = x[2][2];

	// Do the full analytic inverse
	const T A = e * i - f * h;
	const T B = f * g - d * i;
	const T C = d * h - e * g;
	inverted[0][0] = A;                 // A
	inverted[0][1] = B;                 // B
	inverted[0][2] = C;                 // C
	inverted[1][0] = c * h - b * i;     // D
	inverted[1][1] = a * i - c * g;     // E
	inverted[1][2] = b * g - a * h;     // F
	inverted[2][0] = b * f - c * e;     // G
	inverted[2][1] = c * d - a * f;     // H
	inverted[2][2] = a * e - b * d;     // I

	const T det(a * A + b * B + c * C);
	for (size_t col = 0; col < 3; ++col)
	{
		for (size_t row = 0; row < 3; ++row)
		{
			inverted[col][row] /= det;
		}
	}

	return inverted;
}


//------------------------------------------------------------------------------
// Determinant and cofactor

// this is just a dummy matrix helper
template<typename T, size_t ORDER>
class Matrix 
{
	T m[ORDER][ORDER];
public:
	constexpr auto operator[](size_t i) const noexcept { return m[i]; }

	constexpr auto& operator[](size_t i) noexcept { return m[i]; }

	static constexpr Matrix<T, ORDER - 1> submatrix(Matrix in, size_t row, size_t col) noexcept
	{
		size_t colCount = 0, rowCount = 0;
		Matrix<T, ORDER - 1> dest{};
		for (size_t i = 0; i < ORDER; i++)
		{
			if (i != row) {
				colCount = 0;
				for (size_t j = 0; j < ORDER; j++)
				{
					if (j != col)
					{
						dest[rowCount][colCount] = in[i][j];
						colCount++;
					}
				}
				rowCount++;
			}
		}
		return dest;
	}
};

template<typename T, size_t O>
struct Determinant
{
	static constexpr T determinant(Matrix<T, O> in)
	{
		T det = {};
		for (size_t i = 0; i < O; i++) {
			T m = Determinant<T, O - 1>::determinant(Matrix<T, O>::submatrix(in, 0, i));
			T factor = (i % 2 == 1) ? T(-1) : T(1);
			det += factor * in[0][i] * m;
		}
		return det;
	}
};

template<typename T>
struct Determinant<T, 3> {
	static constexpr T determinant(Matrix<T, 3> in) 
	{
		return
			in[0][0] * in[1][1] * in[2][2] +
			in[1][0] * in[2][1] * in[0][2] +
			in[2][0] * in[0][1] * in[1][2] -
			in[2][0] * in[1][1] * in[0][2] -
			in[1][0] * in[0][1] * in[2][2] -
			in[0][0] * in[2][1] * in[1][2];
	}
};

template<typename T>
struct Determinant<T, 2> {
	static constexpr T determinant(Matrix<T, 2> in)
	{
		return in[0][0] * in[1][1] - in[0][1] * in[1][0];
	}
};

template<typename T>
struct Determinant<T, 1> {
	static constexpr T determinant(Matrix<T, 1> in) { return in[0][0]; }
};

template<typename MATRIX>
constexpr MATRIX cofactor(const MATRIX& m)
{
	typedef typename MATRIX::value_type T;

	MATRIX out;
	constexpr size_t order = MATRIX::NUM_COLS;

	Matrix<T, order> in{};
	for (size_t i = 0; i < order; i++)
	{
		for (size_t j = 0; j < order; j++)
		{
			in[i][j] = m[i][j];
		}
	}

	for (size_t i = 0; i < order; i++)
	{
		for (size_t j = 0; j < order; j++)
		{
			T factor = ((i + j) % 2 == 1) ? T(-1) : T(1);
			out[i][j] = Determinant<T, order - 1>::determinant(
				Matrix<T, order>::submatrix(in, i, j)) * factor;
		}
	}
	return out;
}

template<typename MATRIX>
constexpr MATRIX  fastCofactor2(const MATRIX& m)
{
	typedef typename MATRIX::value_type T;

	// Assuming the input matrix is:
	// | a b |
	// | c d |
	//
	// The cofactor are
	// | d -c |
	// | -b a |
	//
	// Importantly, our matrices are column-major!

	MATRIX cof{};

	const T a = m[0][0];
	const T c = m[0][1];
	const T b = m[1][0];
	const T d = m[1][1];

	cof[0][0] = d;
	cof[0][1] = -b;
	cof[1][0] = -c;
	cof[1][1] = a;
	return cof;
}

template<typename MATRIX>
constexpr MATRIX fastCofactor3(const MATRIX& m)
{
	typedef typename MATRIX::value_type T;

	// Assuming the input matrix is:
	// | a b c |
	// | d e f |
	// | g h i |
	//
	// The cofactor are
	// | A B C |
	// | D E F |
	// | G H I |

	// Where:
	// A = (ei - fh), B = (fg - di), C = (dh - eg)
	// D = (ch - bi), E = (ai - cg), F = (bg - ah)
	// G = (bf - ce), H = (cd - af), I = (ae - bd)

	// Importantly, our matrices are column-major!

	MATRIX cof{};

	const T a = m[0][0];
	const T b = m[1][0];
	const T c = m[2][0];
	const T d = m[0][1];
	const T e = m[1][1];
	const T f = m[2][1];
	const T g = m[0][2];
	const T h = m[1][2];
	const T i = m[2][2];

	cof[0][0] = e * i - f * h;  // A
	cof[0][1] = c * h - b * i;  // D
	cof[0][2] = b * f - c * e;  // G
	cof[1][0] = f * g - d * i;  // B
	cof[1][1] = a * i - c * g;  // E
	cof[1][2] = c * d - a * f;  // H
	cof[2][0] = d * h - e * g;  // C
	cof[2][1] = b * g - a * h;  // F
	cof[2][2] = a * e - b * d;  // I

	return cof;
}


/**
	* Cofactor function which switches on the matrix size.
	*/
template<typename MATRIX,
	typename = std::enable_if_t<MATRIX::NUM_ROWS == MATRIX::NUM_COLS, int>>
	inline constexpr MATRIX  cof(const MATRIX& matrix) {
	return (MATRIX::NUM_ROWS == 2) ? fastCofactor2<MATRIX>(matrix) :
		((MATRIX::NUM_ROWS == 3) ? fastCofactor3<MATRIX>(matrix) :
			cofactor<MATRIX>(matrix));
}

/**
	* Determinant of a matrix
	*/
template<typename MATRIX, typename = std::enable_if_t<MATRIX::NUM_ROWS == MATRIX::NUM_COLS, int>>
inline constexpr typename MATRIX::value_type  det(const MATRIX& matrix)
{
	typedef typename MATRIX::value_type T;
	constexpr unsigned int N = MATRIX::NUM_ROWS;
	Matrix<T, N> in{};
	for (size_t i = 0; i < N; i++) {
		for (size_t j = 0; j < N; j++) {
			in[i][j] = matrix[i][j];
		}
	}
	return Determinant<typename MATRIX::value_type, MATRIX::NUM_COLS>::determinant(in);
}

/**
	* Inversion function which switches on the matrix size.
	* @warning This function assumes the matrix is invertible. The result is
	* undefined if it is not. It is the responsibility of the caller to
	* make sure the matrix is not singular.
	*/
template<typename MATRIX, typename = std::enable_if_t<MATRIX::NUM_ROWS == MATRIX::NUM_COLS, int>>
	inline constexpr MATRIX inverse(const MATRIX& matrix) {
	return (MATRIX::NUM_ROWS == 2) ? fastInverse2<MATRIX>(matrix) :
		((MATRIX::NUM_ROWS == 3) ? fastInverse3<MATRIX>(matrix) :
			gaussJordanInverse<MATRIX>(matrix));
}

template<typename MATRIX, typename = std::enable_if_t<MATRIX::NUM_ROWS == MATRIX::NUM_COLS, int>>
inline constexpr MATRIX transpose(MATRIX m)
{
	// for now we only handle square matrix transpose
	MATRIX result{};
	for (size_t col = 0; col < MATRIX::NUM_COLS; ++col) {
		for (size_t row = 0; row < MATRIX::NUM_ROWS; ++row) {
			result[col][row] = m[row][col];
		}
	}
	return result;
}


template<typename MATRIX_R, typename MATRIX_A, typename MATRIX_B,
	typename = std::enable_if_t<
	MATRIX_A::NUM_COLS == MATRIX_B::NUM_ROWS &&
	MATRIX_R::NUM_COLS == MATRIX_B::NUM_COLS &&
	MATRIX_R::NUM_ROWS == MATRIX_A::NUM_ROWS, int>>
constexpr MATRIX_R multiply(MATRIX_A lhs, MATRIX_B rhs)
{
	// pre-requisite:
	//  lhs : D columns, R rows
	//  rhs : C columns, D rows
	//  res : C columns, R rows
	MATRIX_R res{};
	for (size_t col = 0; col < MATRIX_R::NUM_COLS; ++col) {
		res[col] = lhs * rhs[col];
	}
	return res;
}
}

template<template<typename> class BASE, typename T,
	template<typename> class VEC>
class MatProductOperators
{
public:
	//  matrix *= matrix
	template<typename U>
	constexpr BASE<T>& operator*=(const BASE<U>& rhs)
	{
		BASE<T>& lhs(static_cast<BASE<T>&>(*this));
		lhs = matrix::multiply<BASE<T>>(lhs, rhs);
		return lhs;
	}

	// matrix *= scalar
	template<typename U, typename = enable_if_arithmetic_t<U>>
	constexpr BASE<T>& operator*=(U v)
	{
		BASE<T>& lhs(static_cast<BASE<T>&>(*this));
		for (size_t col = 0; col < BASE<T>::NUM_COLS; ++col)
		{
			lhs[col] *= v;
		}
		return lhs;
	}

	// matrix /= scalar
	template<typename U, typename = enable_if_arithmetic_t<U>>
	constexpr BASE<T>& operator/=(U v)
	{
		BASE<T>& lhs(static_cast<BASE<T>&>(*this));
		for (size_t col = 0; col < BASE<T>::NUM_COLS; ++col)
		{
			lhs[col] /= v;
		}
		return lhs;
	}

private:

	template<typename U>
	friend inline constexpr BASE<arithmetic_result_t<T, U>>
		operator*(BASE<T> lhs, BASE<U> rhs) {
		return matrix::multiply<BASE<arithmetic_result_t<T, U>>>(lhs, rhs);
	}

	// matrix * vector
	template<typename U>
	friend inline constexpr typename BASE<arithmetic_result_t<T, U>>::col_type
		operator*(const BASE<T>& lhs, const VEC<U>& rhs) {
		typename BASE<arithmetic_result_t<T, U>>::col_type result{};
		for (size_t col = 0; col < BASE<T>::NUM_COLS; ++col) {
			result += lhs[col] * rhs[col];
		}
		return result;
	}

	// row-vector * matrix
	template<typename U>
	friend inline constexpr typename BASE<arithmetic_result_t<T, U>>::row_type
		operator*(const VEC<U>& lhs, const BASE<T>& rhs)
	{
		typename BASE<arithmetic_result_t<T, U>>::row_type result{};
		for (size_t col = 0; col < BASE<T>::NUM_COLS; ++col) {
			result[col] = dot(lhs, rhs[col]);
		}
		return result;
	}

	// matrix * scalar
	template<typename U, typename = enable_if_arithmetic_t <U>>
	friend inline constexpr BASE<arithmetic_result_t < T, U>>
		operator*(const BASE<T>& lhs, U rhs)
	{
		BASE<arithmetic_result_t<T, U>> result{};
		for (size_t col = 0; col < BASE<T>::NUM_COLS; ++col)
		{
			result[col] = lhs[col] * rhs;
		}
		return result;
	}

	// scalar * matrix
	template<typename U, typename = enable_if_arithmetic_t<U>>
	friend inline constexpr BASE<arithmetic_result_t<T, U>> operator*(U rhs, const BASE<T>& lhs)
	{
		return lhs * rhs;
	}

	// matrix / scalar
	template<typename U, typename = enable_if_arithmetic_t<U>>
	friend inline constexpr BASE<arithmetic_result_t<T, U>> operator/(const BASE<T>& lhs, U rhs)
	{
		BASE<arithmetic_result_t<T, U>> result{};
		for (size_t col = 0; col < BASE<T>::NUM_COLS; ++col)
		{
			result[col] = lhs[col] / rhs;
		}
		return result;
	}
};

template<template<typename U> class BASE, typename T>
class MatSquareFunctions {
private:
	/*
	 * NOTE: the functions below ARE NOT member methods. They are friend functions
	 * with they definition inlined with their declaration. This makes these
	 * template functions available to the compiler when (and only when) this class
	 * is instantiated, at which point they're only templated on the 2nd parameter
	 * (the first one, BASE<T> being known).
	 */
	friend inline constexpr BASE<T> inverse(const BASE<T>& matrix)
	{
		return matrix::inverse(matrix);
	}

	friend inline constexpr BASE<T> cof(const BASE<T>& matrix)
	{
		return matrix::cof(matrix);
	}

	friend inline constexpr BASE<T> transpose(BASE<T> m)
	{
		return matrix::transpose(m);
	}

	friend inline constexpr T trace(BASE<T> m)
	{
		return matrix::trace(m);
	}

	friend inline constexpr T det(const BASE<T>& m)
	{
		return matrix::det(m);
	}

	// unclear why we have to use 'auto' here. 'typename BASE<T>::col_type' produces
	// error: no type named 'col_type' in 'filament::math::details::TMat44<float>'
	friend inline constexpr auto diag(const BASE<T>& m)
	{
		return matrix::diag(m);
	}
};

template<template<typename U> class BASE, typename T>
class MatHelpers {
public:
	constexpr inline size_t getColumnSize() const { return BASE<T>::COL_SIZE; }
	constexpr inline size_t getRowSize() const { return BASE<T>::ROW_SIZE; }
	constexpr inline size_t getColumnCount() const { return BASE<T>::NUM_COLS; }
	constexpr inline size_t getRowCount() const { return BASE<T>::NUM_ROWS; }
	constexpr inline size_t size()  const { return BASE<T>::ROW_SIZE; }  // for TVec*<>

	// array access
	constexpr T const* asArray() const
	{
		return &static_cast<BASE<T> const &>(*this)[0][0];
	}

	// element access
	inline constexpr T const& operator()(size_t row, size_t col) const
	{
		return static_cast<BASE<T> const &>(*this)[col][row];
	}

	inline T& operator()(size_t row, size_t col)
	{
		return static_cast<BASE<T>&>(*this)[col][row];
	}

private:
	constexpr friend inline BASE<T> abs(BASE<T> m)
	{
		for (size_t col = 0; col < BASE<T>::NUM_COLS; ++col) {
			m[col] = abs(m[col]);
		}
		return m;
	}
};


}
}