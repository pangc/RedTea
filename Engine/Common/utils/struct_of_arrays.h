#pragma once
#include <array>
#include <functional>
#include <utility>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "common.h"
#include "memory.h"
#include "compiler_option.h"
namespace redtea
{
namespace common
{

template <typename Allocator, typename ... Elements>
class StructureOfArraysBase
{
	// SOA������Ԫ�ظ���
	static constexpr const size_t kArrayCount = sizeof...(Elements);

public:
	using SoA = StructureOfArraysBase<Allocator, Elements ...>;

	// ��ȡ��N��Ԫ�ص�����
	template<size_t N>
	using TypeAt = typename std::tuple_element<N, std::tuple<Elements...>>::type;

	// SOA�������
	static constexpr size_t getArrayCount() noexcept { return kArrayCount; }

	// Size needed to store "size" array elements
	static size_t getNeededSize(size_t size) noexcept
	{
		return getOffset(kArrayCount - 1, size) + sizeof(TypeAt<kArrayCount - 1>) * size;
	}

	// --------------------------Iterator-----------------------------------------------------

	// Ԥ����Structure��Iterator
	class Structure;
	template<typename T> class Iterator;
	using iterator = Iterator<StructureOfArraysBase*>;
	using const_iterator = Iterator<StructureOfArraysBase const*>;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

	// StructureRef��¼��SOA��ָ���Լ���N��Ԫ��
	class StructureRef
	{
		friend class Structure;
		friend iterator;
		friend const_iterator;
		StructureOfArraysBase* const soa;
		size_t const index;

		StructureRef(StructureOfArraysBase* soa, size_t index) : soa(soa), index(index) { }

		template<size_t ... Is>
		StructureRef& assign(Structure const& rhs, std::index_sequence<Is...>);

		template<size_t ... Is>
		StructureRef& assign(Structure&& rhs, std::index_sequence<Is...>) noexcept;

		friend void swap(StructureRef lhs, StructureRef rhs)
		{
			lhs.soa->swap(lhs.index, rhs.index);
		}

	public:
		StructureRef(StructureRef const& rhs) noexcept : soa(rhs.soa), index(rhs.index) { }

		StructureRef& operator=(StructureRef const& rhs);

		StructureRef& operator=(StructureRef&& rhs) noexcept;

		StructureRef& operator=(Structure const& rhs)
		{
			return assign(rhs, std::make_index_sequence<kArrayCount>());
		}

		StructureRef& operator=(Structure&& rhs) noexcept
		{
			return assign(rhs, std::make_index_sequence<kArrayCount>());
		}

		template<size_t I> TypeAt<I> const& get() const { return soa->elementAt<I>(index); }
		template<size_t I> TypeAt<I>& get() { return soa->elementAt<I>(index); }
	};

	// Structure
	class Structure
	{
		friend class StructureRef;
		friend iterator;
		friend const_iterator;
		using Type = std::tuple<typename std::decay<Elements>::type...>;
		Type elements;

		// ͨ��StructureRef�����SOA�ĵ�N��ֵ
		template<size_t ... Is>
		static Type init(StructureRef const& rhs, std::index_sequence<Is...>)
		{
			return Type{ rhs.soa->template elementAt<Is>(rhs.index)... };
		}

		template<size_t ... Is>
		static Type init(StructureRef&& rhs, std::index_sequence<Is...>) noexcept
		{
			return Type{ std::move(rhs.soa->template elementAt<Is>(rhs.index))... };
		}

	public:
		Structure(Structure const& rhs) = default;
		Structure(Structure&& rhs) noexcept = default;
		Structure& operator=(Structure const& rhs) = default;
		Structure& operator=(Structure&& rhs) noexcept = default;

		// Structure����ͨ����ֵ����StruectureRef����ʼ��
		Structure(StructureRef const& rhs)
			: elements(init(rhs, std::make_index_sequence<kArrayCount>())) {}
		Structure(StructureRef&& rhs) noexcept
			: elements(init(rhs, std::make_index_sequence<kArrayCount>())) {}
		Structure& operator=(StructureRef const& rhs) { return operator=(Structure(rhs)); }
		Structure& operator=(StructureRef&& rhs) noexcept { return operator=(Structure(rhs)); }

		// ���Structure���N��Ԫ��ֵ
		template<size_t I> TypeAt<I> const& get() const { return std::get<I>(elements); }
		template<size_t I> TypeAt<I>& get() { return std::get<I>(elements); }
	};

