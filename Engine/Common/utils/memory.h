#pragma once
#include "../common.h"
#include <cstddef>

namespace redtea
{
namespace common
{
	class AllocatorBase {
	public:
		AllocatorBase() noexcept = default;

		void* alloc(size_t size, size_t alignment = alignof(std::max_align_t), size_t extra = 0);

		void free(void* p) noexcept;
		void free(void* p, size_t) noexcept;
		~AllocatorBase() noexcept = default;
	};

}
}