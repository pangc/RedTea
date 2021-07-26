#pragma once
#include "utils/struct_of_arrays.h"
#include "entity.h"
#include "component.h"
#include <unordered_map>

namespace redtea {
namespace core {

template <typename ... Elements>
class  ComponentManagerBase
{
protected:
	static constexpr size_t ENTITY_INDEX = sizeof ... (Elements);
private:
	using SoA = common::StructureOfArrays<Elements ..., Entity>;
	using Instance = ComponentInstance::Type;
protected:
	SoA mData;
	std::unordered_map<Entity, Instance> mInstanceMap;

public:
	ComponentManagerBase() noexcept
	{
		// ��֤mData��������������Index��1��ʼ
		mData.push_back();
	}

	ComponentManagerBase(ComponentManagerBase&& rhs) noexcept {/* = default */ }
	ComponentManagerBase& operator=(ComponentManagerBase&& rhs) noexcept {/* = default */ }
	~ComponentManagerBase() noexcept = default;

	// not copyable
	ComponentManagerBase(ComponentManagerBase const& rhs) = delete;
	ComponentManagerBase& operator=(ComponentManagerBase const& rhs) = delete;

	Instance GetInstance(Entity e) const noexcept
	{
		auto const& map = mInstanceMap;
		auto pos = map.find(e);
		return pos != map.end() ? pos->second : 0;
	}

	bool HasComponent(Entity e) const noexcept
	{
		return GetInstance(e) != 0;
	}

	size_t GetComponentCount() const noexcept
	{
		// 1..size - 1���������������
		return mData.size() - 1;
	}

	inline Instance AddComponent(Entity e);

	inline Instance RemoveComponent(Entity e);

	bool Empty() const noexcept
	{
		return GetComponentCount() == 0;
	}

	Entity GetEntity(Instance i) const noexcept {
		return GetElement<ENTITY_INDEX>(i);
	}


	// ����Instance��SOA���õ�N��Ԫ��
	template<size_t ElementIndex>
	typename SoA::template TypeAt<ElementIndex>& GetElement(Instance index) noexcept {
		assert(index);
		return data<ElementIndex>()[index];
	}

	template<size_t ElementIndex>
	typename SoA::template TypeAt<ElementIndex> const& GetElement(Instance index) const noexcept {
		assert(index);
		return data<ElementIndex>()[index];
	}

	template<size_t ElementIndex>
	typename SoA::template TypeAt<ElementIndex> const* GetRawArray() const noexcept {
		return data<ElementIndex>();
	}

	template<size_t E>
	struct Field : public SoA::template Field<E, Instance> {
		Field(ComponentManagerBase& soa, Instance i) noexcept
			: SoA::template Field<E, Instance>{ soa.mData, i } {
		}
		using SoA::template Field<E, Instance>::operator =;
	};

protected:
	// ��SOA��ȡ����N������
	template<size_t ElementIndex>
	typename SoA::template TypeAt<ElementIndex>* data() noexcept {
		return mData.template data<ElementIndex>();
	}

	template<size_t ElementIndex>
	typename SoA::template TypeAt<ElementIndex> const* data() const noexcept {
		return mData.template data<ElementIndex>();
	}

};

template<typename ... Elements>
typename ComponentManagerBase<Elements ...>::Instance
ComponentManagerBase<Elements ...>::AddComponent(Entity e) {
	Instance ci = 0;
	if (!HasComponent(e)) {
		// ����һ������
		mData.push_back().template back<ENTITY_INDEX>() = e;
		ci = Instance(mData.size() - 1);
		mInstanceMap[e] = ci;
	}
	else {
		// ��֧��Entity��Ӧ���Component
		ci = mInstanceMap[e];
	}
	assert(ci != 0);
	return ci;
}

template <typename ... Elements>
typename ComponentManagerBase<Elements ...>::Instance
ComponentManagerBase<Elements ... >::RemoveComponent(Entity e) {
	auto& map = mInstanceMap;
	auto pos = map.find(e);
	if (LIKELY(pos != map.end())) {
		size_t index = pos->second;
		assert(index != 0);
		size_t last = mData.size() - 1;
		if (last != index) {
			// �ƶ����һ��Entity��䱻ɾ��������
			mData.forEach([index, last](auto* p) {
				p[index] = std::move(p[last]);
			});

			Entity lastEntity = mData.template elementAt<ENTITY_INDEX>(index);
			// ����Instance
			map[lastEntity] = index;
		}
		mData.pop_back();
		map.erase(pos);
		return last;
	}
	return 0;
}

}
}