	//����һ��Iterator��������SOA
	template<typename CVQualifiedSOAPointer>
	class Iterator
	{
		friend class StructureOfArraysBase;
		CVQualifiedSOAPointer soa; // don't use restrict, can have aliases if multiple iterators are created
		size_t index;

		Iterator(CVQualifiedSOAPointer soa, size_t index) : soa(soa), index(index) {}

	public:
		using value_type = Structure;
		using reference = StructureRef;
		using difference_type = ptrdiff_t;

		Iterator(Iterator const& rhs) noexcept = default;
		Iterator& operator=(Iterator const& rhs) = default;

		reference operator*() const { return { soa, index }; }
		reference operator*() { return { soa, index }; }
		reference operator[](size_t n) { return *(*this + n); }

		template<size_t I> TypeAt<I> const& get() const { return soa->template elementAt<I>(index); }
		template<size_t I> TypeAt<I>& get() { return soa->template elementAt<I>(index); }

		Iterator& operator++() { ++index; return *this; }
		Iterator& operator--() { --index; return *this; }
		Iterator& operator+=(size_t n) { index += n; return *this; }
		Iterator& operator-=(size_t n) { index -= n; return *this; }
		Iterator operator+(size_t n) const { return { soa, index + n }; }
		Iterator operator-(size_t n) const { return { soa, index - n }; }
		difference_type operator-(Iterator const& rhs) const { return index - rhs.index; }
		bool operator==(Iterator const& rhs) const { return (index == rhs.index); }
		bool operator!=(Iterator const& rhs) const { return (index != rhs.index); }
		bool operator>=(Iterator const& rhs) const { return (index >= rhs.index); }
		bool operator> (Iterator const& rhs) const { return (index > rhs.index); }
		bool operator<=(Iterator const& rhs) const { return (index <= rhs.index); }
		bool operator< (Iterator const& rhs) const { return (index < rhs.index); }

		const Iterator operator++(int) { Iterator it(*this); index++; return it; }
		const Iterator operator--(int) { Iterator it(*this); index--; return it; }
	};

	// SOA����������
	iterator begin() noexcept { return { this, 0u }; }
	iterator end() noexcept { return { this, mSize }; }
	const_iterator begin() const noexcept { return { this, 0u }; }
	const_iterator end() const noexcept { return { this, mSize }; }

	//------------------------------Constructor-------------------------------------
	StructureOfArraysBase() = default;

	explicit StructureOfArraysBase(size_t capacity)
	{
		setCapacity(capacity);
	}

	// SOA��Ӧ�ñ�ֱ�ӿ���
	StructureOfArraysBase(StructureOfArraysBase const& rhs) = delete;
	StructureOfArraysBase& operator=(StructureOfArraysBase const& rhs) = delete;

	StructureOfArraysBase(StructureOfArraysBase&& rhs) noexcept
	{
		using std::swap;
		swap(mCapacity, rhs.mCapacity);
		swap(mSize, rhs.mSize);
		swap(mArrayOffset, rhs.mArrayOffset);
		swap(mAllocator, rhs.mAllocator);
	}

	StructureOfArraysBase& operator=(StructureOfArraysBase&& rhs) noexcept
	{
		if (this != &rhs)
		{
			using std::swap;
			swap(mCapacity, rhs.mCapacity);
			swap(mSize, rhs.mSize);
			swap(mArrayOffset, rhs.mArrayOffset);
			swap(mAllocator, rhs.mAllocator);
		}
		return *this;
	}

	~StructureOfArraysBase()
	{
		destroy_each(0, mSize);
		mAllocator.free(mArrayOffset[0]);
	}

	//---------------------------Operation-----------------------------------------------

	size_t size() const noexcept
	{
		return mSize;
	}

	size_t capacity() const noexcept
	{
		return mCapacity;
	}

	void setCapacity(size_t capacity)
	{
		//���·���һ������
		if (capacity >= mSize)
		{
			const size_t sizeNeeded = getNeededSize(capacity);
			void* buffer = mAllocator.alloc(sizeNeeded);

			// ��Ԫ�ؿ��������ڴ��ַ��
			move_each(buffer, capacity);

			// �ͷžɵ��ڴ�
			std::swap(buffer, mArrayOffset[0]);
			mAllocator.free(buffer);

			mCapacity = capacity;
		}
	}

	// �������
	void ensureCapacity(size_t needed)
	{
		if (UNLIKELY(needed > mCapacity))
		{
			// ���ݲ���
			const size_t capacity = (needed * 3 + 1) / 2;
			setCapacity(capacity);
		}
	}

	void resize(size_t needed)
	{
		ensureCapacity(needed);
		resizeNoCheck(needed);
	}

