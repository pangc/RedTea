#include "../common.h"
#include "vector_helper.h"

namespace redtea {
namespace math {

template <typename T>
class Vector2 : public VecAddOperators<Vector2, T>
{
	static constexpr size_t SIZE = 2;
public:
	union {
		T data[2];
		struct { T x, y; };
		struct { T s, t; };
		struct { T r, g; };
		struct { T u, v; };
	};

	inline constexpr size_t size() const { return SIZE; }

	Vector2() = default;
	Vector2(std::initializer_list<const T> list) {
		size_t i = 0;
		assert(list.size() <= SIZE);
		for (auto val : list) {
			data[i++] = val;
		}
	}
	// array access
	inline constexpr T const& operator[](size_t i) const noexcept {
		assert(i < SIZE);
		return data[i];
	}

	inline constexpr T& operator[](size_t i) noexcept {
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
		arithmetic_result_t<T, U> cross(const Vector2& u, const Vector2<U>& v) noexcept {
		return u[0] * v[1] - u[1] * v[0];
	}
};

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;

}
}