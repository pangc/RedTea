#pragma once
#include "entity.h"
#include "entity_manager.h"
#include <vector>

namespace redtea {
namespace core {

	class World;
	class Section
	{
	public:
		Section() = delete;
		Section(uint32_t id, World* world);
		Entity CreateEntity();
		void Destroy();
	private:
		friend class World;
		std::vector<Entity> mEntities;
		std::string mName;
		uint32_t mId;
		World* mWorld;
	};

	class World
	{
	public:
		World();
		~World();
		Section* CreateSection();
		inline EntityManager* GetEntityManager() { return mEntityManger; }
	private:
		std::vector<Section*> sections;
		EntityManager* mEntityManger;
		uint32_t sectionIndex;
	};
}
}