	void clear() noexcept
	{
		resizeNoCheck(0);
	}


	inline void swap(size_t i, size_t j) noexcept
	{
		forEach([i, j](auto p) {
			std::swap(p[i], p[j]);
		});
	}

	inline void pop_back() noexcept
	{
		if (mSize) {
			destroy_each(mSize - 1, mSize);
			mSize--;
		}
	}

	StructureOfArraysBase& push_back() noexcept
	{
		resize(mSize + 1);
		return *this;
	}

	StructureOfArraysBase& push_back(Elements const& ... args) noexcept
	{
		ensureCapacity(mSize + 1);
		return push_back_unsafe(args...);
	}

	StructureOfArraysBase& push_back(Elements&& ... args) noexcept
	{
		ensureCapacity(mSize + 1);
		return push_back_unsafe(std::forward<Elements>(args)...);
	}

	StructureOfArraysBase& push_back_unsafe(Elements const& ... args) noexcept
	{
		const size_t last = mSize++;
		size_t i = 0;
		// placement newһ���¶���
		int UNUSED dummy[] = {
				(new(getArray<Elements>(i) + last)Elements(args), i++, 0)... };
		return *this;
	}

	StructureOfArraysBase& push_back_unsafe(Elements&& ... args) noexcept
	{
		const size_t last = mSize++;
		size_t i = 0;
		int UNUSED dummy[] = {
				(new(getArray<Elements>(i) + last)Elements(std::forward<Elements>(args)), i++, 0)... };
		return *this;
	}

	// ����ÿ��Ԫ�أ���һ������Ϊ�̶�����������array��ָ��
	template<typename F, typename ... ARGS>
	void forEach(F&& f, ARGS&& ... args)
	{
		size_t i = 0;
		int UNUSED dummy[] = {
				(f(getArray<Elements>(i), std::forward<ARGS>(args)...), i++, 0)... };
	}

	// ����ĳ��Ԫ�ص�����ͷ
	template<size_t ElementIndex>
	TypeAt<ElementIndex>* data() noexcept
	{
		return getArray<TypeAt<ElementIndex>>(ElementIndex);
	}

	template<size_t ElementIndex>
	TypeAt<ElementIndex> const* data() const noexcept
	{
		return getArray<TypeAt<ElementIndex>>(ElementIndex);
	}

	template<size_t ElementIndex>
	TypeAt<ElementIndex>* begin() noexcept
	{
		return getArray<TypeAt<ElementIndex>>(ElementIndex);
	}

	template<size_t ElementIndex>
	TypeAt<ElementIndex> const* begin() const noexcept
	{
		return getArray<TypeAt<ElementIndex>>(ElementIndex);
	}

	template<size_t ElementIndex>
	TypeAt<ElementIndex>* end() noexcept
	{
		return getArray<TypeAt<ElementIndex>>(ElementIndex) + size();
	}

	template<size_t ElementIndex>
	TypeAt<ElementIndex> const* end() const noexcept
	{
		return getArray<TypeAt<ElementIndex>>(ElementIndex) + size();
	}

	// ���ĳԪ�������index��ֵ
	template<size_t ElementIndex>
	TypeAt<ElementIndex>& elementAt(size_t index) noexcept
	{
		return data<ElementIndex>()[index];
	}

	template<size_t ElementIndex>
	TypeAt<ElementIndex> const& elementAt(size_t index) const noexcept
	{
		return data<ElementIndex>()[index];
	}

	// ����Ԫ��βֵ
	template<size_t ElementIndex>
	TypeAt<ElementIndex>& back() noexcept
	{
		return data<ElementIndex>()[size() - 1];
	}

	template<size_t ElementIndex>
	TypeAt<ElementIndex> const& back() const noexcept
	{
		return data<ElementIndex>()[size() - 1];
	}

private:

	template<typename T>
	T const* getArray(size_t arrayIndex) const
	{
		return static_cast<T const*>(mArrayOffset[arrayIndex]);
	}

	template<typename T>
	T* getArray(size_t arrayIndex)
	{
		return static_cast<T*>(mArrayOffset[arrayIndex]);
	}

	// ����ÿ��Ԫ�ط�����ڴ��offset
	static inline size_t getOffset(size_t index, size_t capacity) noexcept
	{
		auto offsets = getOffsets(capacity);
		return offsets[index];
	}

