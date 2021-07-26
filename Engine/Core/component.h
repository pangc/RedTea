#pragma once
#include <cstdint>

namespace redtea {
namespace core {

	//template <typename T>
	class ComponentInstance {
	public:
		using Type = uint32_t;

	public:
		constexpr ComponentInstance() noexcept = default;

		// check if this Instance is valid
		constexpr bool IsValid() const noexcept { return mInstance != 0; }

		// Instances of same type can be copied/assigned
		constexpr ComponentInstance(ComponentInstance const& other) noexcept = default;
		constexpr ComponentInstance& operator=(ComponentInstance const& other) noexcept = default;

		// Instances can be compared
		constexpr bool operator!=(ComponentInstance e) const { return mInstance != e.mInstance; }
		constexpr bool operator==(ComponentInstance e) const { return mInstance == e.mInstance; }

		// Instances can be sorted
		constexpr bool operator<(ComponentInstance e) const { return mInstance < e.mInstance; }
		constexpr bool operator<=(ComponentInstance e) const { return mInstance <= e.mInstance; }
		constexpr bool operator>(ComponentInstance e) const { return mInstance > e.mInstance; }
		constexpr bool operator>=(ComponentInstance e) const { return mInstance >= e.mInstance; }

		// and we can iterate
		constexpr ComponentInstance& operator++() noexcept { ++mInstance; return *this; }
		constexpr ComponentInstance& operator--() noexcept { --mInstance; return *this; }
		constexpr const ComponentInstance operator++(int) const noexcept { return ComponentInstance{ mInstance + 1 }; }
		constexpr const ComponentInstance operator--(int) const noexcept { return ComponentInstance{ mInstance - 1 }; }


		// return a value for this Instance (mostly needed for debugging
		constexpr uint32_t AsValue() const noexcept { return mInstance; }

		// auto convert to Type so it can be used as an index
		constexpr operator Type() const noexcept { return mInstance; } // NOLINT(google-explicit-constructor)

		// conversion from Type so we can initialize from an index
		constexpr ComponentInstance(Type value) noexcept { mInstance = value; } // NOLINT(google-explicit-constructor)

	protected:
		Type mInstance = 0;
	};
}
}