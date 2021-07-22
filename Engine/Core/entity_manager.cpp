#include "entity_manager.h"
#include <mutex>
#include "common.h"

namespace redtea {
namespace core {

Entity EntityManager::CreateEntity() {
    Entity e;
    InitEntity(1, &e);
    return e;
}

void EntityManager::InitEntity(int n, redtea::core::Entity *e) {
    // make thread safe
    std::lock_guard<std::mutex> lock(mFreeListLock);
    for(int i = 0; i < n; i++)
    {
		Entity::Type id;
		if (mFreeList.empty())
		{
			id = mCurrentID++;
			
		}
		else
		{
			id = mFreeList.front();
			mFreeList.pop_front();
		}
		e[i].SetId(id);
		e[i].SetManager(this);
    }
}


void EntityManager::DestroyEntitys(int n, Entity* e)
{
	auto& freeList = mFreeList;
	std::unique_lock<std::mutex> lock(mFreeListLock);
	for (int i = 0; i < n; i++)
	{
		freeList.push_back(e[i].GetId());
	}
	lock.unlock();

	// broadcast entity destroy
	// todo:event listener
}


void EntityManager::DestroyEntity(Entity e)
{
	DestroyEntitys(1, &e);
}


EntityManager::~EntityManager()
{

}

}
}