	static inline std::array<size_t, kArrayCount> getOffsets(size_t capacity) noexcept
	{
		// ����ÿ��Ԫ����Ҫ��������С
		const size_t sizes[] = { (sizeof(Elements) * capacity)... };

		// ����malloc���ص�alligan�Ķ���size
		const size_t align = alignof(std::max_align_t);

		std::array<size_t, kArrayCount> offsets;
		offsets[0] = 0;

		for (size_t i = 1; i < kArrayCount; i++) {
			// ��Ҫ�����ϸ�Ԫ�ط�����ڴ�������
			size_t unalignment = sizes[i - 1] % align;
			size_t alignment = unalignment ? (align - unalignment) : 0;
			offsets[i] = offsets[i - 1] + (sizes[i - 1] + alignment);
		}
		return offsets;
	}

	//ǿ��resize��ָ����С
	inline void resizeNoCheck(size_t needed) noexcept {
		assert(mCapacity >= needed);
		if (needed < mSize) {
			// ����
			destroy_each(needed, mSize);
		}
		else if (needed > mSize) {
			// ����
			construct_each(mSize, needed);
		}
		// ����size
		mSize = needed;
	}

	void construct_each(size_t from, size_t to) noexcept
	{
		forEach([from, to](auto p) {
			using T = typename std::decay<decltype(*p)>::type;
			for (size_t i = from; i < to; i++) {
				new(p + i) T();
			}
		});
	}

	// ���component��Ķ���
	void destroy_each(size_t from, size_t to) noexcept
	{
		forEach([from, to](auto p) {
			using T = typename std::decay<decltype(*p)>::type;
			for (size_t i = from; i < to; i++) {
				p[i].~T();
			}
		});
	}

	void move_each(void* buffer, size_t capacity) noexcept {
		auto offsets = getOffsets(capacity);
		size_t index = 0;
		if (mSize) {
			auto size = mSize;
			forEach([buffer, &index, &offsets, size](auto p) {
				using T = typename std::decay<decltype(*p)>::type;
				T* RESTRICT b = static_cast<T*>(buffer);

				// ��Ԫ�صĴ�buffer�����ݿ�������
				// �õ���Ԫ�ص�����ָ���ַ
				T* RESTRICT const arrayPointer =
					reinterpret_cast<T*>(uintptr_t(b) + offsets[index]);

				// ����size��С
				if (std::is_trivially_copyable<T>::value &&
					std::is_trivially_destructible<T>::value) {
					memcpy(arrayPointer, p, size * sizeof(T));
				}
				else {
					for (size_t i = 0; i < size; i++) {
						new(arrayPointer + i) T(std::move(p[i]));
						p[i].~T();
					}
				}
				index++;
			});
		}

		// ��������ָ��ƫ��
		for (size_t i = 1; i < kArrayCount; i++) {
			mArrayOffset[i] = (char*)buffer + offsets[i];
		}
	}

	size_t mCapacity = 0;
	size_t mSize = 0;
	// ÿ��Ԫ�������ָ��Ϊ��֮ 
	void *mArrayOffset[kArrayCount] = { nullptr };
	Allocator mAllocator;
};


template<typename Allocator, typename... Elements>
inline
	typename StructureOfArraysBase<Allocator, Elements...>::StructureRef&
	StructureOfArraysBase<Allocator, Elements...>::StructureRef::operator=(
		StructureOfArraysBase::StructureRef const& rhs) {
	return operator=(Structure(rhs));
}

template<typename Allocator, typename... Elements>
inline
	typename StructureOfArraysBase<Allocator, Elements...>::StructureRef&
	StructureOfArraysBase<Allocator, Elements...>::StructureRef::operator=(
		StructureOfArraysBase::StructureRef&& rhs) noexcept {
	return operator=(Structure(rhs));
}

template<typename Allocator, typename... Elements>
template<size_t... Is>
inline
	typename StructureOfArraysBase<Allocator, Elements...>::StructureRef&
	StructureOfArraysBase<Allocator, Elements...>::StructureRef::assign(
		StructureOfArraysBase::Structure const& rhs, std::index_sequence<Is...>) {
	auto l = { (soa->elementAt<Is>(index) = std::get<Is>(rhs.elements), 0)... };
	return *this;
}

template<typename Allocator, typename... Elements>
template<size_t... Is>
inline
	typename StructureOfArraysBase<Allocator, Elements...>::StructureRef&
	StructureOfArraysBase<Allocator, Elements...>::StructureRef::assign(
		StructureOfArraysBase::Structure&& rhs, std::index_sequence<Is...>) noexcept {
	auto l = {
			(soa->elementAt<Is>(index) = std::move(std::get<Is>(rhs.elements)), 0)... };
	return *this;
}

template<typename ... Elements>
using StructureOfArrays = StructureOfArraysBase < AllocatorBase, Elements...>;

}
}