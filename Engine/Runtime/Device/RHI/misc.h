#include "common.h"
#include <type_traits> 
namespace redtea
{
namespace device
{
    template <typename T, typename U>
    T CHECKED_CAST(U u)
    {
        static_assert(!std::is_same<T, U>::value, "Redundant CHECKED_CAST");
#ifdef _DEBUG
        if (!u) return nullptr;
        T t = dynamic_cast<T>(u);
        if (!t) ASSERT(!"Invalid type cast");  // NOLINT(clang-diagnostic-string-conversion)
        return t;
#else
        return static_cast<T>(u);
#endif
    }

    template<typename T, typename U>  bool ArraysAreDifferent(const T& a, const U& b)
    {
        if (a.size() != b.size())
            return true;

        for (uint32_t i = 0; i < uint32_t(a.size()); i++)
        {
            if (a[i] != b[i])
                return true;
        }

        return false;
    }

    template<typename T, typename U> uint32_t ArrayDifferenceMask(const T& a, const U& b)
    {
        ASSERT(a.size() <= 32);
        ASSERT(b.size() <= 32);

        if (a.size() != b.size())
            return ~0u;

        uint32_t mask = 0;
        for (uint32_t i = 0; i < uint32_t(a.size()); i++)
        {
            if (a[i] != b[i])
                mask |= (1 << i);
        }

        return mask;
    }

    template<typename T> T Align(T size, T alignment)
    {
        return (size + alignment - 1) & ~(alignment - 1);
    }
}
}