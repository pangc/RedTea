#pragma once
#include <vector>
#include <deque>
#include "entity.h"
#include <mutex>

namespace redtea {
namespace core {

class EntityManager
{
public:
    Entity CreateEntity();
    void InitEntity(int n, Entity* e);
    void DestroyEntitys(int n, Entity* e);
	void DestroyEntity(Entity e);
    ~EntityManager();
    
public:
    std::deque<Entity::Type> mFreeList;
	Entity::Type mCurrentID;
    mutable std::mutex mFreeListLock;
};

}
}
