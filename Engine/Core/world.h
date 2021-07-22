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
		void SetActive();
		inline uint32_t GetId() { return mId; }
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
		Section* GetActiveSection();
		Section* GetSectionById(uint32_t id);
		void SetActiveSection(uint32_t id);
	private:
		std::vector<Section*> mSections;
		EntityManager* mEntityManger;
		uint32_t mSectionIndex;
		uint32_t curActiveSection;
	};
}
}