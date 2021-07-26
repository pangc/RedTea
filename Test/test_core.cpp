#include "../Engine/Core/world.h"
#include "../Engine/Core/component_manager.h"
#include "../Engine/Core/entity.h"
#include <gtest/gtest.h>

TEST(CORE_TEST, world)
{
	using namespace redtea::core;
	World world;
	auto sa = world.CreateSection();
	auto sb = world.CreateSection();
	auto sc = world.CreateSection();
	sa->SetActive();
	sa->CreateEntity();
	sa->CreateEntity();

	auto cur_active = world.GetActiveSection();
	EXPECT_EQ(sa->GetId(), cur_active->GetId());
}

TEST(CORE_TEST, component_manager)
{
	using namespace redtea::core;
	struct Vector3
	{
		float x;
		float y;
		float z;
	};

	class MoveManager : public ComponentManagerBase<Vector3, Vector3>
	{
		enum
		{
			Direction,
			Position
		};
	};

	World world;
	MoveManager manager;
	auto section = world.CreateSection();
	auto e = section->CreateEntity();
	manager.AddComponent(e);
	EXPECT_EQ(manager.HasComponent(e), true);
	manager.RemoveComponent(e);
	EXPECT_EQ(manager.HasComponent(e), false);
}
