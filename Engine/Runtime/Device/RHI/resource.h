#pragma once
#include <atomic>
#include "compiler_option.h"
#include "common.h"

namespace redtea {
namespace device {

// Resource Interface
class IResource
{
protected:
    IResource() = default;
    virtual ~IResource() = default;

public:
    virtual unsigned long AddRef() = 0;
    virtual unsigned long Release() = 0;
	virtual unsigned long GetCount() = 0;
	virtual void Destroy() { delete this; }

    // Returns a native object or interface, for example ID3D11Device*, or nullptr if the requested interface is unavailable.
    // Does *not* AddRef the returned interface.
    virtual void* GetNativeObject() { return nullptr; }
    
    // Non-copyable and non-movable
    IResource(const IResource&) = delete;
    IResource(const IResource&&) = delete;
    IResource& operator=(const IResource&) = delete;
    IResource& operator=(const IResource&&) = delete;
};

// RefCounter
template<class T>
class RefCounter : public T
{
private:
    mutable std::atomic<unsigned long> mRefCount = 1;
public:
    virtual unsigned long AddRef() override 
    {
        return mRefCount.fetch_add(1, std::memory_order_relaxed);;
    }

    virtual unsigned long Release() override
    {
        unsigned long result = mRefCount.fetch_sub(1, std::memory_order_acq_rel);
        if (result == 1)
        {
            this->Destroy();
        }
        return result;
    }

	virtual unsigned long GetCount() override
	{
		return mRefCount.load();;
	}
};

// Resource Handler
template <typename T>
class RefCountPtr
{
public:
    typedef T InterfaceType;
public:
    RefCountPtr()
    : mRefPtr(nullptr) { }

    ~RefCountPtr() noexcept
    {
        InternalRelease();
    }

    RefCountPtr(InterfaceType* InReference)
    : mRefPtr(InReference)
    {
        InternalAddRef();
    }

    RefCountPtr(const RefCountPtr& other) noexcept : mRefPtr(other.mRefPtr)
    {
        InternalAddRef();
    }

    RefCountPtr(RefCountPtr &&other) noexcept : mRefPtr(nullptr)
    {
        if (this != reinterpret_cast<RefCountPtr*>(&reinterpret_cast<unsigned char&>(other)))
        {
            Swap(other);
        }
    }
	template<class U>
	RefCountPtr(U* other) noexcept : mRefPtr(other)
	{
		InternalAddRef();
	}

    // convertable
	template<class U>
	RefCountPtr(const RefCountPtr<U> &other, typename std::enable_if<std::is_convertible<U*, T*>::value, void *>::type * = nullptr) noexcept :
		mRefPtr(other.mRefPtr)
	{
		InternalAddRef();
	}

    // Move ctor that allows instantiation of a class when U* is convertible to T*
    template<class U>
    RefCountPtr(RefCountPtr<U>&& other, typename std::enable_if<std::is_convertible<U*, T*>::value, void *>::type * = nullptr) noexcept :
        mRefPtr(other.mRefPtr)
    {
        other.mRefPtr = nullptr;
    }

    RefCountPtr& operator=(std::nullptr_t) noexcept
    {
        InternalRelease();
        return *this;
    }

    RefCountPtr& operator=(T *other) noexcept
    {
        if (ptr_ != other)
        {
            RefCountPtr(other).Swap(*this);
        }
        return *this;
    }

    template <typename U>
    RefCountPtr& operator=(U *other) noexcept
    {
        RefCountPtr(other).Swap(*this);
        return *this;
    }

    RefCountPtr& operator=(const RefCountPtr &other) noexcept
    {
        if (ptr_ != other.mRefPtr)
        {
            RefCountPtr(other).Swap(*this);
        }
        return *this;
    }

    template<class U>
    RefCountPtr& operator=(const RefCountPtr<U>& other) noexcept
    {
        RefCountPtr(other).Swap(*this);
        return *this;
    }

    RefCountPtr& operator=(RefCountPtr &&other) noexcept
    {
        RefCountPtr(static_cast<RefCountPtr&&>(other)).Swap(*this);
        return *this;
    }

    template<class U>
    RefCountPtr& operator=(RefCountPtr<U>&& other) noexcept
    {
        RefCountPtr(static_cast<RefCountPtr<U>&&>(other)).Swap(*this);
        return *this;
    }

    void Swap(RefCountPtr&& r) noexcept
    {
        InterfaceType* tmp = mRefPtr;
        mRefPtr = r.mRefPtr;
        r.mRefPtr = tmp;
    }

    void Swap(RefCountPtr& r) noexcept
    {
        InterfaceType* tmp = mRefPtr;
        mRefPtr = r.mRefPtr;
        r.mRefPtr = tmp;
    }

    NODISCARD T* Get() const noexcept
    {
        return mRefPtr;
    }
    
    operator T*() const
    {
        return mRefPtr;
    }

    InterfaceType* operator->() const noexcept
    {
        return mRefPtr;
    }

    T** operator&()   // NOLINT(google-runtime-operator)
    {
        return &mRefPtr;
    }

	T* Detach() noexcept
	{
		T* ptr = mRefPtr;
		mRefPtr = nullptr;
		return ptr;
	}

	void Attach(InterfaceType* other)
	{
		if (mRefPtr != nullptr)
		{
			auto ref = mRefPtr->Release();
			(void)ref;

			ASSERT(ref != 0 || mRefPtr != other);
		}

		mRefPtr = other;
	}

	// Create a wrapper around a raw object while keeping the object's reference count unchanged
	static RefCountPtr<T> Create(T* other)
	{
		RefCountPtr<T> Ptr;
		Ptr.Attach(other);
		return Ptr;
	}

protected:
    void InternalAddRef() const noexcept
    {
        if (mRefPtr != nullptr)
        {
            mRefPtr->AddRef();
        }
    }

    unsigned long InternalRelease() noexcept
    {
        unsigned long ref = 0;
        InterfaceType* temp = mRefPtr;

        if (temp != nullptr)
        {
            mRefPtr = nullptr;
            ref = temp->Release();
        }

        return ref;
    }

    InterfaceType* mRefPtr;
};

}
}