#pragma once
#include "vector.h"
#include "quaternion_helper.h"

namespace redtea {
namespace math {

template<typename T>
class Quaternion : public VecAddOperators<Quaternion, T>,
		public VecComparisonOperators<Quaternion, T>,
		public QuatProductOperators<Quaternion, T>
{
	public:
	union {
		struct { T x, y, z, w; };
		Vector4<T> xyzw;
		Vector3<T> xyz;
		Vector2<T> xy;
	};

	enum { SIZE = 4 };
	inline constexpr static size_t size() { return SIZE; }

	inline constexpr T const& operator[](size_t i) const noexcept
	{
		// only possible in C++0x14 with constexpr
		ASSERT(i < SIZE);
		return (&x)[i];
	}

	inline constexpr T& operator[](size_t i)
	{
		ASSERT(i < SIZE);
		return (&x)[i];
	}

	// -----------------------------------------------------------------------
	// we want the compiler generated versions for these...
	Quaternion(const Quaternion&) = default;
	~Quaternion() = default;
	Quaternion& operator=(const Quaternion&) = default;

	// constructors

	// default constructor. sets all values to zero.
	constexpr Quaternion() : x(0), y(0), z(0), w(0) {}

	// handles implicit conversion to a quat. must not be explicit.
	template<typename A, typename = enable_if_arithmetic_t<A>>
	constexpr Quaternion(A w) : x(0), y(0), z(0), w(w) {}

	// initialize from 4 values to w + xi + yj + zk
	template<typename A, typename B, typename C, typename D,
		typename = enable_if_arithmetic_t<A, B, C, D>>
		constexpr Quaternion(A w, B x, C y, D z) : x(x), y(y), z(z), w(w) {}

	// initialize from a vec3 + a value to : v.xi + v.yj + v.zk + w
	template<typename A, typename B, typename = enable_if_arithmetic_t<A, B>>
	constexpr Quaternion(const Vector3<A>& v, B w) : x(v.x), y(v.y), z(v.z), w(w) {}

	// initialize from a vec4
	template<typename A, typename = enable_if_arithmetic_t<A>>
	constexpr explicit Quaternion(const Vector4<A>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

	// initialize from a quaternion of a different type
	template<typename A, typename = enable_if_arithmetic_t<A>>
	constexpr explicit Quaternion(const Quaternion<A>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

	// conjugate operator
	constexpr Quaternion operator~() const {
		return conj(*this);
	}

	// constructs a quaternion from an axis and angle
	template<typename A, typename B, typename = enable_if_arithmetic_t<A, B>>
	constexpr static Quaternion fromAxisAngle(const Vector3<A>& axis, B angle)
	{
		return Quaternion(std::sin(angle * 0.5) * normalize(axis), std::cos(angle * 0.5));
	}
};

}
}