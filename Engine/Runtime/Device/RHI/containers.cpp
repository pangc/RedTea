#include "containers.h"

namespace redtea
{
namespace device
{

    BitSetAllocator::BitSetAllocator(const size_t capacity, bool multithreaded)
        : m_MultiThreaded(multithreaded)
    {
        m_Allocated.resize(capacity);
    }

    int BitSetAllocator::allocate()
    {
        if (m_MultiThreaded)
            m_Mutex.lock();

        int result = -1;

        int capacity = static_cast<int>(m_Allocated.size());
        for (int i = 0; i < capacity; i++)
        {
            int ii = (m_NextAvailable + i) % capacity;

            if (!m_Allocated[ii])
            {
                result = ii;
                m_NextAvailable = (ii + 1) % capacity;
                m_Allocated[ii] = true;
                break;
            }
        }

        if (m_MultiThreaded)
            m_Mutex.unlock();

        return result;
    }

    void BitSetAllocator::release(const int index)
    {
        if (index >= 0 && index < static_cast<int>(m_Allocated.size()))
        {
            if (m_MultiThreaded)
                m_Mutex.lock();

            m_Allocated[index] = false;
            m_NextAvailable = std::min(m_NextAvailable, index);

            if (m_MultiThreaded)
                m_Mutex.unlock();
        }
    }

}
}