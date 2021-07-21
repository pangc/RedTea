#include "world.h"

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
		
	}

	World::World()
	{
		mEntityManger = new EntityManager();
	}

	World::~World()
	{
		for (auto section : sections)
		{
			section->Destroy();
			delete section;
		}

		delete mEntityManger;
	}

	Section* World::CreateSection()
	{
		Section* section = new Section(sectionIndex++, this);
		sections.emplace_back(section);
		return section;
	}

}
}