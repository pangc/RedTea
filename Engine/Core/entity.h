#pragma once
#include <cstdint>

namespace redtea {
namespace core {

class Entity
{
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
    
    void Active();
    void Deactive();
	inline void SetId(Type id) { mId = id; };
	inline Type GetId() { return mId; }
	inline void SetManager(EntityManager* m) { mManager = m; };
public:
    Type mId;
    State mState;
	EntityManager* mManager;
};

}
}
