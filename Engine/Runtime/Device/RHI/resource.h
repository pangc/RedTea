#pragma once
#include <atomic>
#include "compiler_option.h"
#include "common.h"

namespace redtea {
namespace device {

typedef uint32_t ObjectType;

    // ObjectTypes namespace contains identifiers for various object types. 
    // All constants have to be distinct. Implementations of NVRHI may extend the list.
    //
    // The encoding is chosen to minimize potential conflicts between implementations.
    // 0x00aabbcc, where:
    //   aa is GAPI, 1 for D3D11, 2 for D3D12, 3 for VK
    //   bb is layer, 0 for native GAPI objects, 1 for reference NVRHI backend, 2 for user-defined backends
    //   cc is a sequential number

    namespace ObjectTypes
    {
        constexpr ObjectType D3D11_Device                           = 0x00010001;
        constexpr ObjectType D3D11_DeviceContext                    = 0x00010002;
        constexpr ObjectType D3D11_Resource                         = 0x00010003;
        constexpr ObjectType D3D11_Buffer                           = 0x00010004;
        constexpr ObjectType D3D11_RenderTargetView                 = 0x00010005;
        constexpr ObjectType D3D11_DepthStencilView                 = 0x00010006;
        constexpr ObjectType D3D11_ShaderResourceView               = 0x00010007;
        constexpr ObjectType D3D11_UnorderedAccessView              = 0x00010008;

        constexpr ObjectType D3D12_Device                           = 0x00020001;
        constexpr ObjectType D3D12_CommandQueue                     = 0x00020002;
        constexpr ObjectType D3D12_GraphicsCommandList              = 0x00020003;
        constexpr ObjectType D3D12_Resource                         = 0x00020004;
        constexpr ObjectType D3D12_RenderTargetViewDescriptor       = 0x00020005;
        constexpr ObjectType D3D12_DepthStencilViewDescriptor       = 0x00020006;
        constexpr ObjectType D3D12_ShaderResourceViewGpuDescripror  = 0x00020007;
        constexpr ObjectType D3D12_UnorderedAccessViewGpuDescripror = 0x00020008;
        constexpr ObjectType D3D12_RootSignature                    = 0x00020009;
        constexpr ObjectType D3D12_PipelineState                    = 0x0002000a;

        constexpr ObjectType VK_Device                              = 0x00030001;
        constexpr ObjectType VK_PhysicalDevice                      = 0x00030002;
        constexpr ObjectType VK_Instance                            = 0x00030003;
        constexpr ObjectType VK_Queue                               = 0x00030004;
        constexpr ObjectType VK_CommandBuffer                       = 0x00030005;
        constexpr ObjectType VK_DeviceMemory                        = 0x00030006;
        constexpr ObjectType VK_Buffer                              = 0x00030007;
        constexpr ObjectType VK_Image                               = 0x00030008;
        constexpr ObjectType VK_ImageView                           = 0x00030009;
        constexpr ObjectType VK_AccelerationStructureKHR            = 0x0003000a;
        constexpr ObjectType VK_Sampler                             = 0x0003000b;
        constexpr ObjectType VK_ShaderModule                        = 0x0003000c;
        constexpr ObjectType VK_RenderPass                          = 0x0003000d;
        constexpr ObjectType VK_Framebuffer                         = 0x0003000e;
        constexpr ObjectType VK_DescriptorPool                      = 0x0003000f;
        constexpr ObjectType VK_DescriptorSetLayout                 = 0x00030010;
        constexpr ObjectType VK_DescriptorSet                       = 0x00030011;
        constexpr ObjectType VK_PipelineLayout                      = 0x00030012;
        constexpr ObjectType VK_Pipeline                            = 0x00030013;
    };

    struct Object
    {
        union {
            uint64_t integer;
            void* pointer;
        };

        Object(uint64_t i) : integer(i) { }  // NOLINT(cppcoreguidelines-pro-type-member-init)
        Object(void* p) : pointer(p) { }     // NOLINT(cppcoreguidelines-pro-type-member-init)

        template<typename T> operator T* () const { return static_cast<T*>(pointer); }
    };
    
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
    virtual Object getNativeObject(ObjectType objectType) { (void)objectType; return nullptr; }
    
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
        if (mRefPtr != other)
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
        if (mRefPtr != other.mRefPtr)
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