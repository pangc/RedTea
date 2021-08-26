#pragma once
#include "../common.h"

namespace redtea {
namespace math {

template<typename T, typename U>
struct arithmetic_result {
	using type = decltype(std::declval<T>() + std::declval<U>());
};

template<typename T, typename U>
using arithmetic_result_t = typename arithmetic_result<T, U>::type;

template<typename A, typename B = int, typename C = int, typename D = int>
using enable_if_arithmetic_t = std::enable_if_t<
	std::is_arithmetic<A>::value &&
	std::is_arithmetic<B>::value &&
	std::is_arithmetic<C>::value &&
	std::is_arithmetic<D>::value>;


template<template<typename T> class VECTOR, typename T>
class VecAddOperators
{
public:
	template<typename U>
	constexpr VECTOR<T>& operator+=(const VECTOR<U>& v)
	{
		VECTOR<T>& lhs = static_cast<VECTOR<T>&>(*this);
		for (size_t i = 0; i < lhs.size(); i++) {
			lhs[i] += v[i];
		}
		return lhs;
	}

	template<typename U, typename = enable_if_arithmetic_t<U>>
	constexpr VECTOR<T>& operator+=(U v) {
		return operator+=(VECTOR<U>(v));
	}

	template<typename U>
	constexpr VECTOR<T>& operator-=(const VECTOR<U>& v)
	{
		VECTOR<T>& lhs = static_cast<VECTOR<T>&>(*this);
		for (size_t i = 0; i < lhs.size(); i++) {
			lhs[i] -= v[i];
		}
		return lhs;
	}

	template<typename U, typename = enable_if_arithmetic_t<U>>
	constexpr VECTOR<T>& operator-=(U v) {
		return operator-=(VECTOR<U>(v));
	}
public:
	template<typename U>
	friend inline constexpr
	VECTOR<arithmetic_result_t<T, U>> operator+(const VECTOR<T>& lv, const VECTOR<U>& rv)
	{
		VECTOR<arithmetic_result_t<T, U>> res(lv);
		res += rv;
		return res;
	}

	template<typename U, typename = enable_if_arithmetic_t<U>>
	friend inline constexpr
	VECTOR<arithmetic_result_t<T, U>> operator+(const VECTOR<T>& lv, U rv)
	{
		return lv + VECTOR<U>(rv);
	}

	template<typename U, typename = enable_if_arithmetic_t<U>>
	friend inline constexpr
	VECTOR<arithmetic_result_t<T, U>> operator+(U lv, const VECTOR<T>& rv)
	{
		return VECTOR<U>(lv) + rv;
	}

	template<typename U>
	friend inline constexpr
	VECTOR<arithmetic_result_t<T, U>> operator-(const VECTOR<T>& lv, const VECTOR<U>& rv)
	{
		VECTOR<arithmetic_result_t<T, U>> res(lv);
		res -= rv;
		return res;
	}

	template<typename U, typename = enable_if_arithmetic_t<U>>
	friend inline constexpr
	VECTOR<arithmetic_result_t<T, U>> operator-(const VECTOR<T>& lv, U rv)
	{
		return lv - VECTOR<U>(rv);
	}

	template<typename U, typename = enable_if_arithmetic_t<U>>
	friend inline constexpr
		VECTOR<arithmetic_result_t<T, U>> operator-(U lv, const VECTOR<T>& rv)
	{
		return VECTOR<U>(lv) - rv;
	}
};

template<template<typename T> class VECTOR, typename T>
class VecComparisonOperators
{
public:
	template<typename U>
	friend inline constexpr
	bool operator==(const VECTOR<T>& lv, const VECTOR<U>& rv)
	{
		for (size_t i = 0; i < lv.size(); i++)
		{
			if (lv[i] != rv[i]) {
				return false;
			}
		}
		return true;
	}

	template<typename U>
	friend inline constexpr
	bool operator!=(const VECTOR<T>& lv, const VECTOR<U>& rv)
	{
		return !operator==(lv, rv);
	}
};

template<template<typename T> class VECTOR, typename T>
class VecFunctions
{
private:
	template<typename U>
	friend constexpr inline
	arithmetic_result_t<T, U> dot(const VECTOR<T>& lv, const VECTOR<U>& rv)
	{
		arithmetic_result_t<T, U> r{};
		for (size_t i = 0; i < lv.size(); i++)
		{
			r += lv[i] * rv[i];
		}
		return r;
	}

	friend inline T norm(const VECTOR<T>& lv)
	{
		return std::sqrt(dot(lv, lv));
	}

	friend inline T length(const VECTOR<T>& lv)
	{
		return norm(lv);
	}

	template<typename U>
	friend inline constexpr
	arithmetic_result_t<T, U> distance(const VECTOR<T>& lv, const VECTOR<U>& rv)
	{
		return length(rv - lv);
	}

	friend inline VECTOR<T> normalize(const VECTOR<T>& lv)
	{
		T l = length(lv);
		ASSERT(l != 0);
		return lv * (T(1) / l);
	}
};

}
}