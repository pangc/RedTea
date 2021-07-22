#include "world.h"
#include "common.h"

namespace redtea {
namespace core {

	Section::Section(uint32_t id, World* world)
	: mId(id),
	mWorld(world)
	{

	}

	Entity Section::CreateEntity()
	{
		Entity e = mWorld->GetEntityManager()->CreateEntity();
		mEntities.emplace_back(e);
		return e;
	}

	void Section::Destroy()
	{
		// batch delete entity
		int size = mEntities.size();
		mWorld->GetEntityManager()->DestroyEntitys(size, mEntities.data());
	}

	void Section::SetActive()
	{
		mWorld->SetActiveSection(mId);
	}

	World::World()
	{
		mEntityManger = new EntityManager();
	}

	World::~World()
	{
		for (auto section : mSections)
		{
			section->Destroy();
			delete section;
		}

		delete mEntityManger;
	}

	Section* World::CreateSection()
	{
		Section* section = new Section(mSectionIndex++, this);
		mSections.emplace_back(section);
		return section;
	}

	Section* World::GetActiveSection()
	{
		Section* p = GetSectionById(curActiveSection);
		assert(p);
		return p;
	}

	Section* World::GetSectionById(uint32_t id)
	{
		for (auto const section : mSections)
		{
			if (section->mId == id)
			{
				return section;
			}
		}
		return nullptr;
	}

	void World::SetActiveSection(uint32_t id)
	{
		curActiveSection = id;
	}

}
}