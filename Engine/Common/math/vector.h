#pragma once
#include "../common.h"
#include "vector_helper.h"

namespace redtea {
namespace math {

template <typename T>
class Vector2 : public VecAddOperators<Vector2, T>, VecComparisonOperators<Vector2, T>, VecFunctions<Vector2, T>
{
	static constexpr size_t SIZE = 2;
public:
	union
	{
		T data[SIZE];
		struct { T x, y; };
		struct { T r, g; };
		struct { T u, v; };
	};

	inline constexpr size_t size() const { return SIZE; }

	Vector2() = default;
	Vector2(std::initializer_list<const T> list)
	{
		size_t i = 0;
		assert(list.size() <= SIZE);
		for (auto val : list)
		{
			data[i++] = val;
		}
	}
	// array access
	inline constexpr T const& operator[](size_t i) const noexcept
	{
		assert(i < SIZE);
		return data[i];
	}

	inline constexpr T& operator[](size_t i) noexcept
	{
		assert(i < SIZE);
		return data[i];
	}

	template<typename A, typename = enable_if_arithmetic_t<A>>
	constexpr Vector2(A v) noexcept : data{ T(v), T(v) } {}

	template<typename A, typename B, typename = enable_if_arithmetic_t<A, B>>
	constexpr Vector2(A x, B y) noexcept : data{ T(x), T(y) } {}

	template<typename A, typename = enable_if_arithmetic_t<A>>
	constexpr Vector2(const Vector2<A>& v) noexcept : data{ T(v[0]), T(v[1]) } {}

	// cross product works only on vectors of size 2 or 3
	template<typename U>
	friend inline constexpr
	arithmetic_result_t<T, U> cross(const Vector2& u, const Vector2<U>& v) noexcept
	{
		return u[0] * v[1] - u[1] * v[0];
	}
};

template <typename T>
class Vector3 : public VecAddOperators<Vector3, T>, VecComparisonOperators<Vector3, T>, VecFunctions<Vector3, T>
{
public:
	static constexpr size_t SIZE = 3;

	union
	{
		T data[SIZE];
		Vector2<T> xy;
		Vector2<T> rg;
		struct
		{
			union
			{
				T x;
				T r;
			};
			union
			{
				struct { T y, z; };
				struct { T g, b; };
				Vector2<T> yz;
				Vector2<T> gb;
			};
		};
	};

	inline constexpr size_t size() const { return SIZE; }

	Vector3() = default;
	Vector3(std::initializer_list<const T> list)
	{
		size_t i = 0;
		assert(list.size() <= SIZE);
		for (auto val : list)
		{
			data[i++] = val;
		}
	}

	// array access
	inline constexpr T const& operator[](size_t i) const noexcept
	{
		assert(i < SIZE);
		return data[i];
	}

	inline constexpr T& operator[](size_t i) noexcept
	{
		assert(i < SIZE);
		return data[i];
	}

	template<typename A, typename = enable_if_arithmetic_t<A>>
	constexpr Vector3(A v) noexcept : data{ T(v), T(v), T(v) } {}

	template<typename A, typename B, typename C,
		typename = enable_if_arithmetic_t<A, B, C>>
		constexpr Vector3(A x, B y, C z) noexcept : data{ T(x), T(y), T(z) } {}

	template<typename A, typename B, typename = enable_if_arithmetic_t<A, B>>
	constexpr Vector3(const Vector2<A>& v, B z) noexcept : data{ T(v[0]), T(v[1]), T(z) } {}

	template<typename A, typename = enable_if_arithmetic_t<A>>
	constexpr Vector3(const Vector3<A>& v) noexcept : data{ T(v[0]), T(v[1]), T(v[2]) } {}

	// cross product works only on vectors of size 3
	template<typename U>
	friend inline constexpr
	Vector3<arithmetic_result_t<T, U>> cross(const Vector3& u, const Vector3<U>& v) noexcept
	{
		return
		{
			u[1] * v[2] - u[2] * v[1],
			u[2] * v[0] - u[0] * v[2],
			u[0] * v[1] - u[1] * v[0]
		};
	}
};

template <typename T>
class Vector4 : public VecAddOperators<Vector4, T>, VecComparisonOperators<Vector4, T>, VecFunctions<Vector4, T>
{
public:
	static constexpr size_t SIZE = 4;

	union
	{
		T data[SIZE];
		Vector2<T> xy, rg;
		Vector3<T> xyz, rgb;
		struct
		{
			union { T x, r; };
			union
			{
				Vector2<T> yz, gb;
				Vector3<T> yzw, gba;
				struct
				{
					union { T y, g; };
					union {
						Vector2<T> zw, ba;
						struct { T z, w; };
						struct { T b, a; };
					};
				};
			};
		};
	};

	inline constexpr size_t size() const { return SIZE; }

	Vector4() = default;
	Vector4(std::initializer_list<const T> list)
	{
		size_t i = 0;
		assert(list.size() <= SIZE);
		for (auto val : list)
		{
			data[i++] = val;
		}
	}

	// array access
	inline constexpr T const& operator[](size_t i) const noexcept
	{
		assert(i < SIZE);
		return data[i];
	}

	inline constexpr T& operator[](size_t i) noexcept
	{
		assert(i < SIZE);
		return data[i];
	}

	template<typename A, typename = enable_if_arithmetic_t<A>>
	constexpr Vector4(A v) noexcept : data{ T(v), T(v), T(v), T(v) } {}

	template<typename A, typename B, typename C, typename D,
		typename = enable_if_arithmetic_t<A, B, C, D>>
		constexpr Vector4(A x, B y, C z, D w) noexcept : data{ T(x), T(y), T(z), T(w) } {}

	template<typename A, typename B, typename C,
		typename = enable_if_arithmetic_t<A, B, C>>
		constexpr Vector4(const Vector2<A>& v, B z, C w) noexcept : data{ T(v[0]), T(v[1]), T(z), T(w) } {}

	template<typename A, typename B, typename = enable_if_arithmetic_t<A, B>>
	constexpr Vector4(const Vector3<A>& v, B w) noexcept : data{ T(v[0]), T(v[1]), T(v[2]), T(w) } {}

	template<typename A, typename = enable_if_arithmetic_t<A>>
	constexpr Vector4(const Vector4<A>& v) noexcept : data{ T(v[0]), T(v[1]), T(v[2]), T(v[3]) } {}

};

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;

using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;

using Vector4f = Vector4<float>;
using Vector4i = Vector4<int>;

}
}