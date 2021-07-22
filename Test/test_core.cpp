#include "../Engine/Core/world.h"
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
