#pragma once
#include <cstdint>
#include <functional>   // for std::hash

namespace redtea {
namespace core {

class Entity
{
	friend struct std::hash<Entity>;
	friend class EntityManager;
    enum class State : uint8_t
    {
        Init,
        Active,
        Deactive,
        Destroy
    };
public:
    using Type = uint32_t;
    
    Entity() noexcept = default;
    Entity(const Entity& e) noexcept = default;
    Entity(Entity&& e) noexcept = default;
    Entity& operator=(const Entity& e) noexcept = default;
    Entity& operator=(Entity&& e) noexcept = default;
    
	// Entities can be compared
	bool operator==(Entity e) const { return e.mId == mId; }
	bool operator!=(Entity e) const { return e.mId != mId; }

	// Entities can be sorted
	bool operator<(Entity e) const { return e.mId < mId; }

    void Active();
    void Deactive();
	inline void SetId(Type id) { mId = id; };
	inline Type GetId() const noexcept { return mId; }
	inline void SetManager(EntityManager* m) { mManager = m; };

private:
	explicit Entity(Type identity) noexcept : mId(identity) { }

private:
    Type mId;
    State mState;
	EntityManager* mManager;
};

}
}

namespace std {

template<>
struct hash<redtea::core::Entity> {
	typedef redtea::core::Entity argument_type;
	typedef size_t result_type;
	result_type operator()(argument_type const& e) const {
		return e.GetId();
	}
};
}