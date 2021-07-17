#include "memory.h"
#include <stdlib.h>

namespace redtea
{
namespace common
{

	inline void* aligned_alloc(size_t size, size_t align) noexcept {
		assert(align && !(align & align - 1));

		void* p = nullptr;

		// must be a power of two and >= sizeof(void*)
		while (align < sizeof(void*)) {
			align <<= 1u;
		}

#if defined(WIN32)
		p = ::_aligned_malloc(size, align);
#else
		::posix_memalign(&p, align, size);
#endif
		return p;
	}

	inline void aligned_free(void* p) noexcept {
#if defined(WIN32)
		::_aligned_free(p);
#else
		::free(p);
#endif
	}

	void * AllocatorBase::alloc(size_t size, size_t alignment, size_t extra)
	{
		assert(extra == 0);
		return aligned_alloc(size, alignment);
	}

	void AllocatorBase::free(void * p) noexcept
	{
		aligned_free(p);
	}

	void AllocatorBase::free(void * p, size_t) noexcept
	{
		free(p);
	}
}
}